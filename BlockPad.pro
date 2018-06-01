#-------------------------------------------------
#
# Project created by QtCreator 2018-02-26T23:34:21
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += concurrent
QT       += webenginewidgets
QT       += svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BlockPad
unix:!macx{
TARGET = blockpad
}
TEMPLATE = app

DEFINES += AWS_SDK_PLATFORM_WINDOWS
DEFINES += ENABLE_WINDOWS_CLIENT
DEFINES += ENABLE_BCRYPT_ENCRYPTION
DEFINES += USE_IMPORT_EXPORT
DEFINES += AWS_S3_EXPORTS
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_NO_DEBUG_OUTPUT
#DEFINES += TEST_LICENSE
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
    tablePrinter/tableprinter.cpp \
    ticker.cpp \
    webBrowser/browserwindow.cpp \
    webBrowser/downloadmanagerwidget.cpp \
    webBrowser/downloadwidget.cpp \
    webBrowser/tabwidget.cpp \
    webBrowser/webpage.cpp \
    webBrowser/webpopupwindow.cpp \
    webBrowser/webview.cpp \
    webBrowser/browser.cpp \
    networklicenseserver.cpp \
    activatewgt.cpp \
    fileencryptionwgt.cpp \
    onetimepadgenerator.cpp \
    messagescramblerwgt.cpp \
    searchwgt.cpp \
    find/searchresulttreeitemdelegate.cpp \
    find/findwidget.cpp \
    find/treewidgetfindresults.cpp

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
    tablePrinter/tableprinter.h \
    ticker.h \
    webBrowser/browserwindow.h \
    webBrowser/downloadmanagerwidget.h \
    webBrowser/downloadwidget.h \
    webBrowser/tabwidget.h \
    webBrowser/webpage.h \
    webBrowser/webpopupwindow.h \
    webBrowser/webview.h \
    webBrowser/browser.h \
    networklicenseserver.h \
    activatewgt.h \
    fileencryptionwgt.h \
    onetimepadgenerator.h \
    messagescramblerwgt.h \
    searchwgt.h \
    find/searchresulttreeitemdelegate.h \
    find/findwidget.h \
    find/globalsearch.h \
    find/treewidgetfindresults.h

FORMS += \
    passwordwidget.ui \
    register.ui \
    mainwidget.ui \
    blockpad.ui \
    settingswgt.ui \
    generatepassword.ui \
    onetimepadgeneratorwgt.ui \
    ticker.ui \
    webBrowser/certificateerrordialog.ui \
    webBrowser/downloadmanagerwidget.ui \
    webBrowser/downloadwidget.ui \
    webBrowser/passworddialog.ui \
    activatewgt.ui \
    onetimepadgenerator.ui \
    messagescramblerwgt.ui \
    searchwgt.ui \
    find/findwidget.ui

RESOURCES += \
    fonts.qrc \
    icons.qrc \
    totalcryptosprices.qrc \
    webBrowser/data/simplebrowser.qrc

#########################################################
#LIBS
#########################################################
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

LIBS += -L$$PWD/../../../../usr/local/lib/ -laws-cpp-sdk-s3 -laws-cpp-sdk-core -laws-cpp-sdk-transfer

INCLUDEPATH += $$PWD/../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../usr/local/include
ICON = BlockPad.icns
}

unix:!macx{
LIBS += -L$$PWD/../../OpenSSl/build/lib/ -lcrypto -lssl

INCLUDEPATH += $$PWD/../../OpenSSl/build/include/openssl
DEPENDPATH += $$PWD/../../OpenSSl/build/include/openssl

LIBS += -L$$PWD/../../aws/build/lib/ -laws-cpp-sdk-s3 -laws-cpp-sdk-core -laws-cpp-sdk-transfer

INCLUDEPATH += $$PWD/../../aws/build/include
DEPENDPATH += $$PWD/../../aws/build/include

}

#########################################################
#INSTALLS
#########################################################
win32{
install.path = C:/Users/alex.user-ion/Documents/GitHub/BlockPadBin
CONFIG(release, debug|release):install.files += $$OUT_PWD/release/BlockPad.exe
CONFIG(debug, debug|release):install.files += $$OUT_PWD/debug/BlockPad.exe
install.files += BlockPadReadMe.rtf
INSTALLS +=install

local_install.path = $$OUT_PWD
local_install.files += BlockPadReadMe.rtf
INSTALLS +=local_install

CONFIG(release, debug|release): LIBS += -L$$PWD/'../../../../../Program Files (x86)/aws-cpp-sdk-all/bin/' -laws-cpp-sdk-core -laws-cpp-sdk-s3 -laws-cpp-sdk-transfer
CONFIG(debug, debug|release): LIBS += -L$$PWD/'../../../../../Program Files (x86)/aws-cpp-sdk-all/bin-debug/' -laws-cpp-sdk-core -laws-cpp-sdk-s3 -laws-cpp-sdk-transfer
#CONFIG(release, debug|release): LIBS +=  -laws-cpp-sdk-core -laws-cpp-sdk-s3 -laws-cpp-sdk-transfer
#CONFIG(debug, debug|release): LIBS +=  -laws-cpp-sdk-core -laws-cpp-sdk-s3 -laws-cpp-sdk-transfer


INCLUDEPATH += $$PWD/'../../../../../Program Files (x86)/aws-cpp-sdk-all/include'
DEPENDPATH += $$PWD/'../../../../../Program Files (x86)/aws-cpp-sdk-all/include'

}

macx{
install.path = $$OUT_PWD/BlockPad.app/Contents/UpdateTools/
install.files += /Users/admin/Desktop/build-UpdateBlockPad-Desktop_Qt_5_10_0_clang_64bit-Release/UpdateBlockPad.app
INSTALLS +=install

addDocs_install.path = Contents/Resources/
addDocs_install.files = BlockPadReadMe.rtf
QMAKE_BUNDLE_DATA += addDocs_install
}

unix:!macx{
install.path += $$OUT_PWD
install.files += BlockPadReadMe.rtf
install.files += /home/alex/Projects/UpdateBlockPadRelease/UpdateBlockPad
INSTALLS +=install

QMAKE_LFLAGS_RPATH += $${LD_RUN_PATH}
QMAKE_LFLAGS_RPATH += $${LDFLAGS}
QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../libs\'-Wl,-rpath,$${QMAKE_LFLAGS_RPATH}"
#QMAKE_RPATHDIR += /usr/share/blockpad/lib
#QMAKE_RPATHDIR += $$[QT_INSTALL_LIBS]

we_resources.path = /usr/share/blockpad/bin/resources
we_resources.files = webEngine_resources/*

target.path = /usr/share/blockpad/bin
target.files += $$OUT_PWD/blockpad
target.files += qt.conf
target.files += webEngine_Exe/*
target.files += blockpad.sh
target.files += BlockPadReadMe.rtf

data.path = /usr/share/blockpad/lib
data.files = lib/*

# .desktop file
desktop.path = /usr/share/applications/
desktop.files += dist/blockpad.desktop

# logo
pixmaps.path = /usr/share/pixmaps/
pixmaps.files += dist/blockpad.png

INSTALLS+=\
        pixmaps\
        desktop\
        data\
        target\
        we_resources
}



