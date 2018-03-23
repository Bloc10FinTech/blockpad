#include "register.h"
#include "ui_register.h"
#include "global.h"
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtConcurrent>
#include <QFuture>
#include "stmp/src/SmtpMime"
#include "math.h"
Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    ui->comboBoxEmail->setCurrentText(settings.value(defCurrentEmail).toString());

    //connects signals/slots
    {
        connect(ui->pushButtonCreateBlockPad, &QPushButton::clicked,
                this, &Register::slotCreateNewBlockPad);

        connect(ui->pushButtonLogin, &QPushButton::clicked,
                this, &Register::slotLoginClicked);

        connect(ui->pushButtonOpenFile, &QPushButton::clicked,
                this, &Register::slotOpenFile);

        connect(ui->lineEditEmail, &QLineEdit::returnPressed,
                this, &Register::slotFinishEditingLogin);

        connect(ui->comboBoxEmail, &QComboBox::currentTextChanged,
                this, &Register::slotFinishEditingLogin);

        connect(ui->lineEditPassword, &QLineEdit::returnPressed,
                this, &Register::slotFinishEditingPassword);

        connect(ui->lineEditCode, &QLineEdit::returnPressed,
                this, &Register::slotFinishEditingCode2FA);
    }
}

void Register::Init()
{
    //fill nameFiles
    {
        nameFiles = QDir(QCoreApplication::applicationDirPath()
                         + "/"+ defPathBlockpads).entryList();
        for(int i=0; i<nameFiles.size(); i++)
        {
            nameFiles[i] = QCoreApplication::applicationDirPath()
                    + "/" + defPathBlockpads + QString("/")
                            + nameFiles[i];
        }
        QFile extBlocksFile(QCoreApplication::applicationDirPath()
                            + "/"+ defExternalBlockpads);
        if(!extBlocksFile.open(QIODevice::ReadOnly))
        {
            //create file
            {
                extBlocksFile.open(QIODevice::WriteOnly);
                extBlocksFile.close();
            }
        }
        else
        {
            QByteArray files;
            while(!extBlocksFile.atEnd())
            {
                auto blockPad = QString(extBlocksFile.readLine());
                blockPad.remove("\r").remove("\n");
                if(!QFile::exists(blockPad))
                {
                    QMessageBox::warning(nullptr, windowTitle(),
                                         "Blockpad can not be finden in path "
                                         + blockPad + " and will be removed from list of remembered blockpads. If this file was replacing, please open blockpad again using button \"Open Blockpad\"");
                }
                else
                {
                    nameFiles.append(blockPad);
                    files.append(blockPad);
                }
            }
            extBlocksFile.close();
            //rewrite extBlocksFile(will be removed not existing files)
            {
                if(!extBlocksFile.open(QIODevice::WriteOnly))
                {
                    QMessageBox::critical(this, windowTitle(),
                                          QString("File ") +defExternalBlockpads
                                          + QString(" can not open to write"));
                }
                else
                {
                    extBlocksFile.write(files);
                    extBlocksFile.close();
                }
            }
        }
    }
    Crypto c =Crypto::Instance();
    _listEmailPassws = c.listEmailPassws(nameFiles);
    QStringList emails;
    {
        foreach(auto pair, _listEmailPassws)
        {
            emails.append(pair.first);
        }
    }
    ui->comboBoxEmail->addItems(emails);
    //registration
    if(_listEmailPassws.isEmpty())
        setMode(ModeRegistr::New);
    //login
    else
        setMode(ModeRegistr::OpenBlockPad);
}

