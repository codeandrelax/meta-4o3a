SUMMARY = "Hello world application"
DESCRIPTION = "Hello world application"
SECTION = "examples"
LICENSE = "CLOSED"

DEPENDS += " libgpiod "

SRC_URI = "file://spitestapp.c"

TARGET_CC_ARCH += "${LDFLAGS}"

S = "${WORKDIR}"

do_compile () {
	${CC} spitestapp.c -o spitestapp -lgpiod
}

do_install () {
	install -d ${D}${bindir}
	install -m 0775 spitestapp ${D}${bindir}/
}
