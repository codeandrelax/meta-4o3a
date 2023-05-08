obj-m += mcp23017_v2.o

KDIR ?= "/home/nikola/KONZOLA_ZA_UREDJAJE/rpi_skeleton/linux"

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
