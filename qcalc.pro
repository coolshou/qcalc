TARGET = qcalc
QT += widgets

HEADERS       = src/button.h \
    src/qcalc.h
SOURCES       = src/button.cpp \
                src/main.cpp \
    src/qcalc.cpp

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/calculator
#INSTALLS += target

RESOURCES += \
    src/qcalc.qrc

FORMS += \
    src/qcalc.ui

DISTFILES += \
    qcalc.desktop
