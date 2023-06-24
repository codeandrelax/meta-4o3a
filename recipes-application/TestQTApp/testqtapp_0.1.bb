SUMMARY = "Hello world QT application"
DESCRIPTION = "Test QT application"

LICENSE = "CLOSED"
PR = "r0"

DEPENDS += " qtbase wayland "

SRC_URI = "file://TestQTApp.cpp \
	   file://TestQTApp.pro \
	   "

TARGET_CC_ARCH += "${LDFLAGS}"

S = "${WORKDIR}"

do_install:append () {
	install -d ${D}${bindir}
	install -m 0775 testqtapp ${D}${bindir}/
	
	ln -s -r ${D}/${bindir}/testqtapp ${D}/${bindir}/b2qt
}

FILES_${PN} += "${bindir}/testqtapp \
		${bindir}/b2qt \
		"

inherit qt6-qmake
