obj-m += rotary_encoder.o main.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=/home/nemanja/rotary_encoder modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=/home/nemanja/rotary_encoder clean
