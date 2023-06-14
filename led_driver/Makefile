obj-m += tlc59731.o

KDIR ?= "/home/nikola/KONZOLA_ZA_UREDJAJE/rpi_skeleton/linux"

all: module dt
	echo Building Device Tree Overlay and kernel module

module:
	make -C $(KDIR) M=$(PWD) modules

dt: tlc59731-spi.dts
	dtc -@ -I dts -O dtb -o tlc59731-spi.dtbo tlc59731-spi.dts
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -rf tlc59731-spi.dtbo
