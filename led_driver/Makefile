obj-m += tlc59731.o

KDIR ?= "/home/nikola/KONZOLA_ZA_UREDJAJE/rpi_skeleton/linux"

all: module dt
	echo Building Device Tree Overlay and kernel module

module:
	make -C $(KDIR) M=$(PWD) modules

dt: overlay.dts
	dtc -@ -I dts -O dtb -o overlay.dtbo overlay.dts
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -rf overlay.dtbo
