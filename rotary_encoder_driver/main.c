#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define DRIVER_NAME "redriver"
#define DRIVER_CLASS "ModuleClass"

static char buffer[100];

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nemanja Cenic");
MODULE_DESCRIPTION("Rotary encoder");

struct rotary_encoder{
	int pin_1;
	int pin_2;
	int pin_sw;
	int state;
	int value;
};

void register_rotary_encoder(struct rotary_encoder *re){
	gpio_request(re->pin_1, "gpio_pin_1");
	gpio_direction_input(re->pin_1);
	gpio_request(re->pin_2, "gpio_pin_2");
	gpio_direction_input(re->pin_2);
	gpio_request(re->pin_sw, "gpio_pin_sw");
	gpio_direction_input(re->pin_sw);
	re->state = 0;
	re->value = 0;
}

void unregister_rotary_encoder(struct rotary_encoder *re){
	gpio_free(re->pin_1);
	gpio_free(re->pin_2);
	gpio_free(re->pin_sw);
	re->state = 0;
	re->value = 0;
}

void encoder_state_machine(struct rotary_encoder *re, int pin_1, int pin_2){
	switch(re->state){
		case 0:
			if(pin_1 == 1 && pin_2 == 0){
				re->state = 1;
				re->value++;
			}
			if(pin_1 == 0 && pin_2 == 1){
				re->state = 3;
				re->value--;
			}
			break;
		case 1:
			if(pin_1 == 1 && pin_2 == 1){
				re->state = 2;
				re->value++;
			}
			if(pin_1 == 0 && pin_2 == 0){
				re->state = 0;
				re->value--;
			}
			break;
		case 2:
			if(pin_1 == 0 && pin_2 == 1){
				re->state = 3;
				re->value++;
			}
			if(pin_1 == 1 && pin_2 == 0){
				re->state = 1;
				re->value--;
			}
			break;
		case 3:
			if(pin_1 == 0 && pin_2 == 0){
				re->state = 0;
				re->value++;
			}
			if(pin_1 == 1 && pin_2 == 1){
				re->state = 2;
				re->value--;
			}
			break;
		default:
			break;
	}
	//printk("v: %d\n", re->value);
}

struct rotary_encoder re;

unsigned int flag1 = 0;
unsigned int flag2 = 0;
unsigned int lock = 0;
unsigned int pin_1_last_state = 0;
unsigned int pin_2_last_state = 0;
unsigned int pin_1_current_state = 0;
unsigned int pin_2_current_state = 0;

unsigned long old_jiffie_21 = 0;
unsigned long old_jiffie_20 = 0;
unsigned long old_jiffie_sw = 0;
unsigned int irq_number;
unsigned int irq_number_20;
unsigned int irq_num_pin_sw;
//signed int value = 0;
signed int old_value = 0;
//int state = 0;

/*static struct task_struct *kthread_1;

int thread_function(void *data){
	while(!kthread_should_stop()){
		if(state == 1){
			value++;
			state = 0;
			printk("v: %d\n", value);
		}
		else if(state == 2){
			value--;
			state = 0;
			printk("v: %d\n", value);
		}
//		msleep(1);
	}
	return 0;
}
*/
static irqreturn_t gpio_irq_handler(int irq, void *dev_id){
	unsigned int diff = jiffies - old_jiffie_21;
	if(diff < 1)
		return IRQ_HANDLED;

	old_jiffie_21 = jiffies;

	pin_1_current_state = gpio_get_value(re.pin_1);
	if(pin_1_current_state != pin_1_last_state){
		pin_1_last_state = pin_1_current_state;
		encoder_state_machine(&re, pin_1_last_state, pin_2_last_state);
		/*if(re.state == 0){
			re.state = 1;
			re.value++;
			printk("v: %d\n", re.value);
		}
		else
			re.state = 0;*/
	}
/*	if(old_value == value){
		value++;
		printk("v: %d\n", value);
	}
	else
		old_value = value*/
	/*pin_1_current_state = gpio_get_value(re.pin_1);
	if(pin_1_current_state != pin_1_last_state){
		if(gpio_get_value(re.pin_2) != pin_1_current_state)
			value++;
		else
			value--;
		printk("v: %d\n", value);
		pin_1_last_state = pin_1_current_state;
	}*/
	/*msleep(1);
	pin_1_current_state = gpio_get_value(re.pin_1);
	if(state == 0 && pin_1_current_state != pin_1_last_state){
		if(gpio_get_value(re.pin_1) != gpio_get_value(re.pin_2))
			state = 1;
		else
			state = 2;
		pin_1_last_state = pin_1_current_state;
	}*/
	return IRQ_HANDLED;
}

