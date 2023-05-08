/***************************************************************************/
/**
 *  \file       driver.c
 *
 *  \details    Simple I2C driver explanation (SSD_1306 OLED Display Interface)
 *
 *  \author     EmbeTronicX
 *
 *  \Tested with Linux raspberrypi 5.4.51-v7l+
 *
 * *******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/jiffies.h>

/*------------chip registers-------------*/
#define BANK1

#ifdef BANK1

#define IODIRA 0x00   // direction register 1-input, 0-output
#define IODIRB 0x10   // direction register 1-input, 0-output
#define IPOLA 0x01    // polarity register 0-noninverted, 1-inverted
#define IPOLB 0x11    // polarity register 0-noninverted, 1-inverted
#define GPINTENA 0x02 // interrupt on change feature 1-corresponding pin is enabled for interrupt-on-change
#define GPINTENB 0x12 // interrupt on change feature 1-corresponding pin is enabled for interrupt-on-change
#define DEFVALA 0x03  // default compare value register
#define DEFVALB 0x13
#define INTCONA 0x05 // interrupt control register 1-IO pin compared to DEFVAL, 0-IO pin compared to previous value
#define INTCONB 0x14
#define IOCON 0x05 // CONFIGURATION REGISTER   0x15 WHEN BANK0
#define GPPUA 0x06 // 1-Pull-up enabled, 0-Pull-up disabled
#define GPPUB 0x16
#define INTFA 0x07 // interrupt condition 1- pin caused interrupt
#define INTFB 0x17
#define INTCAPA 0x08 // captures the GPIO port value at time the interrupt occured.The register remains unchanged until theinterrupt is cleared via a read of INTCAP or GPIO.
#define INTCAPB 0x18
#define GPIOA 0x09 // The GPIO register reflects the value on the port.
                   // Reading from this register reads the port. Writing to this
                   // register modifies the Output Latch (OLAT) register.
#define GPIOB 0x19
#define OLATA 0x0A // The OLAT register provides access to the output
                   // latches. A read from this register results in a read of the
                   // OLAT and not the port itself. A write to this register
                   // modifies the output latches that modifies the pins
                   // configured as outputs.
#define OLATB 0x1A

#else

#define IODIRA 0x00
#define IODIRB 0x01
#define IPOLA 0x02
#define IPOLB 0x03
#define GPINTENA 0x04
#define GPINTENB 0x05
#define DEFVALA 0x06
#define DEFVALB 0x07
#define INTCONA 0x08
#define INTOCNB 0x09
#define IOCON 0x0A // 0x0B
#define GPPUA 0x0C
#define GPPUB 0x0D
#define INTFA 0x0E
#define INTFB 0x0F
#define INTCAPA 0x10
#define INTCAPB 0x11
#define GPIOA 0x12
#define GPIOB 0x13
#define OLATA 0x14
#define OLATB 0x15

#endif
/*---------------------------------------*/
/*-------IOCON REGISTER BITS-------------*/
#define BANK_IOCON (1 << 7)   // The registers associated with each port are separated into different banks
#define MIRROR_IOCON (1 << 6) // interrupt pins internally connected
#define SEQOP_IOCON (1 << 5)  // Sequential operation disabled, address pointer does not increment.
#define DISSLW_IOCON (1 << 4) // SDA Output, Slew rate disabled
#define HAEN_IOCON (1 << 3)   // Enables the MCP23S17 address pins.
#define ODR_IOCON (1 << 2)    // Configures the INT pin as an open-drain output
#define INTPOL_IOCON (1 << 1) // Interrupt pin Active-HIGH
/*--------------------------------------*/

#define I2C_BUS_AVAILABLE (1)          // I2C Bus available in our Raspberry Pi
#define SLAVE_DEVICE_NAME ("mcp23017") // Device and Driver Name
#define SLAVE_ADDR (0x08)              // should be changed

static struct i2c_adapter *mcp23017_i2c_adapter = NULL; // I2C Adapter Structure
static struct i2c_client *mcp23017_i2c_client = NULL;   // I2C Cient Structure (In our case it is mcp23017)

