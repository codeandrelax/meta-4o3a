SUMMARY = "Hello world application"
DESCRIPTION = "Hello world application"
SECTION = "examples"
LICENSE = "CLOSED"

DEPENDS += " libgpiod "

SRC_URI = "file://spitestapp2.c \
	   file://TLC59731.c \
	   file://TLC59731.h \
	  "

TARGET_CC_ARCH += "${LDFLAGS}"

S = "${WORKDIR}"

do_compile () {
	${CC} spitestapp2.c TLC59731.c -o spitestapp2 -lgpiod
}

do_install () {
	install -d ${D}${bindir}
	install -m 0775 spitestapp2 ${D}${bindir}/
}
