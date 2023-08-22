# Intro

This YOCTO project layer has the following functionality:
- Adds custom splash screen during Linux boot process - 4o3a logo
- Install GPIO module in Linux kernel
- Install Encoder module in Linux kernel
- Compile QT test application
- Autostart test application
- Manipulate TLC59731 LED driver via SPI protocol
- Patches Device Tree source files so GPIO pins and SPI peripherals are enabled (register setup)

# How to setup a project

Follow the steps given in official Variscite [wiki](https://variwiki.com/index.php?title=B2QT_Build_Release&release=mx8mn-b2qt-kirkstone-5.15-2.0.x-v1.0) tutorial.
YOCTO project Kirkstone 4.0 uses Boot2QT meta layer. 

After the steps from previous link has been completed, clone this repository (main branch) into sources directory of var-b2qt (so that you get the following path var-b2qt/sources/meta-4o3a)

meta-4o3a is a layer which adds functionality of Station Manager to Linux build.

It is neccessary to edit local.conf file located in build-imx8mn-var-som/conf directory.

```
IMAGE_INSTALL:append = " testqtapp "
IMAGE_INSTALL:append = " spitestapp "
IMAGE_INSTALL:append = " spitestapp2 "

DISTRO_FEATURES:append = " wayland "
IMAGE_INSTALL:append = " qtbase qtwayland "
CORE_IMAGE_EXTRA_INSTALL += " wayland weston "

IMAGE_INSTALL:append = " gpiomodule "
KERNEL_MODULE_AUTOLOAD += " gpio_driver "

IMAGE_INSTALL:append = " rotary-module "
KERNEL_MODULE_AUTOLOAD += " rotary_driver "

IMAGE_INSTALL:append = " libgpiod libgpiod-tools "
```
**IMPORTANT:** meta-4o3a must be added as a layer in YOCTO project. This can be done in two ways.
1) Edit conf/bblayers.conf file, add path to meta-4o3a layer
```
BBLAYERS ?= " \
  ${BSPDIR}/sources/poky/meta \
  ${BSPDIR}/sources/poky/meta-poky \
  ${BSPDIR}/sources/meta-freescale \
  ${BSPDIR}/sources/meta-freescale-3rdparty \
  ${BSPDIR}/sources/meta-freescale-distro \
  ${BSPDIR}/sources/meta-freescale-ml \
  ${BSPDIR}/sources/meta-variscite-bsp \
  ${BSPDIR}/sources/meta-variscite-sdk \
  ${BSPDIR}/sources/meta-variscite-hab \
  ${BSPDIR}/sources/meta-openembedded/meta-oe \
  ${BSPDIR}/sources/meta-openembedded/meta-python \
  ${BSPDIR}/sources/meta-openembedded/meta-networking \
  ${BSPDIR}/sources/meta-openembedded/meta-initramfs \
  ${BSPDIR}/sources/meta-openembedded/meta-multimedia \
  ${BSPDIR}/sources/meta-openembedded/meta-filesystems \
  ${BSPDIR}/sources/meta-boot2qt/meta-boot2qt \
  ${BSPDIR}/sources/meta-boot2qt/meta-boot2qt-distro \
  ${BSPDIR}/sources/meta-mingw \
  ${BSPDIR}/sources/meta-qt6 \
  ${BSPDIR}/sources/meta-swupdate \
  ${BSPDIR}/sources/meta-virtualization \
  ${BSPDIR}/sources/meta-4o3a \
  "
```

2) Use bitbake to add a layer.
```
   bitbake-layers add_layer "path-to-meta-4o3a"
```


# Build

Build begins by executing:
```
MACHINE=imx8mn-var-som bitbake b2qt-embedded-qt6-image
```
Here MACHINE is defined to be _imx8m-var-som_ board.
Board used is NXP i.MX 8M Nano.

To write Linux image (wic.bz file) to a SD card it is neccessary to run:
```
sudo MACHINE=imx8mn-var-som ../sources/meta-variscite-sdk/scripts/var_mk_yocto_sdcard/var-create-yocto-sdcard.sh /dev/sdb
```

# Problems during build

If you experience problems during build (eg. random crashes of Linux terminal) try limiting number of parallel processes ran by bitbake.
To limit number of parallel processes edit local.conf file located in build-imx8mn-var-som/conf directory.
Comment out the line
```
BB_NUMBER_THREADS ?= "${@oe.utils.cpu_count()}"
```
and instead put
```
BB_NUMBER_THREADS = "8"
```
where 8 represents number of processor cores used while building. The lower this number is, the more time build process will take.