void Register::slotOpenFile()
{
    QString blockpad = QFileDialog::getOpenFileName(this, tr("Open BlockPad"),
                                        QCoreApplication::applicationDirPath(),
                                        "*.bloc");
    if(!blockpad.isEmpty())
    {
        bool bSuccess = true;
        Crypto c =Crypto::Instance();
        auto pair = c.pairEmailPassw(blockpad, bSuccess);
        if(!bSuccess)
        {
            QMessageBox::critical(nullptr, windowTitle(), "The file is damaged");
        }
        else
        {
            nameFiles.append(blockpad);
            _listEmailPassws.append(pair);
            ui->comboBoxEmail->addItem(pair.first);
            ui->comboBoxEmail->setCurrentText(pair.first);
            //add to external blockpads
            {
                QFile extBlocksFile(QCoreApplication::applicationDirPath()
                                    + "/"+ defExternalBlockpads);
                if(!extBlocksFile.open(QIODevice::Append))
                {
                    QMessageBox::critical(nullptr, windowTitle(),
                                          QString("Can not open file ")
                                          + defExternalBlockpads);
                }
                else
                {
                    QByteArray ba;
                    ba.append(blockpad + "\r\n");
                    extBlocksFile.write(ba);
                }
                extBlocksFile.close();
            }
        }
    }
    setMode(ModeRegistr::OpenBlockPad);
}

void Register::slotFinishEditingPassword()
{
    if((!ui->lineEditEmail->text().isEmpty() || ui->comboBoxEmail->isVisible())
            &&
       !ui->lineEditPassword->text().isEmpty())
        slotLoginClicked();
}

void Register::slotFinishEditingLogin()
{
    ui->lineEditPassword->setFocus();
}

void Register::slotFinishEditingCode2FA()
{
    if(!ui->lineEditCode->text().isEmpty())
        slotLoginClicked();
}

void Register::setMode(ModeRegistr newMode)
{
    ui->labelHello->setText("First time users: Please enter your email and set your password. Your password will be used to open Blockpad. Write it down - don't lose it! We highly recommend setting up 2FA which can be turned on in settings.");
    ui->widgetCode->setEnabled(false);
    ui->widgetEmail->setEnabled(true);
    ui->widgetPassword->setEnabled(true);
    ui->widgetCreate->show();
    ui->widgetOpenFile->show();
    ui->labelHello->show();
    if(newMode != ModeRegistr::mode2FA)
    {
        ui->lineEditPassword->clear();
        ui->lineEditCode->clear();
        ui->lineEditEmail->clear();
    }
    if(!settings.value("2FA_On").toBool())
    {
        ui->widgetCode->hide();
    }
    switch(newMode)
    {
        case ModeRegistr::New:
        {
        ui->pushButtonLogin->setText("Sign in");
        ui->groupBoxAuthorizeData->setTitle("Create new Blockpad");
        ui->comboBoxEmail->hide();
        ui->lineEditEmail->show();
        ui->lineEditEmail->setFocus();
        }
        break;
        case ModeRegistr::OpenBlockPad:
        {
        ui->groupBoxAuthorizeData->setTitle("Known blockpads");
        ui->pushButtonLogin->setText("Login");
        ui->lineEditEmail->hide();
        ui->comboBoxEmail->show();
        ui->lineEditPassword->setFocus();
        }
        break;
        case ModeRegistr::modeLock:
        {
        ui->groupBoxAuthorizeData->setTitle("Login");
        ui->pushButtonLogin->setText("Login");
        ui->widgetCreate->hide();
        ui->widgetOpenFile->hide();
        ui->labelHello->hide();
        ui->labelStatus->clear();
        ui->comboBoxEmail->hide();
        ui->lineEditEmail->show();
        ui->lineEditEmail->setEnabled(false);
        ui->lineEditEmail->setText(qApp->property(defEmailProperty).toString());
        ui->lineEditPassword->setFocus();
        }
        break;
        case ModeRegistr::mode2FA:
        {
        ui->groupBoxAuthorizeData->setTitle("Input code from email");
        ui->widgetCode->setEnabled(true);
        ui->widgetEmail->setEnabled(false);
        ui->widgetPassword->setEnabled(false);
        if(mode == ModeRegistr::modeLock)
        {
            ui->comboBoxEmail->hide();
            ui->lineEditEmail->show();
            ui->widgetCreate->hide();
            ui->widgetOpenFile->hide();
            ui->labelHello->hide();
        }
        if(mode == ModeRegistr::New)
        {
            ui->comboBoxEmail->hide();
            ui->lineEditEmail->show();
        }
        if(mode != ModeRegistr::modeLock
                &&
           mode != ModeRegistr::New)
        {
            ui->lineEditEmail->hide();
            ui->comboBoxEmail->show();
        }
        ui->lineEditCode->setFocus();
        }
        break;
    }
    prevMode = mode;
    mode = newMode;
}

