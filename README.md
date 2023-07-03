
# How to setup a project

Follow the steps given on official Variscite [wiki](https://variwiki.com/index.php?title=B2QT_Build_Release&release=mx8mn-b2qt-kirkstone-5.15-2.0.x-v1.0) 

After the steps from previous link have been completed, clone this repository (main branch) into sources directory of var-b2qt (so that you get the following path var-b2qt/sources/meta-4o3a)

meta-4o3a is a layer which adds functionality of Station Manager to Linux build.

It is neccessary to edit local.conf file located in build-imx8mn-var-som/conf directory.

```
IMAGE_INSTALL:append = " testqtapp "
IMAGE_INSTALL:append = " spitestapp "
IMAGE_INSTALL:append = " spitestapp2 "

DISTRO_FEATURES:append = " wayland "
IMAGE_INSTALL:append = " qtbase qtwayland "
CORE_IMAGE_EXTRA_INSTALL += " wayland weston "

IMAGE_INSTALL:append = " ledmodule "
KERNEL_MODULE_AUTOLOAD += " tlc59731 "

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
3) kprint and dmseg

4) creating-layers using bitbake

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
To insptect, for example, value of IOMUXC_SAI3_MCLK_SELECT_INPUT (stored at offset 0x303305C0, read reference manual for IMX8M processor) register, execute:
memtool -32 0x303305C0 1
3) Patching Linux DTS:
git diff

# Usefull links

https://www.variscite.com/product/system-on-module-som/cortex-a53-krait/var-som-mx8m-nano-nxp-i-mx-8m-nano/?utm_source=google&utm_medium=cpc&utm_campaign=291971056&utm_content=651046960526&utm_term=&gclid=Cj0KCQjwmN2iBhCrARIsAG_G2i6JdotD3rt7zlenG8P5_CO0bHn0vWSfMGHIu9___WM57jci2HttU3UaAlCrEALw_wcB
https://variwiki.com/index.php?title=VAR-SOM-MX8M-NANO_B2QT&release=mx8mn-b2qt-kirkstone-5.15-2.0.x-v1.0
https://doc.qt.io/Boot2Qt/b2qt-customization.html
https://raymii.org/s/tutorials/Yocto_boot2qt_for_the_Raspberry_Pi_4_both_Qt_6_and_Qt_5.html#toc_9
http://www.java2s.com/Code/Cpp/Qt/Addclickedeventforpushbutton.htm
file:///home/dami-dam/Downloads/How%20to%20use%20gpio-hog%20demo.pdf
