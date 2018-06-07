#include "retreivecloudwgt.h"
#include "ui_retreivecloudwgt.h"
#include <QMessageBox>
#include <QStandardPaths>
#include "global.h"
#include "stega/steganography.h"
#include <QFile>
#include <QProcess>

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <iostream>
#include <fstream>

RetreiveCloudWgt::RetreiveCloudWgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RetreiveCloudWgt)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window);
    //table
    {
        QStringList list;
        for(int i_Column=0; i_Column<columnsRetreiveCloud::nColumns;i_Column++)
        {
            switch(i_Column)
            {
                case columnsRetreiveCloud::LicenseKey:
                {
                    list<< "License key";
                }
                break;
                case columnsRetreiveCloud::devName:
                {
                    list<< "Device";
                }
                break;
                case columnsRetreiveCloud::id:
                {
                    list<< "ID";
                }
                break;
                case columnsRetreiveCloud::download:
                {
                    list<< "";
                }
                break;
            }
        }
        ui->tableWidgetCloudBlockPads
             ->setColumnCount((int)columnsRetreiveCloud::nColumns);
        ui->tableWidgetCloudBlockPads
             ->setHorizontalHeaderLabels(list);
        {
            QHeaderView * HeaderView = ui->tableWidgetCloudBlockPads
                                        ->horizontalHeader();
            HeaderView->setSectionResizeMode(QHeaderView::Stretch);
            HeaderView->setSectionResizeMode(columnsRetreiveCloud::id,
                                             QHeaderView::ResizeToContents);
            HeaderView->setSectionResizeMode(columnsRetreiveCloud::download,
                                             QHeaderView::Fixed);
            HeaderView->resizeSection(columnsRetreiveCloud::download,
                                      30);
        }
    }

    //load settings
    {
        if(settings.value(defDownloadCloud).type() != QVariant::Invalid)
            ui->lineEditDownloadFolder->setText(settings.value(defDownloadCloud).toString());
        else
            ui->lineEditDownloadFolder->setText(
                    QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));

        if(settings.value(defAutoOpen).type() != QVariant::Invalid)
            ui->checkBoxAutoOpenBlockPad->setChecked(settings.value(defAutoOpen).toBool());
        else
            ui->checkBoxAutoOpenBlockPad->setChecked(true);
    }

    //signals-slots connects
    {
        connect(ui->lineEditDownloadFolder, &QLineEdit::editingFinished,
                this, &RetreiveCloudWgt::slotDownloadEditFinished);
        connect(ui->checkBoxAutoOpenBlockPad, &QCheckBox::toggled,
                this, &RetreiveCloudWgt::slotAutoOpenChanged);

        connect(ui->pushButtonLogin, &QPushButton::clicked,
                this, &RetreiveCloudWgt::slotLoginClicked);

        connect(&netwLicenseServer, &NetworkLicenseServer::sigBlockPadsFinished,
                this, &RetreiveCloudWgt::slotBlockPadsFinished);
    }

    //test
    {
//        QMap<QString, QMap<QString, QStringList>> blockpads;
//        blockpads["L7Q2VW-HH7VVX-JMN428-P8IV9G"]["dev_name1"] = QStringList() << "83" << "2" << "3";
//        blockpads["license1"]["dev_name2"] = QStringList() << "4" << "5" << "6";
//        blockpads["license2"]["dev_name1"] = QStringList() << "7" << "8" << "9";
//        blockpads["license3"]["dev_name3"] = QStringList() << "10333333333333" << "11" << "12";
//        slotBlockPadsFinished(true, blockpads);
    }

#if defined(WIN32) || defined(WIN64)
#else
    ui->checkBoxAutoOpenBlockPad->hide();
#endif
}

void RetreiveCloudWgt::slotDownloadClicked()
{
    QWidget *w = qobject_cast<QWidget *>(sender()->parent());
    if(w)
    {
        int row = ui->tableWidgetCloudBlockPads->indexAt(w->pos()).row();

        QString license = ui->tableWidgetCloudBlockPads
                ->item(row,columnsRetreiveCloud::LicenseKey)->text();
        QString devName = ui->tableWidgetCloudBlockPads
                ->item(row,columnsRetreiveCloud::devName)->text();
        QString id = ui->tableWidgetCloudBlockPads
                ->item(row,columnsRetreiveCloud::id)->text();

        QString strKey = license
                +"/" + devName
                +"/" + id + ".bloc";
        auto stdKey = strKey.toStdString();
        const char * Key =stdKey.data();


        Aws::SDKOptions options;
        options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Off;
        Aws::InitAPI(options);
        Aws::String accessKey;
        //fill accessKey
        {
            SteganoReaderWriter st;
            st.decode_img("://Icons/1TimePad.png");
            QString str = st.read_data();
            accessKey = str.toStdString().data();
        }
        Aws::String sequreKey;
        //fill sequreKey
        {
            SteganoReaderWriter st;
            st.decode_img("://Icons/UpdateIcon.png");
            QString str = st.read_data();
            sequreKey = str.toStdString().data();
        }
        Aws::Auth::AWSCredentials credentionals(accessKey,sequreKey);
        Aws::S3::S3Client s3_client(credentionals);

        Aws::S3::Model::GetObjectRequest object_request;
        object_request.WithBucket("blockpadcloud").WithKey(Key);

        auto get_object_outcome = s3_client.GetObject(object_request);

        if (get_object_outcome.IsSuccess())
        {
            Aws::OFStream local_file;
            QString baseFile =
              ui->lineEditDownloadFolder->text()
              + "/" + id;
            QString downloadFile = baseFile + ".bloc";
            int iN = 1;
            while(1)
            {
                if(QFile::exists(downloadFile))
                {
                    downloadFile = baseFile +"_" + QString::number(iN) + ".bloc";
                    iN++;
                }
                else
                    break;
            }
            auto stdDownloadFile = downloadFile.toStdString();
            const char * chDownloadFile =stdDownloadFile.data();
            local_file.open(chDownloadFile, std::ios::out | std::ios::binary);
            local_file << get_object_outcome.GetResult().GetBody().rdbuf();
#if defined(WIN32) || defined(WIN64)
            if(ui->checkBoxAutoOpenBlockPad->isChecked())
            {
                QProcess proc;
                proc.startDetached(QCoreApplication::applicationFilePath(),
                                   QStringList() << downloadFile);
            }
#endif
        }
        else
        {
            QMessageBox::critical(nullptr, windowTitle(),
                                  "Error: "
              + QString(get_object_outcome.GetError().GetExceptionName().data())
              + " " + QString(get_object_outcome.GetError().GetMessage().data()));
        }

        Aws::ShutdownAPI(options);
    }
}

