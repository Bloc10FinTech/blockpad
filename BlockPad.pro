#-------------------------------------------------
#
# Project created by QtCreator 2018-02-26T23:34:21
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += concurrent
QT       += webenginewidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BlockPad
TEMPLATE = app
QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    tablewidgetbase.cpp \
    tablewidgetcoinrecords.cpp \
    tablewidgetaccounts.cpp \
    passwordwidget.cpp \
    crypt.cpp \
    register.cpp \
    mainwidget.cpp \
    eventswaiting.cpp \
    blockpad.cpp \
    settingswgt.cpp \
    codeeditor.cpp \
    stmp/src/emailaddress.cpp \
    stmp/src/mimeattachment.cpp \
    stmp/src/mimecontentformatter.cpp \
    stmp/src/mimefile.cpp \
    stmp/src/mimehtml.cpp \
    stmp/src/mimeinlinefile.cpp \
    stmp/src/mimemessage.cpp \
    stmp/src/mimemultipart.cpp \
    stmp/src/mimepart.cpp \
    stmp/src/mimetext.cpp \
    stmp/src/quotedprintable.cpp \
    stmp/src/smtpclient.cpp \
    highlighter.cpp

HEADERS += \
    global.h \
    tablewidgetbase.h \
    tablewidgetcoinrecords.h \
    tablewidgetaccounts.h \
    passwordwidget.h \
    crypt.h \
    register.h \
    mainwidget.h \
    eventswaiting.h \
    blockpad.h \
    settingswgt.h \
    codeeditor.h \
    stmp/src/emailaddress.h \
    stmp/src/mimeattachment.h \
    stmp/src/mimecontentformatter.h \
    stmp/src/mimefile.h \
    stmp/src/mimehtml.h \
    stmp/src/mimeinlinefile.h \
    stmp/src/mimemessage.h \
    stmp/src/mimemultipart.h \
    stmp/src/mimepart.h \
    stmp/src/mimetext.h \
    stmp/src/quotedprintable.h \
    stmp/src/smtpclient.h \
    stmp/src/smtpexports.h \
    highlighter.h \
    adswebpage.h

FORMS += \
    passwordwidget.ui \
    register.ui \
    mainwidget.ui \
    blockpad.ui \
    settingswgt.ui

RESOURCES += \
    fonts.qrc \
    icons.qrc \
    passwords.qrc
win32{
INCLUDEPATH += C:\OpenSSL-Win32\include
#LIBS += -L$$PWD/../../../../../OpenSSL-Win32/lib/MinGW/ -leay32

#INCLUDEPATH += $$PWD/../../../../../OpenSSL-Win32/lib/MinGW
#DEPENDPATH += $$PWD/../../../../../OpenSSL-Win32/lib/MinGW

#PRE_TARGETDEPS += $$PWD/../../../../../OpenSSL-Win32/lib/MinGW/libeay32.a

#LIBS += -L$$PWD/../../../../../OpenSSL-Win32/lib/MinGW/ -lssleay32
#PRE_TARGETDEPS += $$PWD/../../../../../OpenSSL-Win32/lib/MinGW/ssleay32.a

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../OpenSSL-Win32/lib/VC/ -lssleay32MD
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../OpenSSL-Win32/lib/VC/ -lssleay32MDd

INCLUDEPATH += $$PWD/../../../../../OpenSSL-Win32/include/openssl
DEPENDPATH += $$PWD/../../../../../OpenSSL-Win32/include/openssl

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../OpenSSL-Win32/lib/VC/ -llibeay32MD
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../OpenSSL-Win32/lib/VC/ -llibeay32MDd

RC_ICONS = BlockPad.ico
}

macx{
LIBS += -L$$PWD/../../openssl/openssl-1.0.2n/lib/ -lcrypto
INCLUDEPATH += $$PWD/../../openssl/openssl-1.0.2n/include
DEPENDPATH += $$PWD/../../openssl/openssl-1.0.2n/include
PRE_TARGETDEPS += $$PWD/../../openssl/openssl-1.0.2n/lib/libcrypto.a
LIBS += -L$$PWD/../../openssl/openssl-1.0.2n/lib/ -lssl
PRE_TARGETDEPS += $$PWD/../../openssl/openssl-1.0.2n/lib/libssl.a

ICON = BlockPad.icns
}

