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
#include "stega/steganography.h"

extern QString fileInit;
Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    bool on = settings.value("2FA_On").toBool();
    //hello message
    {
        if(on)
        {
            ui->labelHello->setTextFormat(Qt::RichText);
            ui->labelHello->setTextInteractionFlags(Qt::TextBrowserInteraction);
            ui->labelHello->setText("Welcome, Blockpad user. You have turned on 2FA. Enter your username, password, and click 'get code' to get your 2FA code. When you have entered all 3, click Login. If you are a first time user, click <html><a style = 'text-decoration:none'href ='firstUserPage'>here</a></html>.");
        }
        else
        {
            ui->labelHello->setText("First time users: Please enter your email and set your password. Your password will be used to open Blockpad. Write it down - don't lose it! We highly recommend setting up 2FA which can be turned on in settings.");
        }
    }
    //connects signals/slots
    {
        connect(ui->pushButtonCreateBlockPad, &QPushButton::clicked,
                this, &Register::slotCreateNewBlockPad);

        connect(ui->pushButtonLogin, &QPushButton::clicked,
                this, &Register::slotLoginClicked);

        connect(ui->pushButtonGetCode, &QPushButton::clicked,
                this, &Register::slotLoginClicked);

        connect(ui->pushButtonOpenFile, &QPushButton::clicked,
                this, &Register::slotOpenFile);

        connect(ui->pushButtonRetrieveFile, &QPushButton::clicked,
                this, &Register::slotRetreiveCloudClicked);

        connect(ui->lineEditEmail, &QLineEdit::returnPressed,
                this, &Register::slotFinishEditingLogin);

        connect(ui->comboBoxEmail, &QComboBox::currentTextChanged,
                this, &Register::slotFinishEditingLogin);

        connect(ui->comboBoxId, &QComboBox::currentTextChanged,
                this, &Register::slotFinishChoosingId);

        connect(ui->lineEditPassword, &QLineEdit::returnPressed,
                this, &Register::slotFinishEditingPassword);

        connect(ui->lineEditCode, &QLineEdit::returnPressed,
                this, &Register::slotFinishEditingCode2FA);

        connect(ui->labelHello, &QLabel::linkActivated,
                this, &Register::slotHelloLinkActivated);
    }
    ui->lineEditEmail->setVisible(false);
}

void Register::slotRetreiveCloudClicked()
{
    if(retreiveCloudWgt.isNull())
    {
        retreiveCloudWgt = new RetreiveCloudWgt();
        retreiveCloudWgt->show();
    }
    else
        retreiveCloudWgt->activateWindow();
}

void Register::slotFinishChoosingId()
{
    if(ui->comboBoxId->currentText() != defNoneId)
    {
        auto file = fileIds.key(ui->comboBoxId->currentText());
        int displayIndex = nameFiles.indexOf(file);
        ui->comboBoxEmail->setCurrentIndex(displayIndex);
    }
}

void Register::timerEvent(QTimerEvent *event)
{
    if(resendId == event->timerId())
    {
        resendTime--;
        if(resendTime > 0)
            ui->labelStatus->setText("You can resend email with code through "
                                     + QString::number(resendTime) + " seconds");
        else
        {
            ui->pushButtonGetCode->setEnabled(true);
            killTimer(resendId);
            ui->labelStatus->clear();
        }
    }
}

void Register::slotHelloLinkActivated(QString link)
{
    ui->labelHello->setText("First time users: Please enter your email and set your password. Your password will be used to open Blockpad. Write it down - don't lose it! We highly recommend setting up 2FA which can be turned on in settings.");
}

