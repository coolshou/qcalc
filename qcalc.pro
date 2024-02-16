isEmpty(PREFIX) {
 PREFIX = /usr/
}
TARGET = qcalc
QT += widgets

HEADERS       = src/button.h \
    src/qcalc.h
SOURCES       = src/button.cpp \
                src/main.cpp \
    src/qcalc.cpp

# install
target.path = $$PREFIX/bin/


RESOURCES += \
    src/qcalc.qrc

FORMS += \
    src/qcalc.ui


INSTALLS += target

unix {
DESKTOP = qcalc.desktop
DESKTOP.path = $$PREFIX/share/appliactions/

IMAGES = src/images/qcalc.png
IMAGES.path = $$PREFIX/share/pixmaps/

DISTFILES += $$DESKTOP \
    $$IMAGES

}