dev_t dev = 0;
static struct class *dev_class;
static struct cdev mcp23017_cdev;

// interrupt pins, needs to be configured
#define INTA_PIN (17)
#define INTB_PIN (27)

// This used for storing the IRQ number for the GPIO
unsigned int INTA_irqNumber;
unsigned int INTB_irqNumber;

// This variables store values on ports
// that is updated on every occurred interrupt
#define mem_size        128           //Memory Size
unsigned char *gpio_state_buf;

// Interrupt handler for INTA_irqNumber This will be called whenever
//  there is a raising edge detected.
static irqreturn_t INTA_irq_handler(int irq, void *dev_id)
{
  static unsigned long flags = 0;

  // if data_status < 0 it meas error has occurred during reading
  // else data is written to it
  int data_status = 0;

  local_irq_save(flags);
  // Reads byte from GPIOA register
  data_status = i2c_smbus_read_byte_data(mcp23017_i2c_client, GPIOA);
  local_irq_restore(flags);

  // updating gpio_state_buf[0]
  gpio_state_buf[0] = (data_status < 0) ? gpio_state_buf[0] : data_status;

  return IRQ_HANDLED;
}

// Interrupt handler for INTB_irqNumber This will be called whenever
//  there is a raising edge detected.
static irqreturn_t INTB_irq_handler(int irq, void *dev_id)
{
  static unsigned long flags = 0;

  // if data_status < 0 it meas error has occurred during reading
  // else data is written to it
  int data_status = 0;

  local_irq_save(flags);
  // Reads byte from GPIOA register
  data_status = i2c_smbus_read_byte_data(mcp23017_i2c_client, GPIOB);
  local_irq_restore(flags);

  // updating gpio_state_buf[0]
  gpio_state_buf[1] = (data_status < 0) ? gpio_state_buf[1] : data_status;

  return IRQ_HANDLED;
}

/*
** This function getting called when the slave has been found
** Note : This will be called only once when we load the driver.
*/
static int mcp23017_probe(struct i2c_client *client,
                          const struct i2c_device_id *id)
{
  pr_info("mcp23017 Probed!!!\n");

  return 0;
}

/*
** This function getting called when the slave has been removed
** Note : This will be called only once when we unload the driver.
*/
static void mcp23017_remove(struct i2c_client *client)
{
  pr_info("mcp23017 Removed!!!\n");
}

/*
** Structure that has slave device id
*/
static const struct i2c_device_id mcp23017_id[] = {
    {SLAVE_DEVICE_NAME, 0},
    {}};
MODULE_DEVICE_TABLE(i2c, mcp23017_id);

/*
** I2C driver Structure that has to be added to linux
*/
static struct i2c_driver mcp23017_driver = {
    .driver = {
        .name = SLAVE_DEVICE_NAME,
        .owner = THIS_MODULE,
    },
    .probe = mcp23017_probe,
    .remove = mcp23017_remove,
    .id_table = mcp23017_id,
};

/*
** I2C Board Info strucutre
*/
static struct i2c_board_info mcp23017_i2c_board_info = {
    I2C_BOARD_INFO(SLAVE_DEVICE_NAME, SLAVE_ADDR)};

/*
** This function will be called when we open the Device file
*/
static int mcp23017_open(struct inode *inode, struct file *file)
{
  pr_info("Device File Opened...!!!\n");
  return 0;
}

