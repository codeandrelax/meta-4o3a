#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/delay.h>

#define LED_PIN 20
#define NUM_DEV 26
#define NUM_LEDS NUM_DEV*3
#define DRIVER_NAME "leddriver"
#define DRIVER_CLASS "ModuleClass"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nemanja Cenic and Nikola Cetic");
MODULE_DESCRIPTION("LED driver");

const int pTime = 1;
int tCycle = pTime * 6;

static char buffer[20];
int value = 0;

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

void writeLED(char, char, char, char);
void writeZero(void);
void writeOne(void);
void writeCommTimer(void);
void writeCommand(void);
void waitEOS(void);
void waitGSLAT(void);
void writeData(char);

void writeLED(char dev_num, char red, char green, char blue) {
  for (int i = 0; i < NUM_DEV; i++) {
    writeCommTimer();
    
    if (i == dev_num){
      	writeCommand();
	writeData(blue);
	writeData(green);
	writeData(red);
    }
    waitEOS();
  }
  waitGSLAT();
}

void writeZero(void) {
  gpio_set_value(LED_PIN, 1);
  udelay(pTime);
  //nanosleep((const struct timespec[]){{0, pTime*1000}}, NULL);
  gpio_set_value(LED_PIN, 0);
  udelay(pTime*5);
  //nanosleep((const struct timespec[]){{0, pTime*5*1000}}, NULL); 
}

void writeOne(void) {
  gpio_set_value(LED_PIN, 1);
  udelay(pTime);
  //nanosleep((const struct timespec[]){{0, pTime*1000}}, NULL);
  gpio_set_value(LED_PIN, 0);
  udelay(pTime);
  //nanosleep((const struct timespec[]){{0, pTime*1000}}, NULL);
  gpio_set_value(LED_PIN, 1);
  udelay(pTime);
  //nanosleep((const struct timespec[]){{0, pTime*1000}}, NULL);
  gpio_set_value(LED_PIN, 0);
  udelay(pTime*3);
  //nanosleep((const struct timespec[]){{0, pTime*3*1000}}, NULL);
}

void writeCommTimer(void) {
  writeZero();
  writeZero();
}

void writeCommand(void) {
  writeOne();
  writeOne();
  
  writeOne();
  writeZero();
  writeOne();
  writeZero();
}

//end of sequence (for a single driver chip)
void waitEOS(void) {
  gpio_set_value(LED_PIN, 0);
  udelay(tCycle*4);
  //nanosleep((const struct timespec[]){{0, tCycle*4*1000}}, NULL); // min 3.5 to max 5.5 times tCycle
}

//grayscale latch (for the end of a chain of driver chips)
void waitGSLAT(void) {
  gpio_set_value(LED_PIN, 0);
  udelay(tCycle*10);
  //nanosleep((const struct timespec[]){{0, tCycle*10*1000}}, NULL); // minimum 8 time tCycle
}

void writeData(char data) {
  for (char i = 0; i<8; i++) {
    if(data & 0b10000000)
		writeOne();
    else
		writeZero();
    data <<= 1;
  }
}

static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(buffer));

	/* Copy data to user */
	not_copied = copy_to_user(user_buffer, buffer, to_copy);

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief Write data to buffer
 */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta, values[4];

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(buffer));

	/* Copy data to user */
	not_copied = copy_from_user(buffer, user_buffer, to_copy);
	
	sscanf(buffer, "%d %d %d %d", &values[0], &values[1], &values[2], &values[3]);
	writeLED(values[0], values[1], values[2], values[3]);

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev_nr - open was called!\n");
	return 0;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev_nr - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

static int __init ModuleInit(void){
	printk("Hello, Kernel!\n");

	/* Allocate a device nr */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}
	printk("Led driver - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

	/* Create device class */
	if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("Device class can not be created!\n");
		goto ClassError;
	}

	/* create device file */
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	/* Initialize device file */
	cdev_init(&my_device, &fops);

	/* Regisering device to kernel */
	if(cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

	/* GPIO init */
	if(gpio_request(LED_PIN, "gpio_leds_pin")) {
		printk("Can not allocate GPIO led pin\n");
		goto AddError;
	}

	/* Set GPIO direction */
	if(gpio_direction_output(LED_PIN, 0)) {
		printk("Can not set GPIO led pin to output!\n");
		goto GpioError;
	}

	printk("Done\n");
	return 0;

GpioError:
	gpio_free(LED_PIN);
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}

static void __exit ModuleExit(void){
	gpio_set_value(LED_PIN, 0);
	gpio_free(LED_PIN);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
}

module_init(ModuleInit);
module_exit(ModuleExit);
