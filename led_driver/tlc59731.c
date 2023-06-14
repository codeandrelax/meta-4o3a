#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/cdev.h>


// driver needed variables
static struct spi_device *spi_dev;

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "tlc59731"
#define DRIVER_CLASS "led_strip"
//--------------------------------//


/*-------Operation needed variables------*/
/* buffer to be sent*/
const u8 zero = 0b10000000;
const u8 one  = 0b10010000;



struct __attribute__((packed)) led {
    u64 start_sequence; // 0b1000000010000000100100001001000010010000100000001001000010000000   0x8080909090809080
    u64 red;
    u64 green;
    u64 blue;
    u32 EOS;            // 0x00000000
};


u32 numleds;
size_t buff_size;
struct led *buff;
/*---------------------------------------*/

union map{
    u64 number;
    u8 array[8];
};

static u64 convert_number(u8 number){
    union map map;    
    u8 bit = 0;
    u8 mask = 0x80;
    for(int i = 0; i<8;i++){
        bit = number & (mask>>i);
        if(bit){
            map.array[i] = 0b10010000;
        }
        else{
            map.array[i] = 0b10000000;
        }
    }

    return map.number;
}


ssize_t tlc59731_write(struct file * filp, const char * user_buff, size_t len, loff_t * ofp){
    u8 temp_buff[20];
    u32 led_index;
    u32 red,green,blue;
    u32 to_copy = min(len,sizeof(temp_buff));
    u32 not_copied = copy_from_user(temp_buff, user_buff, to_copy);
    u32 diff = to_copy - not_copied;

    sscanf(temp_buff,"%d %d %d %d",&led_index,&red,&green,&blue);

    if(led_index < 1 || led_index >numleds)
        return diff;

    buff[led_index].red = convert_number(red);
    buff[led_index].green = convert_number(green);
    buff[led_index].blue = convert_number(blue);

    spi_write(spi_dev,buff,buff_size);

    return diff;
}

void initialize_leds(void){
    for(u32 i = 1; i<buff_size/sizeof(struct led)-1; i++){
        buff[i].start_sequence = 0x8090809090908080;
        buff[i].red = convert_number(0);
        buff[i].green = convert_number(0);
        buff[i].blue = convert_number(0);
        buff[i].EOS=0x00000000;

    }
    spi_write(spi_dev,buff,buff_size);
}

static struct file_operations fops ={
    .write = tlc59731_write
};


static int tlc59731_probe(struct spi_device *spi)
{
    struct device_node *np;
    u32 max_speed_hz;
    u8 mode;

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
    np = spi->dev.of_node;
    
    if (of_property_read_u32(np, "spi-max-frequency", &max_speed_hz) ||
        of_property_read_u8(np, "spi-mode", &mode) || of_property_read_u32(np, "nleds", &numleds)) {
        dev_err(&spi->dev, "Failed to read device tree properties\n");
        return -EINVAL;
    }
    // Configure SPI device
    spi_dev->max_speed_hz = max_speed_hz;
    spi_dev->mode = mode;

    // Perform any additional initialization or configuration
    buff_size = (numleds+2)*sizeof(struct led);
    buff = kzalloc(buff_size, GFP_KERNEL);
    initialize_leds();
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
    initialize_leds();
    cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
    kfree(buff);

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

module_spi_driver(tlc59731);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikola Cetic and Nemanja Cenic");
MODULE_DESCRIPTION("SPI Kernel Module with Device Tree Data for led strip");