/*
** This function will be called when we close the Device file
*/
static int mcp23017_release(struct inode *inode, struct file *file)
{
  pr_info("Device File Closed...!!!\n");
  return 0;
}
/*
** This function will be called when we read from the Device file
*/
static ssize_t mcp23017_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
  if (copy_to_user(buf, gpio_state_buf, mem_size))
  {
    pr_err("Data read error!");
  }
  printk(KERN_INFO "Data Read : Done!\n");
  return mem_size;
}
/*
** This function will be called when we read to the Device file
*/
static ssize_t mcp23017_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
  if (copy_from_user(gpio_state_buf, buf, len))
  {
    pr_err("Data write error!");
  }
  /*--------------test purpose only for arduino slave--------------*/
  i2c_smbus_write_byte(mcp23017_i2c_client, gpio_state_buf[0]);
  printk(KERN_INFO "Data Write : Done! %c%c\n", gpio_state_buf[0], gpio_state_buf[1]);
  return len;
}
// File operation structure
static struct file_operations fops =
    {
        .owner = THIS_MODULE,
        .open = mcp23017_open,
        .release = mcp23017_release,
        .write = mcp23017_write,
        .read = mcp23017_read};

/*
** Module Init function
*/
static int __init mcp23017_driver_init(void)
{
  /*Allocating Major number*/
  if ((alloc_chrdev_region(&dev, 0, 1, "mcp23017_dev")) < 0)
  {
    pr_err("Cannot allocate major number\n");
    goto r_unreg;
  }
  pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

  /*Creating cdev structure*/
  cdev_init(&mcp23017_cdev, &fops);

  /*Adding character device to the system*/
  if ((cdev_add(&mcp23017_cdev, dev, 1)) < 0)
  {
    pr_err("Cannot add the device to the system\n");
    goto r_del;
  }

  /*Creating struct class*/
  if (IS_ERR(dev_class = class_create(THIS_MODULE, "mcp23017_class")))
  {
    pr_err("Cannot create the struct class\n");
    goto r_class;
  }

  /*Creating device*/
  if (IS_ERR(device_create(dev_class, NULL, dev, NULL, "mcp23017_device")))
  {
    pr_err("Cannot create the Device line:%d",__LINE__);
    goto r_device;
  }
  /*-------------instantiation of i2c device------------------*/
  mcp23017_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
  if (mcp23017_i2c_adapter == NULL)
  {
    pr_err("Error during instantiation of i2c adapter line:%d",__LINE__);
    goto r_device;
  }

  mcp23017_i2c_client = i2c_new_client_device(mcp23017_i2c_adapter, &mcp23017_i2c_board_info);
  if (mcp23017_i2c_client == NULL)
  {
    pr_err("Error during instantiation of i2c device line:%d",__LINE__);
    i2c_put_adapter(mcp23017_i2c_adapter);
    goto r_device;
  }

  i2c_add_driver(&mcp23017_driver);
  pr_info("I2C driver added!");
  i2c_put_adapter(mcp23017_i2c_adapter);

  // Interrupt A GPIO configuration
  // Checking the GPIO is valid or not
  if (gpio_is_valid(INTA_PIN) == false)
  {
    pr_err("GPIO %d is not valid\n", INTA_PIN);
    goto r_i2c_device;
  }

  // Requesting the GPIO
  if (gpio_request(INTA_PIN, "INTA_PIN") < 0)
  {
    pr_err("ERROR: GPIO %d request\n", INTA_PIN);
    goto r_gpioa_pin;
  }

  // configure the GPIO as input
  gpio_direction_input(INTA_PIN);

  // Get the IRQ number for our GPIO
  INTA_irqNumber = gpio_to_irq(INTA_PIN);
  pr_info("INTA_irqNumber = %d\n", INTA_irqNumber);

  if (request_irq(INTA_irqNumber,           // IRQ number
                  (void *)INTA_irq_handler, // IRQ handler
                  IRQF_TRIGGER_RISING,      // Handler will be called in raising edge
                  "INTA",                   // used to identify the device name using this IRQ
                  NULL))
  { // device id for shared IRQ
    pr_err("my_device: cannot register IRQ ");
    goto r_gpioa_pin;
  }

  // Interrupt B GPIO configuration
  // Checking the GPIO is valid or not
  if (gpio_is_valid(INTB_PIN) == false)
  {
    pr_err("GPIO %d is not valid\n", INTB_PIN);
    goto r_gpioa_pin;
  }

  // Requesting the GPIO
  if (gpio_request(INTB_PIN, "INTB_PIN") < 0)
  {
    pr_err("ERROR: INTB %d request\n", INTB_PIN);
    goto r_gpiob_pin;
  }

  // configure the GPIO as input
  gpio_direction_input(INTB_PIN);

  // Get the IRQ number for our GPIO
  INTB_irqNumber = gpio_to_irq(INTB_PIN);
  pr_info("INTB_irqNumber = %d\n", INTB_irqNumber);

  if (request_irq(INTB_irqNumber,           // IRQ number
                  (void *)INTB_irq_handler, // IRQ handler
                  IRQF_TRIGGER_RISING,      // Handler will be called in raising edge
                  "INTB",                   // used to identify the device name using this IRQ
                  NULL))
  { // device id for shared IRQ
    pr_err("my_device: cannot register IRQ ");
    goto r_gpiob_pin;
  }

  /*Creating Physical memory*/
  if ((gpio_state_buf = kmalloc(mem_size, GFP_KERNEL)) == 0)
  {
    pr_info("Cannot allocate memory in kernel\n");
    goto r_gpiob_pin;
  }

  pr_info("Device Driver Insert...Done!!!\n");

  /*-------------configuration of device registers-----------------*/
  i2c_smbus_write_byte_data(mcp23017_i2c_client,IOCON,   0b10100010);
  i2c_smbus_write_byte_data(mcp23017_i2c_client,IODIRA,  0b01111111);
  i2c_smbus_write_byte_data(mcp23017_i2c_client,IODIRB,  0b01111111);
  i2c_smbus_write_byte_data(mcp23017_i2c_client,IPOLA,   0b00000000);
  i2c_smbus_write_byte_data(mcp23017_i2c_client,IPOLB,   0b00000000);
  i2c_smbus_write_byte_data(mcp23017_i2c_client,GPINTENA,0b01111111);
  i2c_smbus_write_byte_data(mcp23017_i2c_client,GPINTENB,0b01111111);
  i2c_smbus_write_byte_data(mcp23017_i2c_client,DEFVALA, 0b00000000);
  i2c_smbus_write_byte_data(mcp23017_i2c_client,DEFVALB, 0b00000000);
  i2c_smbus_write_byte_data(mcp23017_i2c_client,INTCONA, 0b00000000);
  i2c_smbus_write_byte_data(mcp23017_i2c_client,INTCONB, 0b00000000);
  i2c_smbus_write_byte_data(mcp23017_i2c_client,GPPUA,   0b00000000);
  i2c_smbus_write_byte_data(mcp23017_i2c_client,GPPUB,   0b00000000);
/*-----------------------------------------------------------------*/

  return 0;

r_gpiob_pin:
  gpio_free(INTB_PIN);
r_gpioa_pin:
  gpio_free(INTA_PIN);
r_i2c_device:
  i2c_del_driver(&mcp23017_driver);
r_device:
  device_destroy(dev_class, dev);
r_class:
  class_destroy(dev_class);
r_del:
  cdev_del(&mcp23017_cdev);
r_unreg:
  unregister_chrdev_region(dev, 1);

  return -1;
}

/*
** Module Exit function
*/
static void __exit mcp23017_driver_exit(void)
{
  kfree(gpio_state_buf);
  free_irq(INTB_irqNumber, NULL);
  free_irq(INTA_irqNumber, NULL);
  gpio_free(INTA_PIN);
  gpio_free(INTB_PIN);
  device_destroy(dev_class, dev);
  class_destroy(dev_class);
  cdev_del(&mcp23017_cdev);
  unregister_chrdev_region(dev, 1);
  i2c_unregister_device(mcp23017_i2c_client);
  i2c_del_driver(&mcp23017_driver);
  pr_info("Driver Removed!!!\n");
}

module_init(mcp23017_driver_init);
module_exit(mcp23017_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikola Cetic nikolacetic8@gmail.com");
MODULE_DESCRIPTION("MCP23017 driver that uses I2C protocol!");
MODULE_VERSION("1.0");