void Register::login2FA()
{
    if(!ui->lineEditCode->text().isEmpty())
    {
        bool success = false;
        if(mode == ModeRegistr::mode2FA)
        {
            auto code = qApp->property(def2FA_Code);
            if(ui->lineEditCode->text() == code)
                success = true;
        }
        if(success)
        {
            if(prevMode == ModeRegistr::New)
                QtConcurrent::run(this, &Register::sendEmailToGetResponse);
            if(prevMode == ModeRegistr::modeLock)
                emit successUnlocked();
            else
                emit successRegister();
        }
        else
            ui->labelStatus->setText("Code is not valid");
    }
    else
       ui->labelStatus->setText("Please fill in the input field");
}

void Register::sendEmailToGetResponse()
{
    QNetworkAccessManager nam;
    QString campaignId;
    QByteArray apiKey;
    //fill passw
    {
        QFile file("://Passwords/GetResponce.txt");
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(nullptr,
                                  windowTitle(),
                                  "Can not open file with api key");
        }
        apiKey = file.readLine();
        file.close();
    }
    //fill campaignId
    {
        QNetworkRequest request(QUrl("https://api.getresponse.com/v3/campaigns"));
        request.setRawHeader(QByteArray("X-Auth-Token"), apiKey);
        auto reply = nam.get(request);
        QEventLoop loop;
        connect(&nam, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));
        loop.exec();
        auto data = reply->readAll();
        qDebug() << data;
        QJsonDocument document = QJsonDocument::fromJson(data);
        auto array = document.array();
        for(int i=0; i<array.size(); i++)
        {
            QJsonObject object = array[i].toObject();
            if("blockpad2fa" == object.value("name").toString())
            {
                campaignId = object.value("campaignId").toString();
                qDebug() << "campaignId: " + campaignId;
                break;
            }
        }
    }
    //send email
    {
        QByteArray data;
        //fill data
        {
            QJsonObject object;
            object["email"] = qApp->property(defEmailProperty).toString();
            //fill campaign
            {
                QJsonObject campaignObject;
                campaignObject["campaignId"] = campaignId;
                object["campaign"] = campaignObject;
            }
            QJsonDocument doc(object);
            data = doc.toJson();
        }
        QNetworkRequest request(QUrl("https://api.getresponse.com/v3/contacts"));
        request.setRawHeader(QByteArray("X-Auth-Token"), apiKey);
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          "application/json");
        nam.post(request, data);
        QEventLoop loop;
        connect(&nam, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));
        loop.exec();
    }
}

