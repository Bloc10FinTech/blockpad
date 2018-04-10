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

DEFINES +=
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_NO_DEBUG_OUTPUT
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
    highlighter.cpp \
    generatepassword.cpp \
    global.cpp \
    richitemdelegate.cpp \
    stega/lodepng.cpp \
    stega/steganography.cpp \
    tablePrinter/tableprinter.cpp

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
    adswebpage.h \
    generatepassword.h \
    richitemdelegate.h \
    stega/lodepng.h \
    stega/steganography.h \
    tablePrinter/tableprinter.h

FORMS += \
    passwordwidget.ui \
    register.ui \
    mainwidget.ui \
    blockpad.ui \
    settingswgt.ui \
    generatepassword.ui

RESOURCES += \
    fonts.qrc \
    icons.qrc
win32{
#QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
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

win32{
install.path = C:/Users/user/Documents/GitHub/BlockPadBin
CONFIG(release, debug|release):install.files += $$OUT_PWD/release/BlockPad.exe
CONFIG(debug, debug|release):install.files += $$OUT_PWD/debug/BlockPad.exe
INSTALLS +=install
}

macx{
install.path = $$OUT_PWD/BlockPad.app/Contents/UpdateTools/
install.files += /Users/admin/Desktop/build-UpdateBlockPad-Desktop_Qt_5_10_0_clang_64bit-Release/UpdateBlockPad.app
INSTALLS +=install
}