void Register::Init()
{
    //fill nameFiles
    {
        nameFiles = QDir(Utilities::filesDirectory()
                         + "/"+ defPathBlockpads).entryList();
        for(int i=0; i<nameFiles.size(); i++)
        {
            nameFiles[i] = Utilities::filesDirectory()
                    + "/" + defPathBlockpads + QString("/")
                            + nameFiles[i];
        }
        QFile extBlocksFile(Utilities::filesDirectory()
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
                    files.append(blockPad + "\r\n");
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
    QStringList emails;
    _listEmailPassws = c.listEmailPassws(nameFiles,
                                         fileIds,
                                         emails);
    ui->comboBoxEmail->addItems(emails);
    ui->comboBoxId->addItem(defNoneId);
    ui->comboBoxId->addItems(fileIds.values());
    //load current email
    {
        auto currentFile = settings.value(defCurrentFile).toString();
        int currentIndex = nameFiles.indexOf(currentFile);
        if(currentIndex != -1)
        {
            ui->comboBoxEmail->setCurrentIndex(currentIndex);
        }
    }
    //current id
    {
        if(ui->comboBoxEmail->currentIndex()!= -1)
        {
            auto currentFile = nameFiles[ui->comboBoxEmail->currentIndex()];
            if(fileIds.contains(currentFile))
                ui->comboBoxId->setCurrentText(fileIds[currentFile]);
            else
                ui->comboBoxId->setCurrentText(defNoneId);
        }
    }
    //registration
    if(_listEmailPassws.isEmpty())
        setMode(ModeRegistr::New);
    //login
    else
        setMode(ModeRegistr::OpenBlockPad);
    if(!fileInit.isEmpty())
        OpenFile(fileInit);
    ui->comboBoxEmail->setProperty("nullProp", false);
    ui->comboBoxEmail->style()->unpolish(ui->comboBoxEmail);
    ui->comboBoxEmail->style()->polish(ui->comboBoxEmail);
    if(ui->comboBoxEmail->currentIndex()!= -1)
        ui->comboBoxEmail->setToolTip(nameFiles[ui->comboBoxEmail->currentIndex()]);
}

QString Register::currentEmail()
{
    QString res;
    if(ui->comboBoxEmail->currentIndex() >= 0)
        res = _listEmailPassws[ui->comboBoxEmail->currentIndex()].first;
    return res;
}

void Register::slotOpenFile()
{
    killTimer(resendId);
    ui->labelStatus->clear();
    QString blockpad = QFileDialog::getOpenFileName(this, tr("Open BlockPad"),
                                        Utilities::filesDirectory(),
                                        "*.bloc");
    OpenFile(blockpad);
}

void Register::OpenFile(QString blockpad)
{
    if(!blockpad.isEmpty())
    {
        bool bSuccess = true;
        Crypto c =Crypto::Instance();
        QString id;
        auto pair = c.pairEmailPassw(blockpad, bSuccess,id);
        if(!bSuccess)
        {
            QMessageBox::critical(nullptr, windowTitle(), "The file is damaged");
        }
        else
        {
            int index=1;
            auto email = pair.first;
            auto displayEmail = pair.first;
            if(!nameFiles.contains(blockpad)
                    &&
               !nameFiles.contains(QString(blockpad).replace("/", "\\"))
                    &&
               !nameFiles.contains(QString(blockpad).replace("\\", "/")))
            {
                while(1)
                {
                    if(ui->comboBoxEmail->findText(displayEmail) == -1)
                         break;
                    else
                    {
                        displayEmail = email+ " (" + QString::number(index) + ")";
                        index++;
                    }
                }
                nameFiles.append(blockpad);
                _listEmailPassws.append(pair);
                ui->comboBoxEmail->addItem(displayEmail);
                if(!fileIds.contains(id))
                {
                    fileIds[blockpad] = id;
                    ui->comboBoxId->addItem(id);
                }
                //add to external blockpads
                {
                    QFile extBlocksFile(Utilities::filesDirectory()
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
                        extBlocksFile.seek(extBlocksFile.size());
                        extBlocksFile.write(ba);
                    }
                    extBlocksFile.close();
                }
            }
            if(nameFiles.contains(QString(blockpad).replace("/", "\\")))
                blockpad.replace("/", "\\");
            if(nameFiles.contains(QString(blockpad).replace("\\", "/")))
                blockpad.replace("\\", "/");
            ui->comboBoxEmail->setCurrentIndex(nameFiles.indexOf(blockpad));
            if(ui->comboBoxId->findText(id) == -1)
                ui->comboBoxId->addItem(id);
            ui->comboBoxId->setCurrentText(id);
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
    if(!ui->comboBoxEmail->isHidden()
            &&
       ui->comboBoxEmail->currentIndex() != -1)
        ui->comboBoxEmail->setToolTip(nameFiles[ui->comboBoxEmail->currentIndex()]);
    if(!ui->widgetId->isHidden())
    {
        auto currentFile = nameFiles[ui->comboBoxEmail->currentIndex()];
        if(fileIds.contains(currentFile))
            ui->comboBoxId->setCurrentText(fileIds[currentFile]);
        else
            ui->comboBoxId->setCurrentText(defNoneId);
    }
}

void Register::slotFinishEditingCode2FA()
{
    if(!ui->lineEditCode->text().isEmpty())
        slotLoginClicked();
}

void Register::setMode(ModeRegistr newMode)
{
    ui->widgetCode->setEnabled(false);
    ui->widgetEmail->setEnabled(true);
    ui->widgetPassword->setEnabled(true);
    ui->widgetId->setEnabled(true);
    ui->pushButtonCreateBlockPad->show();
    ui->pushButtonOpenFile->show();
    ui->labelHello->show();
    ui->pushButtonGetCode->setEnabled(true);
    ui->widgetId->hide();
    ui->labelEmpty->show();
    if(newMode != ModeRegistr::mode2FA)
    {
        ui->lineEditPassword->clear();
        ui->lineEditCode->clear();
        ui->lineEditEmail->clear();
    }
    if(!settings.value("2FA_On").toBool())
    {
        ui->widgetCode->hide();
        ui->widgetGetCode->hide();
    }
    else
        ui->pushButtonLogin->setEnabled(false);
    switch(newMode)
    {
        case ModeRegistr::New:
        {
        ui->pushButtonLogin->setText("Sign in");
        ui->groupBoxAuthorizeData->setTitle("Create new Blockpad");
        ui->comboBoxEmail->hide();
        ui->lineEditEmail->show();
        ui->lineEditEmail->setFocus();
        ui->comboBoxId->setCurrentText(defNoneId);
        }
        break;
        case ModeRegistr::OpenBlockPad:
        {
        ui->groupBoxAuthorizeData->setTitle("Known blockpads");
        ui->pushButtonLogin->setText("Login");
        ui->lineEditEmail->hide();
        ui->comboBoxEmail->show();
        ui->lineEditPassword->setFocus();
        ui->widgetId->show();
        ui->labelEmpty->hide();
        }
        break;
        case ModeRegistr::modeLock:
        {
        ui->groupBoxAuthorizeData->setTitle("Login");
        ui->pushButtonLogin->setText("Login");
        ui->pushButtonCreateBlockPad->hide();
        ui->pushButtonOpenFile->hide();
        ui->labelHello->hide();
        ui->labelStatus->clear();
        ui->comboBoxEmail->hide();
        ui->lineEditEmail->show();
        ui->lineEditEmail->setEnabled(false);
        ui->lineEditEmail->setText(qApp->property(defEmailProperty).toString());
        ui->lineEditPassword->setFocus();
        ui->widgetId->hide();
        }
        break;
        case ModeRegistr::mode2FA:
        {
        ui->groupBoxAuthorizeData->setTitle("Input code from email");
        ui->widgetCode->setEnabled(true);
        ui->widgetEmail->setEnabled(false);
        ui->widgetId->setEnabled(false);
        ui->widgetPassword->setEnabled(false);
        ui->pushButtonLogin->setEnabled(true);
        ui->pushButtonGetCode->setEnabled(false);
        if(mode == ModeRegistr::modeLock)
        {
            ui->comboBoxEmail->hide();
            ui->lineEditEmail->show();
            ui->pushButtonCreateBlockPad->hide();
            ui->pushButtonOpenFile->hide();
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
            ui->widgetId->show();
            ui->labelEmpty->hide();
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
        SteganoReaderWriter st;
        st.decode_img("://Icons/settings.png");
        QString str = st.read_data();
        apiKey.append(str);
    }
    //fill campaignId
    {
        QNetworkRequest request(QUrl("https://api.getresponse.com/v3/campaigns"));
        request.setRawHeader(QByteArray("X-Auth-Token"), apiKey);
        QEventLoop loop;
        connect(&nam, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));
        auto reply = nam.get(request);
        if(!reply->isFinished())
            loop.exec();
        auto data = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(data);
        auto array = document.array();
        for(int i=0; i<array.size(); i++)
        {
            QJsonObject object = array[i].toObject();
            if("blockpad2fa" == object.value("name").toString())
            {
                campaignId = object.value("campaignId").toString();
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
        QEventLoop loop;
        connect(&nam, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));
        auto reply = nam.post(request, data);
        if(!reply->isFinished())
            loop.exec();
        auto data_ = reply->readAll();
        auto statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
        qDebug() << "statusCode: " << statusCode;
    }
}

void Register::slotLoginClicked()
{
    ui->labelStatus->clear();
    repaint();
    if(mode == ModeRegistr::mode2FA
            &&
       (ui->pushButtonLogin->hasFocus()
        ||
        ui->lineEditCode->hasFocus()))
    {
        login2FA();
        return;
    }
    if(mode == ModeRegistr::mode2FA
            &&
       ui->pushButtonGetCode->hasFocus())
    {
        mode = prevMode;
    }
    if(!ui->lineEditPassword->text().isEmpty())
    {
        bool success = false;
        //login
        if(mode == ModeRegistr::OpenBlockPad)
        {
//            for(int i=0; i<_listEmailPassws.size(); i++)
//            {
                auto pair = _listEmailPassws[ui->comboBoxEmail->currentIndex()];
                if(pair.second == ui->lineEditPassword->text())
                {
                    success = true;
                }
//            }
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
                qApp->setProperty(defEmailProperty, currentEmail());
            qApp->setProperty(defPasswordProperty, ui->lineEditPassword->text());
            QString fileName;
            //fill fileName
            {
                if(mode == ModeRegistr::OpenBlockPad)
                    fileName = nameFiles[ui->comboBoxEmail->currentIndex()];
                if(mode == ModeRegistr::New)
                {
                    auto email = ui->lineEditEmail->text();
                    auto name_email = email.split("@")[0];
                    auto baseName = Utilities::filesDirectory()
                            + "/" + defPathBlockpads + QString("/")
                            + name_email + "_blockpad";
                    fileName = baseName + ".bloc";
                    if(QFile(fileName).exists())
                    {
                        int index = 1;
                        while(1)
                        {
                            if(!QFile(baseName + "_"
                               + QString::number(index) + ".bloc").exists())
                                break;
                            else
                                index++;
                        }
                        fileName = baseName + "_"
                           + QString::number(index) + ".bloc";
                    }
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
                    if(send2FA())
                        setMode(ModeRegistr::mode2FA);
                }
                else
                {
                    settings.setValue(defCurrentFile,
                                      qApp->property(defFileProperty).toString());
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

bool Register::send2FA()
{
    SmtpClient smtp("smtp.sendgrid.net", 465 , SmtpClient::SslConnection);
    smtp.setUser("apikey");
    QString passw;
    //fill passw
    {
        SteganoReaderWriter st;
        st.decode_img("://Icons/Save.png");
        passw = st.read_data();
    }
    smtp.setPassword(passw);
    MimeMessage message;

    EmailAddress sender("BlockPad", "BlockPad");
    message.setSender(&sender);
    QString email = qApp->property(defEmailProperty).toString();
    EmailAddress to(email, "User");
    message.addRecipient(&to);

    message.setSubject("BlockPad Code");

    MimeHtml html;
    qint64 code = 0;
    //fill code
    {
        for(int i=0; i<8; i++)
        {
            code += (qrand()%10) * pow(10, i);
        }
    }
    qApp->setProperty(def2FA_Code, code);
//    html.setHtml("<body> <b>Your Code is: " + QString::number(code)
//    + "<br><br><br><br><br><br> <a href=\"https://www.blockpad.io\">www.blockpad.io</a>\n" + "<//body>");

    html.setHtml("<b>Your Code is: " + QString::number(code)
    + "<br><br><br><br><br><br> <a href=\"https://www.blockpad.io\">www.blockpad.io</a>\n");

    //text.setText("Your Code is: " + QString::number(code) + " \n");
    message.addPart(&html);
    if (!smtp.connectToHost())
    {
        QMessageBox::critical(nullptr, windowTitle(),
                              "Failed to connect to smtp host!");
        return false;
    }

    if (!smtp.login())
    {
        QMessageBox::critical(nullptr, windowTitle(),
                              "Failed to login to smtp host!");
        return false;
    }

    if (!smtp.sendMail(message))
    {
        QMessageBox::critical(nullptr, windowTitle(),
                              "Failed to send mail to smtp host!");
        return false;
    }

    smtp.quit();
    QMessageBox::information(nullptr, windowTitle(),
                             "We sent a confirmation code to your email.");

    resendTime = 60;
    ui->labelStatus->setText("You can resend email with code through "
                                 + QString::number(resendTime) + " seconds");
    resendId = startTimer(1000);
    ui->pushButtonGetCode->setEnabled(false);
    return true;
}

void Register::slotCreateNewBlockPad()
{
    killTimer(resendId);
    ui->labelStatus->clear();
    setMode(ModeRegistr::New);
}

Register::~Register()
{
    delete ui;
}