Comment out the line
```
PARALLEL_MAKE ?= "-j ${@oe.utils.cpu_count()}"
```
and instead put
```
PARALLEL_MAKE = "-j 8"
```
where 8 is the number of parallel processes run during build. The lower this number is, thje more time build process will take.
Aproximately every process takes around 3GB of RAM memory, so adjust this number according to RAM memory available.

# Miscelaneous

1) Development is done on [Symphony-Board](https://www.variscite.com/product/single-board-computers/symphony-board/) with [NXP i.MX 8M Nano](https://www.variscite.com/product/system-on-module-som/cortex-a53-krait/var-som-mx8m-nano-nxp-i-mx-8m-nano/) System-on-Module.
2) Linux image has built-in tool named _memtool_
memtool is a command-line utility used for reading and writing values to physical memory addresses. It allows direct access to memory regions, such as memory-mapped I/O (MMIO) regions, system memory, or device memory, from user space.
This tool enables a user to inspect values stored in configuration registers which is usefull for debuging.
Path where memtool is stored:
   ```
   ./run/medai/rootfs-mmcblk2p1/unit_tests/memtool
   ```
   To inspect, for example, value of IOMUXC_SAI3_MCLK_SELECT_INPUT (stored at offset 0x303305C0, read reference manual for IMX8M processor) register, execute:
memtool -32 0x303305C0 1
3) Patching Linux DTS:
```
git diff --no-index imx8mn-var-som-symphony.dts.orig imx8mn-var-som-symphony.dts.dts > 0001-gpio-dts.patch
```
Patching Device-Tree source file is done via git diff and patch is added to a new recipe. [DigiKey - Intro to Embedded Linux](https://www.digikey.com/en/maker/projects/intro-to-embedded-linux-part-5-how-to-enable-i2c-in-the-yocto-project/6843bbf9a83c4c96888fccada1e7aedf)

4) To enable autoboot of an application, Linux service is used. Service refers to a script that runs in the background and performs specific tasks. Services are typically managed by a service manager, such as Systemd or SysVinit, which handles the lifecycle of the services, including starting, stopping, restarting, and monitoring them.
Autoboot service for testapp is named _testqtapp.service_ and is located in _recipes-applcation/TestQTApp_. This service starts an executable named testqtapp on boot.
```
[Unit]
Description=Test Qt Application
After=network.target

[Service]
ExecStart=/usr/bin/testqtapp
Restart=on-failure

[Install]
WantedBy=multi-user.target
```
5) During Linux kernel development it's best to use kprint to print out what's happening inside the driver and use dmesg in terminal to show those messages.

4) To create a new layer in YOCTO project using bitbake, one should run:
```
bitbake-layers create-layer meta-layer_name
```
6) Sometimes, when running multiple builds consecutively, some created files may be in an "invalid state". YOCTO will ussualy report back wihich recipe failed. Then that recipe must be cleaned. For example, if recipe _gpiomodule_ failed, it can be cleaned using bitbake with:
```
MACHINE=imx8mn-var-som bitbake -c cleanall gpiomodule
```
7) To build a particular recipe, which is useful during development since only the recipe with changes can be build, reducing build time therefore reducing development time, you run:
```
MACHINE=imx8mn-var-som bitbake recipe-name
```
# Usefull links
[VAR-SOM-MX8M-NANO Datasheet](https://www.variscite.com/wp-content/uploads/2020/01/VAR-SOM-MX8M-NANO_Datasheet.pdf)

[Symphony-Board Datasheet](https://www.variscite.com/wp-content/uploads/2019/07/Symphony-Board_Datasheet.pdf)

[How to use GPIO hog demo presentation](https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&ved=2ahUKEwil7rTkovL_AhWvgf0HHaeyDcoQFnoECBYQAQ&url=https%3A%2F%2Fcommunity.nxp.com%2Fpwmxy87654%2Fattachments%2Fpwmxy87654%2Fimx-processors%2540tkb%2F5635%2F4%2FHow%2520to%2520use%2520gpio-hog%2520demo.pdf&usg=AOvVaw3NCniX-v8mDmk3v_UmwOPI&opi=89978449)

[Varichite Blog, Varichite Device Tree](https://www.variscite.com/blog/getting-started-with-variscite-device-trees/)

[Varichite Device Tree Webinar](https://www.youtube.com/watch?v=nTyO2_D-NUk)

[Inserting Linux driver in Linux kernel, YOCTO project, video, Shahim Vedaei](https://www.youtube.com/watch?v=DnAbIkry-oA)

[Introduction to Embedded Linux, Digi Key](https://www.youtube.com/watch?v=9vsu67uMcko&list=PLEBQazB0HUyTpoJoZecRK6PpDG31Y7RPB)

