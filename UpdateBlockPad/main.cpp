/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <QtConcurrent>
#include <QTimer>
#include <QProcess>
#include <QString>
#include <QFile>
#include <QDir>
#include <atomic>
#include <functional>

using namespace QtConcurrent;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    if(argc < 2)
    {
        QMessageBox::critical(nullptr, "BlockPad update error",
                              "number of arguments -1");
        return 1;
    }
    QString pathFiles = QString::fromUtf8(argv[1]);
    if(argc < 3)
    {
        QMessageBox::critical(nullptr, "BlockPad update error",
                              "number of arguments -2");
        return 1;
    }
    QString pathApp = QString::fromUtf8(argv[2]);
    std::atomic_bool bSuccess;
    bSuccess.store(true);
    QString strUnSuccess;

    // Create a progress dialog.
    QProgressDialog dialog;
    dialog.setLabelText(QString("Wait while updating"));

    QTimer timer;
    timer.start(1000);
    // Create a QFutureWatcher and connect signals and slots.
    QFutureWatcher<void> futureWatcher;
    QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &app, [&]()
    {
        QProcess proc;
        if(bSuccess.load())
        {
            QString cmd = "open " + pathApp + "/BlockPad.app";
            proc.startDetached(cmd);
        }
        else
        {
            QMessageBox::critical(nullptr, "BlockPad Update Error",
                                  strUnSuccess);
        }
        dialog.close();
    });
    QObject::connect(&timer, &QTimer::timeout,
                     &app, [&]()
    {
       dialog.setValue(dialog.value() + (100 - dialog.value())/10);
    });
    //function of copiing files
    auto update = [&]()
    {
        //To unzip - delete new version in filePath
        {
            QDir dir(pathFiles + "/BlockPad.app");
            dir.removeRecursively();
        }
        //unzip
        {
            //test
            QProcess pros;
            QString strProc = "unzip BlockPad.zip";
            pros.setWorkingDirectory(pathFiles);
            pros.start(strProc);
            pros.waitForFinished(10*60*1000);
            bSuccess = !pros.exitCode();
            if(!bSuccess.load())
            {
                strUnSuccess = "Can not unzip new version";
                return;
            }
            QFile(pathFiles + "/BlockPad.zip").remove();
        }
        //clean
        {
            QDir dir(pathApp + "/BlockPad.app");
            bSuccess = dir.removeRecursively();
            if(!bSuccess.load())
            {
                strUnSuccess = "Can not remove old application";
                return;
            }
        }
        //remove new version to appPath
        {
            auto _pathFiles = pathFiles;
            auto _pathApp = pathApp;
            QProcess pros;
            pros.start("sh -c \"cp -R " + _pathFiles.replace(" ", "\\ ")
                       + "/BlockPad.app "
                       + _pathApp.replace(" ", "\\ ") + "/BlockPad.app\"");
            pros.waitForFinished(10*60*1000);
            bSuccess = !pros.exitCode();
            if(!bSuccess.load())
            {
                strUnSuccess = "Can not remove new version to old place";
                return;
            }
        }
        //delete new version in filePath
        {
            QDir dir(pathFiles + "/BlockPad.app");
            dir.removeRecursively();
        }
    };

    // Start the computation.
    futureWatcher.setFuture(QtConcurrent::run(update));

    // Display the dialog and start the event loop.
    dialog.exec();

    futureWatcher.waitForFinished();
}
