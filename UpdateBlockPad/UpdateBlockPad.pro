QT += concurrent widgets
CONFIG += console

SOURCES += main.cpp

RESOURCES += \
    res.qrc

unix:!macx{
install.path += $$OUT_PWD
install.files += update.sh
INSTALLS +=install
}
