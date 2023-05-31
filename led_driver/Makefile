obj-m += main.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=/home/nemanja/led_driver modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=/home/nemanja/led_driver clean