void RetreiveCloudWgt::slotAutoOpenChanged(bool bCheck)
{
    settings.setValue(defAutoOpen, bCheck);
}

void RetreiveCloudWgt::slotDownloadEditFinished()
{
    QString downloadPath = ui->lineEditDownloadFolder->text();
    settings.setValue(defDownloadCloud, downloadPath);
}

void RetreiveCloudWgt::slotLoginClicked()
{
    //not all credentionals
    if(ui->lineEditPassword->text().isEmpty()
            ||
       ui->lineEditUsername->text().isEmpty())
    {
        QMessageBox::critical(nullptr, windowTitle(),
                              "Please input credentials!");
        if(ui->lineEditPassword->text().isEmpty())
            ui->lineEditPassword->setFocus();
        if(ui->lineEditUsername->text().isEmpty())
            ui->lineEditUsername->setFocus();

        return;
    }

    netwLicenseServer.sendCloudBlockPadsRequest(ui->lineEditUsername->text(),
                                                ui->lineEditPassword->text());
}

void RetreiveCloudWgt::slotBlockPadsFinished(bool bSuccess,
                          //QMap<license, QMap<device {id0, id1,...}>>
                          QMap<QString, QMap<QString, QStringList>> cloudBlockPads)
{
    if(!bSuccess)
        QMessageBox::critical(nullptr, windowTitle(),
                              "There is no such username or password is incorrect!");
    else
    {
        foreach(QString license, cloudBlockPads.keys())
        {
            auto licenseData = cloudBlockPads[license];
            foreach(QString device, licenseData.keys())
            {
                auto deviceData = licenseData[device];
                foreach(QString id, deviceData)
                {
                    ui->tableWidgetCloudBlockPads->insertRow(0);
                    //license
                    {
                        QTableWidgetItem * it = new QTableWidgetItem();
                        auto flags = it->flags().setFlag(Qt::ItemIsEditable, false);
                        it->setFlags(flags);
                        ui->tableWidgetCloudBlockPads->setItem(0,
                                                               columnsRetreiveCloud::LicenseKey,
                                                               it);
                        it->setText(license);
                    }
                    //device
                    {
                        QTableWidgetItem * it = new QTableWidgetItem();
                        auto flags = it->flags().setFlag(Qt::ItemIsEditable, false);
                        it->setFlags(flags);
                        ui->tableWidgetCloudBlockPads->setItem(0,
                                                               columnsRetreiveCloud::devName,
                                                               it);
                        it->setText(device);
                    }
                    //id
                    {
                        QTableWidgetItem * it = new QTableWidgetItem();
                        auto flags = it->flags().setFlag(Qt::ItemIsEditable, false);
                        it->setFlags(flags);
                        ui->tableWidgetCloudBlockPads->setItem(0,
                                                               columnsRetreiveCloud::id,
                                                               it);
                        it->setText(id);
                    }
                    //download
                    {
                        QWidget* wdg = new QWidget(this);
                        QVBoxLayout layout(wdg);
                        QPushButton * but = new QPushButton(wdg);
                        but->setText("Download");
                        but->setStyleSheet("QPushButton"
                        "{"
                        "background-color:transparent;"
                        "border: none;"
                        "min-width:26;"
                        "min-height:26;"
                        "max-width:26;"
                        "max-height:26;"
                        "qproperty-iconSize: 26px 26px;"
                        "qproperty-text:\"\""
                        "}");
                        but->setIcon(QIcon("://Icons/download.png"));
                        connect(but, &QPushButton::clicked,
                                this, &RetreiveCloudWgt::slotDownloadClicked);
                        layout.addWidget(but);
                        auto horMargin = (ui->tableWidgetCloudBlockPads
                                ->columnWidth(columnsRetreiveCloud::download) - 26)/2;
                        layout.setContentsMargins(horMargin,2,horMargin,2);
                        wdg->setLayout(&layout);
                        ui->tableWidgetCloudBlockPads->setCellWidget(0,
                                                                     columnsRetreiveCloud::download,
                                                                     wdg);
                    }
                }
            }
        }
    }
}

RetreiveCloudWgt::~RetreiveCloudWgt()
{
    delete ui;
}
