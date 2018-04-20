#include "blockpad.h"
#include <QApplication>
#include <QFontDatabase>
#include <QDir>
#include "global.h"
#include "mainwidget.h"
#include "eventswaiting.h"
#include <QLibraryInfo>
#include <QStandardPaths>
#include <QDebug>

QString fileInit;
#if defined(WIN32) || defined(WIN64)
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <iostream>
#include <fstream>
#endif

int main(int argc, char *argv[])
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime().addSecs(24)));
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Bloc10");
    QCoreApplication::setOrganizationDomain("www.bloc10.com");
    QCoreApplication::setApplicationName("BlockPad");
//    //aws test
//    {
//        Aws::SDKOptions options;
//        options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;
//        Aws::InitAPI(options);
//        Aws::String accessKey = "AKIAIFY7ILAN7LFVX7HA";
//        Aws::String sequreKey = "bwqDslr0tv2ZVtBKLq60wQILFaeLF4ep6s3hmb10";
//        Aws::Auth::AWSCredentials credentionals(accessKey,sequreKey);
//        Aws::S3::S3Client s3_client(credentionals);
//        auto outcome = s3_client.ListBuckets();

//        if (outcome.IsSuccess())
//        {
//            std::cout << "Your Amazon S3 buckets:" << std::endl;

//            Aws::Vector<Aws::S3::Model::Bucket> bucket_list =
//                outcome.GetResult().GetBuckets();

//            for (auto const &bucket : bucket_list)
//            {
//                std::cout << "  * " << bucket.GetName() << std::endl;
//            }
//        }
//        else
//        {
//            std::cout << "ListBuckets error: "
//                << outcome.GetError().GetExceptionName() << " - "
//                << outcome.GetError().GetMessage() << std::endl;
//        }
//        {
//            Aws::S3::Model::PutObjectRequest object_request;
//            object_request.WithBucket("blockpadcloud").WithKey("Test/dvajdual_blockpad.bloc").WithServerSideEncryption(Aws::S3::Model::ServerSideEncryption::AES256);

//            Aws::String file_name = "C:\\Users\\user\\AppData\\Local\\Bloc10\\BlockPad\\blockpads\\dvajdual_blockpad.bloc";

//            // Binary files must also have the std::ios_base::bin flag or'ed in
//            auto input_data = Aws::MakeShared<Aws::FStream>("PutObjectInputStream",
//                file_name.c_str(), std::ios_base::in | std::ios_base::binary);

//            object_request.SetBody(input_data);

//            auto put_object_outcome = s3_client.PutObject(object_request);

//            if (put_object_outcome.IsSuccess())
//            {
//                std::cout << "Done!" << std::endl;
//            }
//            else
//            {
//                std::cout << "PutObject error: " <<
//                    put_object_outcome.GetError().GetExceptionName() << " " <<
//                    put_object_outcome.GetError().GetMessage() << std::endl;
//            }
//        }
//        {
//            Aws::S3::Model::GetObjectRequest object_request;
//            object_request.WithBucket("blockpadcloud").WithKey("Test/dvajdual_blockpad.bloc");

//            auto get_object_outcome = s3_client.GetObject(object_request);

//            if (get_object_outcome.IsSuccess())
//            {
//                Aws::OFStream local_file;
//                local_file.open("C:\\Users\\user\\Desktop\\download.bloc", std::ios::out | std::ios::binary);
//                local_file << get_object_outcome.GetResult().GetBody().rdbuf();
//                std::cout << "Done!" << std::endl;
//            }
//            else
//            {
//                std::cout << "GetObject error: " <<
//                    get_object_outcome.GetError().GetExceptionName() << " " <<
//                    get_object_outcome.GetError().GetMessage() << std::endl;
//            }
//        }
//        Aws::ShutdownAPI(options);
//    }
    if(argc > 1)
        fileInit = QString(argv[1]);
    //create application files directories
    {
        QDir().mkpath(Utilities::filesDirectory());
        //create defPathBlockpads
        if(!QDir(Utilities::filesDirectory()
                 + "/"+ defPathBlockpads).exists())
            QDir(Utilities::filesDirectory()).mkdir(defPathBlockpads);
    }
    //fonts
    {
        QDir dir("://Fonts");
        foreach(auto file, dir.entryList())
        {
            QFontDatabase::addApplicationFont("://Fonts/"+ file);
        }
        QString font;
        int fontSize = 0;
        //fill font and fontSize
        {
            QSettings settings;
            font = settings.value("Font").toString();
            if(font == "")
                font = "Roboto";
            if(fontSize == 0)
            {
            #if defined(WIN32) || defined(WIN64)
                fontSize = 12;
            #endif
            #ifdef __APPLE__
                fontSize = 15;
            #endif
            }
        }
        a.setFont (QFont (font, fontSize, appFontWeight));
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
