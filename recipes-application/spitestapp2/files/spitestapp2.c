// SPDX-License-Identifier: GPL-2.0-only
/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "TLC59731.h"

static const char *device = "/dev/spidev0.2";
static uint32_t mode = 0x20;
static uint8_t bits = 8;
static uint32_t speed = 100000;
static uint16_t delay = 0;

static uint8_t default_rx[BUFFER_SIZE] = {0, };

int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
	uint32_t request;

	printf("SPITESTAPP2 START\n");

	fd = open(device, O_RDWR);

	request = mode;
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

	uint8_t *rx = default_rx;

	initialize_buffer();

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)LED_buffer,
		.rx_buf = (unsigned long)rx,
		.len = BUFFER_SIZE,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
		.cs_change = 0,
	};
	
	setLED(1, 255, 255, 255);
	setLED(2, 255, 0, 0);
	setLED(3, 0, 255, 0);
	setLED(4, 0, 0, 255);
	setLED(5, 0, 0, 0);
	setLED(6, 255, 255, 0);
	setLED(7, 255, 0, 255);
	setLED(8, 0, 255, 255);
	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	//transfer(fd, default_tx, default_rx, sizeof(default_tx));
	char buffer[100];
	
	int i = 0;
	while(1)
	{
		i = ((i +1) % 32);
		initialize_buffer();
		setLED(i, 255, 0, 0);
		ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
		usleep(500000);
	}
	close(fd);

	return ret;
}
