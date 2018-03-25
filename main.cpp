#include "blockpad.h"
#include <QApplication>
#include <QFontDatabase>
#include <QDir>
#include "global.h"
#include "mainwidget.h"
#include "eventswaiting.h"
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime().addSecs(24)));
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Bloc10");
    QCoreApplication::setOrganizationDomain("www.bloc10.com");
    QCoreApplication::setApplicationName("BlockPad");
    //create application files directories
    {
        QDir().mkpath(Utilities::appFilesDirectory());
        //create defPathBlockpads
        if(!QDir(Utilities::appFilesDirectory()
                 + "/"+ defPathBlockpads).exists())
            QDir(Utilities::appFilesDirectory()).mkdir(defPathBlockpads);
    }
    //fonts
    {
        QDir dir("://Fonts");
        foreach(auto file, dir.entryList())
        {
            QFontDatabase::addApplicationFont("://Fonts/"+ file);
        }
        QString font;
        //fill font
        {
            QSettings settings;
            font = settings.value("Font").toString();
            if(font == "")
                font = "Roboto";
        }
        a.setFont (QFont (font, appFontPointSize, appFontWeight));
    }
    EventsWaiting evWait;
    a.installEventFilter(&evWait);
    MainWidget wgt;
    wgt.show();

    //connect signals/slots
    {
        a.connect(&evWait, &EventsWaiting::StartLockScreen,
                  &wgt, &MainWidget::slotLockScreen);
        a.connect(&wgt, &MainWidget::sigScreenLock_Time,
                  &evWait, &EventsWaiting::slotSetTimeLockScreen);
    }

    return a.exec();
}
