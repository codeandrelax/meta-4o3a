#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/cdev.h>

/* buffer to be sent*/
const u8 zero = 0b10000000;
const u8 one  = 0b10010000;

u8 buff[64] = {one,one,zero};

static struct spi_device *spi_dev;

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "tlc59731"
#define DRIVER_CLASS "led_strip"


ssize_t tlc59731_write(struct file * filp, const char * user_buff, size_t len, loff_t * ofp){
    printk("writing");
    spi_write(spi_dev,buff,3);
    return 0;
}

static struct file_operations fops ={
    .write = tlc59731_write
};

static int tlc59731_probe(struct spi_device *spi)
{
	printk(KERN_INFO "Probing tlc59731\n");


	/* Allocate a device nr */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}
	printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

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
    // Save the spi_device pointer for later use
    spi_dev = spi;

    // Extract device tree properties
    struct device_node *np = spi->dev.of_node;
    u32 max_speed_hz;
    u8 mode;
    if (of_property_read_u32(np, "spi-max-frequency", &max_speed_hz) ||
        of_property_read_u8(np, "spi-mode", &mode)) {
        dev_err(&spi->dev, "Failed to read device tree properties\n");
        return -EINVAL;
    }

    // Configure SPI device
    spi_dev->max_speed_hz = max_speed_hz;
    spi_dev->mode = mode;

    // Perform any additional initialization or configuration

    return 0;

AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}

static void tlc59731_remove(struct spi_device *spi)
{
    // Perform any cleanup or resource release

}

static const struct of_device_id tlc59731_dt_ids[] = {
    { .compatible = "ti,tlc59731" },
    {},
};
MODULE_DEVICE_TABLE(of, tlc59731_dt_ids);

static struct spi_driver tlc59731 = {
    .driver = {
        .name = "tlc59731",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(tlc59731_dt_ids),
    },
    .probe = tlc59731_probe,
    .remove = tlc59731_remove,
};

static int __init spi_driver_init(void)
{
    int ret;

    // Register the SPI driver
    ret = spi_register_driver(&tlc59731);
    if (ret < 0) {
        pr_err("Failed to register SPI driver\n");
        return ret;
    }

    return 0;
}

static void __exit spi_driver_exit(void)
{
    cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
    // Unregister the SPI driver
    spi_unregister_driver(&tlc59731);
   
}

module_init(spi_driver_init);
module_exit(spi_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikola Cetic and Nemanja Cenic");
MODULE_DESCRIPTION("SPI Kernel Module with Device Tree Data for led strip");