static irqreturn_t gpio_irq_handler_sw(int irq, void *dev_id){
	unsigned int diff = jiffies - old_jiffie_sw;
	if(diff < 20)
		return IRQ_HANDLED;
	printk("sw\n");
	old_jiffie_sw = jiffies;
	return IRQ_HANDLED;
}

static irqreturn_t gpio_21_irq_handler(int irq, void *dev_id){
	unsigned int diff = jiffies - old_jiffie_20;
	if(diff < 1)
		return IRQ_HANDLED;

	old_jiffie_20 = jiffies;

	pin_2_current_state = gpio_get_value(re.pin_2);
	if(pin_2_current_state != pin_2_last_state){
		pin_2_last_state = pin_2_current_state;
		encoder_state_machine(&re, pin_1_last_state, pin_2_last_state);
/*		if(re.state == 0){
			re.state = 1;
			re.value--;
			printk("v: %d\n", re.value);
		}
		else
			re.state = 0;
*/	}
/*	if(old_value == value){
		value--;
		printk("v: %d\n", value);
	}
	else
		old_value = value;
*/	return IRQ_HANDLED;
}

static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;

	sprintf(buffer, "%d", re.value);

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
	int to_copy, not_copied, delta;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(buffer));

	/* Copy data to user */
	not_copied = copy_from_user(buffer, user_buffer, to_copy);

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
	re.pin_1 = 21;
	re.pin_2 = 20;
	re.pin_sw = 16;

	register_rotary_encoder(&re);

	pin_1_last_state = gpio_get_value(re.pin_1);
	pin_2_last_state = gpio_get_value(re.pin_2);

	irq_number = gpio_to_irq(re.pin_1);
	irq_num_pin_sw = gpio_to_irq(re.pin_sw);
	irq_number_20 = gpio_to_irq(re.pin_2);

	if(alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
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
//	kthread_1 = kthread_run(thread_function, NULL, "kthread_1");
/*	if(kthread_1 == NULL){
		printk("Error: Thread!\n");
		unregister_rotary_encoder(&re);
		return -1;
	}
*/
	if(request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "my_gpio_irq", NULL) != 0){
		printk("Error: Pin 1 interrupt\n");
//		kthread_stop(kthread_1);
		unregister_rotary_encoder(&re);
		return -1;
	}

	if(request_irq(irq_num_pin_sw, gpio_irq_handler_sw, IRQF_TRIGGER_FALLING, "gpio_sw_irq", NULL) != 0){
		printk("Error: SW interrupt\n");
		free_irq(irq_number, NULL);
//		kthread_stop(kthread_1);
		unregister_rotary_encoder(&re);
		return -1;
	}

	if(request_irq(irq_number_20, gpio_21_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "gpio_fall_irq", NULL) != 0){
		printk("Error: Pin 2 interrupt\n");
		free_irq(irq_number, NULL);
		free_irq(irq_num_pin_sw, NULL);
		unregister_rotary_encoder(&re);
//		kthread_stop(kthread_1);
		return -1;
	}

	printk("Done\n");
	return 0;
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}

static void __exit ModuleExit(void){
	free_irq(irq_number, NULL);
	free_irq(irq_num_pin_sw, NULL);
	free_irq(irq_number_20, NULL);
//	kthread_stop(kthread_1);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	unregister_rotary_encoder(&re);
}

module_init(ModuleInit);
module_exit(ModuleExit);
