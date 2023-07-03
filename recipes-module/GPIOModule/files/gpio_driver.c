/***************************************************************************//**
*  \file       simple_gpio.c
*
*  \details    Simple GPIO driver explanation
*
*  \author     Nikola Cetic
*
*  \Tested with Linux raspberrypi
*
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>  //copy_to/from_user()
#include <linux/gpio.h>     //GPIO
#include <linux/err.h>

//(gpio_bank - 1) * 32 + gpio_bit
// Connector 6, GPIO5_IO26 --> (5-1)*32+26=128+26=154
// Connector 12, GPIO4_IO28 --> (4-1)*32+28=96+28=124

//LED is connected to this GPIO
#define GPIO_154 (154)
#define GPIO_124 (124)
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev simple_gpio_cdev;
 
static int __init simple_gpio_driver_init(void);
static void __exit simple_gpio_driver_exit(void);
 
 
/*************** Driver functions **********************/
static int simple_gpio_open(struct inode *inode, struct file *file);
static int simple_gpio_release(struct inode *inode, struct file *file);
static ssize_t simple_gpio_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t simple_gpio_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);
/******************************************************/
 
//File operation structure 
static struct file_operations fops =
{
  .owner          = THIS_MODULE,
  .read           = simple_gpio_read,
  .write          = simple_gpio_write,
  .open           = simple_gpio_open,
  .release        = simple_gpio_release,
};

/*
** This function will be called when we open the Device file
*/ 
static int simple_gpio_open(struct inode *inode, struct file *file)
{
  pr_info("Device File Opened...!!!\n");
  return 0;
}

/*
** This function will be called when we close the Device file
*/
static int simple_gpio_release(struct inode *inode, struct file *file)
{
  pr_info("Device File Closed...!!!\n");
  return 0;
}

/*
** This function will be called when we read the Device file
*/ 

static ssize_t simple_gpio_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
  uint8_t gpio_state[2];
  
  //reading GPIO value
  gpio_state[0] = gpio_get_value(GPIO_154);
  gpio_state[1] = gpio_get_value(GPIO_124);

  //write to user
  len = 1;
  if( copy_to_user(buf, gpio_state, 2) > 0) {
    pr_err("ERROR: Not all the bytes have been copied to user\n");
  }
  
  //pr_info("Read function : GPIO_154 = %d \n", gpio_state[0]);
  //pr_info("Read function : GPIO_124 = %d \n", gpio_state[1]);
  return 0;
}

/*
** This function will be called when we write the Device file
*/ 
static ssize_t simple_gpio_write(struct file *filp, 
                const char __user *buf, size_t len, loff_t *off)
{
    pr_err("Usupported command\n");
  return len;
}

/*
** Module Init function
*/ 
static int __init simple_gpio_driver_init(void)
{
  /*Allocating Major number*/
  if((alloc_chrdev_region(&dev, 0, 1, "simple_gpio_dev")) <0){
    pr_err("Cannot allocate major number\n");
    goto r_unreg;
  }
  pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
  /*Creating cdev structure*/
  cdev_init(&simple_gpio_cdev,&fops);
 
  /*Adding character device to the system*/
  if((cdev_add(&simple_gpio_cdev,dev,1)) < 0){
    pr_err("Cannot add the device to the system\n");
    goto r_del;
  }
 
  /*Creating struct class*/
  if(IS_ERR(dev_class = class_create(THIS_MODULE,"simple_gpio_class"))){
    pr_err("Cannot create the struct class\n");
    goto r_class;
  }
 
  /*Creating device*/
  if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"simple_gpio_device"))){
    pr_err( "Cannot create the Device \n");
    goto r_device;
  }
  
  //Checking the GPIO is valid or not
  if(gpio_is_valid(GPIO_154) == false){
    pr_err("GPIO %d is not valid\n", GPIO_154);
    goto r_device;
  }
  
  //Requesting the GPIO
  if(gpio_request(GPIO_154,"GPIO_154") < 0){
    pr_err("ERROR: GPIO %d request\n", GPIO_154);
    goto r_GPIO_154;
  }
  
  //configure the GPIO as output
  gpio_direction_input(GPIO_154);
  
  /* Using this call the GPIO 21 will be visible in /sys/class/gpio/
  ** Now you can change the gpio values by using below commands also.
  ** echo 1 > /sys/class/gpio/gpio21/value  (turn ON the LED)
  ** echo 0 > /sys/class/gpio/gpio21/value  (turn OFF the LED)
  ** cat /sys/class/gpio/gpio21/value  (read the value LED)
  ** 
  ** the second argument prevents the direction from being changed.
  */
  gpio_export(GPIO_154, false);
  
  //Checking the GPIO is valid or not
  if(gpio_is_valid(GPIO_124) == false){
    pr_err("GPIO %d is not valid\n", GPIO_124);
    goto r_GPIO_154;
  }
  
  //Requesting the GPIO
  if(gpio_request(GPIO_124,"GPIO_124") < 0){
    pr_err("ERROR: GPIO %d request\n", GPIO_124);
    goto r_GPIO_124;
  }
  
  //configure the GPIO as output
  gpio_direction_input(GPIO_124);
  
  /* Using this call the GPIO 21 will be visible in /sys/class/gpio/
  ** Now you can change the gpio values by using below commands also.
  ** echo 1 > /sys/class/gpio/gpio21/value  (turn ON the LED)
  ** echo 0 > /sys/class/gpio/gpio21/value  (turn OFF the LED)
  ** cat /sys/class/gpio/gpio21/value  (read the value LED)
  ** 
  ** the second argument prevents the direction from being changed.
  */
  gpio_export(GPIO_124, false);

  pr_info("Device Driver Insert...Done!!!\n");
  return 0;
 
r_GPIO_124:
  gpio_free(GPIO_124);
r_GPIO_154:
  gpio_free(GPIO_154);
r_device:
  device_destroy(dev_class,dev);
r_class:
  class_destroy(dev_class);
r_del:
  cdev_del(&simple_gpio_cdev);
r_unreg:
  unregister_chrdev_region(dev,1);
  
  return -1;
}

/*
** Module exit function
*/ 
static void __exit simple_gpio_driver_exit(void)
{
  gpio_unexport(GPIO_124);
  gpio_free(GPIO_124);
  gpio_unexport(GPIO_154);
  gpio_free(GPIO_154);
  device_destroy(dev_class,dev);
  class_destroy(dev_class);
  cdev_del(&simple_gpio_cdev);
  unregister_chrdev_region(dev, 1);
  pr_info("Device Driver Remove...Done!!\n");
}
 
module_init(simple_gpio_driver_init);
module_exit(simple_gpio_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("NIkola Cetic nikolacetic8@gmail.com");
MODULE_DESCRIPTION("A simple device driver - GPIO Driver");
MODULE_VERSION("1.0");