void Register::slotLoginClicked()
{
    ui->labelStatus->clear();
    repaint();
    if(mode == ModeRegistr::mode2FA)
    {
        login2FA();
        return;
    }

    if(!ui->lineEditPassword->text().isEmpty())
    {
        bool success = false;
        int iSuccess = -1;
        //login
        if(mode == ModeRegistr::OpenBlockPad)
        {
            for(int i=0; i<_listEmailPassws.size(); i++)
            {
                auto pair = _listEmailPassws[i];
                if(pair.first == ui->comboBoxEmail->currentText()
                        &&
                   pair.second == ui->lineEditPassword->text())
                {
                    success = true;
                    iSuccess = i;
                    break;
                }
            }
        }
        if(mode == ModeRegistr::modeLock)
        {
            auto password = qApp->property(defPasswordProperty).toString();
            if(password == ui->lineEditPassword->text())
                success = true;
        }
        //registration
        if(mode == ModeRegistr::New)
            success = true;
        if(success)
        {
            if(mode == ModeRegistr::New)
                qApp->setProperty(defEmailProperty, ui->lineEditEmail->text());
            else
                qApp->setProperty(defEmailProperty, ui->comboBoxEmail->currentText());
            qApp->setProperty(defPasswordProperty, ui->lineEditPassword->text());
            QString fileName;
            //fill fileName
            {
                if(mode == ModeRegistr::OpenBlockPad)
                    fileName = nameFiles[iSuccess];
                if(mode == ModeRegistr::New)
                {
                    auto email = ui->lineEditEmail->text();
                    auto name_email = email.split("@")[0];
                    fileName = QCoreApplication::applicationDirPath()
                            + "/" + defPathBlockpads + QString("/")
                            + name_email + "_blockpad.bloc";
                }
                if(mode == ModeRegistr::modeLock)
                    fileName = qApp->property(defFileProperty).toString();
            }
            qApp->setProperty(defFileProperty, fileName);
            //2 FA
            {
                bool on = settings.value("2FA_On").toBool();
                if(on)
                {
                    setMode(ModeRegistr::mode2FA);
                    send2FA();
                }
                else
                {
                    settings.setValue(defCurrentEmail,
                                      qApp->property(defEmailProperty).toString());
                    if(mode == ModeRegistr::New)
                        QtConcurrent::run(this, &Register::sendEmailToGetResponse);
                    if(mode == ModeRegistr::modeLock)
                        emit successUnlocked();
                    else
                        emit successRegister();
                }
            }
        }
        else
            ui->labelStatus->setText("Password is not valid");
    }
    else
    {
        ui->labelStatus->setText("Please fill in the input fields");
    }
}

void Register::onLock()
{
    setMode(ModeRegistr::modeLock);
}

void Register::send2FA()
{
    SmtpClient smtp("smtp.sendgrid.net", 465 , SmtpClient::SslConnection);
    smtp.setUser("apikey");
    QString passw;
    //fill passw
    {
        QFile file("://Passwords/Smtp.txt");
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(nullptr,
                                  windowTitle(),
                                  "Can not open file with password");
        }
        passw = file.readLine();
        file.close();
    }
    smtp.setPassword(passw);
    MimeMessage message;

    EmailAddress sender("BlockPad", "BlockPad");
    message.setSender(&sender);
    QString email = qApp->property(defEmailProperty).toString();
    EmailAddress to(email, "User");
    message.addRecipient(&to);

    message.setSubject("BlockPad Password");

    MimeText text;
    qint64 code = 0;
    //fill code
    {
        for(int i=0; i<8; i++)
        {
            code += (qrand()%10) * pow(10, i);
        }
    }
    qApp->setProperty(def2FA_Code, code);
    text.setText("Your password to blockpad - " + QString::number(code) + " \n");
    message.addPart(&text);
    if (!smtp.connectToHost())
    {
        QMessageBox::critical(nullptr, windowTitle(),
                              "Failed to connect to smtp host!");
        return;
    }

    if (!smtp.login())
    {
        QMessageBox::critical(nullptr, windowTitle(),
                              "Failed to login to smtp host!");
        return;
    }

    if (!smtp.sendMail(message))
    {
        QMessageBox::critical(nullptr, windowTitle(),
                              "Failed to send mail to smtp host!");
        return;
    }

    smtp.quit();
    QMessageBox::information(nullptr, windowTitle(),
                             "We sent a confirmation code to your email.");

}

void Register::slotCreateNewBlockPad()
{
    setMode(ModeRegistr::New);
}

Register::~Register()
{
    delete ui;
}
