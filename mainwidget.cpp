#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QDir>
#include <QIcon>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QProcess>
#include <QtConcurrent>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
#ifdef __APPLE__
    QIcon icon = QIcon(QCoreApplication::applicationDirPath() +
                                      QLatin1String("/../Resources/BlockPad.icns"));
    setWindowIcon(icon);
#endif
    ui->stackedWidget->setCurrentWidget(ui->regist);
    //connect signals/slots
    {
        connect(ui->regist, &Register::successRegister,
                this, &MainWidget::slotSuccessRegistered);
        connect(ui->regist, &Register::successUnlocked,
                this, &MainWidget::slotSuccessUnlocked);
        connect(ui->blockPad, &BlockPad::sigScreenLock_Time,
                this, &MainWidget::sigScreenLock_Time);
    }
    bool b2FA = false;
    if(settings.value("2FA_On").type() != QVariant::Invalid)
    {
        b2FA = settings.value("2FA_On").toBool();
    }
#ifdef __APPLE__
    if(settings.value("updateToolsVersion").toString()
            != defVersionApplication
       || !QFile(Utilities::filesDirectory()
            + "/UpdateBlockPad.app").exists())
        QtConcurrent::run(this, &MainWidget::updateUpdateTools);
#endif
}

void MainWidget::showEvent(QShowEvent *event)
{
    if(bFirstShow)
    {
        ui->blockPad->setMinimumWidth(0);
        ui->blockPad->setMinimumHeight(0);
        ui->regist->Init();
        ui->regist->adjustSize();
        ui->stackedWidget->setFixedSize(ui->regist->size());
        setFixedSize(ui->regist->size());
        bFirstShow = false;
    }
    QWidget::showEvent(event);
}

#ifdef __APPLE__
    void MainWidget::updateUpdateTools()
    {
        //clean
        {
            QDir dir(Utilities::filesDirectory()+ "/UpdateBlockPad.app");
            dir.removeRecursively();
        }
        //remove new version to appPath
        {
            QProcess pros;
            pros.start("sh -c \"cp -R " +Utilities::applicationPath().replace(" ", "\\ ") +"/BlockPad.app/Contents/UpdateTools/UpdateBlockPad.app "
                       + Utilities::filesDirectory().replace(" ", "\\ ")  + "/UpdateBlockPad.app\"");
            pros.waitForFinished(10*60*1000);
        }
        settings.setValue("updateToolsVersion", defVersionApplication);
    }
#endif

void MainWidget::closeEvent(QCloseEvent *event)
{
    bool bClose = true;
    if(ui->blockPad->needSaving()
            &&
       ui->stackedWidget->currentWidget() == ui->blockPad)
    {
        auto button = QMessageBox::warning(this, "BlockPad", "You have unsaved data. Do you want to save them?",
                                 QMessageBox::Ok, QMessageBox::No, QMessageBox::Cancel);

        if(button == QMessageBox::Ok)
            ui->blockPad->slotSaveEncrypt();
        if(button == QMessageBox::Cancel)
        {
            event->ignore();
            bClose = false;
        }
    }

    if(bClose)
    {
        ui->blockPad->closeSeparateWgts();
        event->accept();
        //QWidget::closeEvent(event);
    }
}

void MainWidget::slotLockScreen()
{
    if(ui->stackedWidget->currentWidget() == ui->blockPad)
    {
        ui->stackedWidget->setCurrentWidget(ui->regist);
        ui->blockPad->closeSeparateWgts();
        ui->blockPad->activateWidgets(false);
        ui->blockPad->setMinimumWidth(0);
        ui->blockPad->setMinimumHeight(0);
        ui->regist->onLock();
        ui->regist->adjustSize();
        ui->stackedWidget->setFixedSize(ui->regist->size());
        setFixedSize(ui->regist->size());
        showNormal();
        bool b2FA = false;
        if(settings.value("2FA_On").type() != QVariant::Invalid)
        {
            b2FA = settings.value("2FA_On").toBool();
        }
        setGeometry(
            QStyle::alignedRect(
                Qt::LeftToRight,
                Qt::AlignCenter,
                QSize(ui->regist->size()),
                qApp->desktop()->availableGeometry()
            )
        );

    }
}

void MainWidget::slotSuccessRegistered()
{
    ui->stackedWidget->setCurrentWidget(ui->blockPad);   
    setMinimumSize(0,0);
    setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    ui->stackedWidget->setMinimumSize(0,0);
    ui->stackedWidget->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    ui->blockPad->setMinimumWidth(800);
    ui->blockPad->setMinimumHeight(400);
    showMaximized();
    ui->blockPad->Init();
    ui->blockPad->activateWidgets(true);
}


void MainWidget::slotSuccessUnlocked()
{
    ui->stackedWidget->setCurrentWidget(ui->blockPad);   
    setMinimumSize(0,0);
    setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    ui->stackedWidget->setMinimumSize(0,0);
    ui->stackedWidget->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    ui->blockPad->setMinimumWidth(800);
    ui->blockPad->setMinimumHeight(400);
    ui->blockPad->activateWidgets(true);
    showMaximized();
}

MainWidget::~MainWidget()
{
    delete ui;
}
