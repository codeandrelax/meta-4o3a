obj-m += simple_gpio.o

KDIR ?= "/home/nikola/KONZOLA_ZA_UREDJAJE/rpi_skeleton/linux"

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
