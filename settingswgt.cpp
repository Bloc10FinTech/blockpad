#include "settingswgt.h"
#include "ui_settingswgt.h"
#include <QFontDatabase>
#include "global.h"
#include <QStringList>
#include <QDebug>
#include <QFile>
#include <QMessageBox>

SettingsWgt::SettingsWgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWgt)
{
    ui->setupUi(this);
    QFontDatabase database;
    auto families = database.families();
    ui->comboBoxFonts->addItems(families);
    ui->comboBoxFonts->setCurrentText(qApp->font().family());
    Utilities::setAppFamilyFont(ui->labelSettingsTitle, 16,QFont::Bold);
    setAttribute(Qt::WA_DeleteOnClose);
    //load settings
    {
        if(settings.value("MakePasswordsHidden").type() != QVariant::Invalid)
        {
            auto allways = !settings.value("MakePasswordsHidden").toBool();
            ui->checkBoxPasswVisible->setChecked(allways);
            //ui->tableWidgetAccounts->slotAllwaysChecked(!allways);
        }

        if(settings.value("2FA_On").type() != QVariant::Invalid)
        {
            auto on = settings.value("2FA_On").toBool();
            ui->checkBox2FA_On->setChecked(on);
        }

        if(settings.value("LockScreen_Time").type() != QVariant::Invalid)
        {
            auto time = settings.value("LockScreen_Time").toInt();
            ui->spinBoxXMinutesLockScreen->setValue(time);
        }
    }
    //add pixmap to label
    {
        QPixmap p("://Icons/Block10_Logo.png");
        int w = ui->labelLogo->width();
        int h = ui->labelLogo->height();

        ui->labelLogo->setPixmap(p.scaled(w,h,
                                          Qt::KeepAspectRatio,
                                          Qt::SmoothTransformation));
    }

    //signals-slots connects
    {
        connect(ui->comboBoxFonts, SIGNAL(currentIndexChanged(QString)),
                this, SLOT(slotFontAppChanged(QString)));
        connect(ui->checkBoxPasswVisible, &QCheckBox::toggled,
                this, &SettingsWgt::slotPasswordVisibleClicked);
        connect(ui->checkBox2FA_On, &QCheckBox::toggled,
                this, &SettingsWgt::slot2FA_On_Clicked);
        connect(ui->spinBoxXMinutesLockScreen, &QSpinBox::editingFinished,
                this, &SettingsWgt::slotLockScreen_Time_FinishEditing);
    }
}

void SettingsWgt::slotFontAppChanged(QString newFamily)
{
    foreach (QWidget *widget, QApplication::allWidgets())
    {
        QFont font = widget->font();
        font.setFamily(newFamily);
        widget->setFont(font);
    }
    qApp->setFont (QFont (newFamily, appFontPointSize, appFontWeight));
    settings.setValue("Font", newFamily);
    settings.sync();
}

void SettingsWgt::slotPasswordVisibleClicked(bool bCheck)
{
    settings.setValue("MakePasswordsHidden", !bCheck);
    emit sigPasswordVisible(!bCheck);
    //ui->tableWidgetAccounts->slotAllwaysChecked(!bCheck);
}

void SettingsWgt::slot2FA_On_Clicked(bool bCheck)
{
    settings.setValue("2FA_On", bCheck);
    settings.sync();
}


void SettingsWgt::slotLockScreen_Time_FinishEditing()
{
    settings.setValue("LockScreen_Time", ui->spinBoxXMinutesLockScreen->value());
    settings.sync();
    emit sigScreenLock_Time(ui->spinBoxXMinutesLockScreen->value());
}

SettingsWgt::~SettingsWgt()
{
    delete ui;
}
