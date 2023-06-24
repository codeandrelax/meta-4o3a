SUMMARY = "Hello world QT application"
DESCRIPTION = "Hello world QT application"

LICENSE = "CLOSED"
PR = "r0"

DEPENDS += " qtbase wayland "

SRC_URI = "file://FileReaderThread.cpp \
	   file://FileReaderThreadHelper.cpp \
	   file://main.cpp \
	   file://mainwindow.cpp \
	   file://moc_FileReaderThread.cpp \
	   file://moc_mainwindow.cpp \
	   file://mainwindow.ui \
	   file://Constants.h \
	   file://FileReaderThread.h \
	   file://mainwindow.h \
	   file://moc_predefs.h \
	   file://ui_mainwindow.h \
	   file://TestApp.pro \
	   "

TARGET_CC_ARCH += "${LDFLAGS}"

S = "${WORKDIR}"

do_install:append () {
	install -d ${D}${bindir}
	install -m 0775 TestApp ${D}${bindir}/
	
	ln -s -r ${D}/${bindir}/TestApp ${D}/${bindir}/b2qt
}

FILES_${PN} += "${bindir}/TestApp \
		${bindir}/b2qt \
		"

inherit qt6-qmake
