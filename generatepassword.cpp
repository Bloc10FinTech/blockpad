#include "generatepassword.h"
#include "ui_generatepassword.h"
#include <QIntValidator>
#include <QDesktopWidget>
#include <QStyle>

GeneratePassword::GeneratePassword(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneratePassword)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QPixmap pixmap("://Icons/Password.png");
    pixmap = pixmap.scaled(ui->labelIconPassword->width(),
                           ui->labelIconPassword->height());
    ui->labelIconPassword->setPixmap(pixmap);
    Utilities::setAppFamilyFont(ui->labelTitlePassword, 16,QFont::Bold);
    //add validators
    {
        QIntValidator * val = new QIntValidator(ui->lineEditPasswLength);
        val->setBottom(1);
        val->setTop(32000);
        ui->lineEditPasswLength->setValidator(val);
    }
#ifdef __linux__
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            QSize(this->size()),
            qApp->desktop()->availableGeometry()
        )
    );
#endif
    //connect signals/slots
    {
        connect(ui->pushButtonGenerate, &QPushButton::clicked,
                this, &GeneratePassword::slotGeneratePassword);
    }
}

void GeneratePassword::slotGeneratePassword()
{
    QString allowables;
    if(ui->checkBoxSymbols->isChecked())
        allowables.append(symbols);
    if(!ui->checkBoxAmbiguous->isChecked())
        allowables.append(ambiguouses);
    if(ui->checkBoxLowercase->isChecked())
        allowables.append(lowercases);
    if(ui->checkBoxNumbers->isChecked())
        allowables.append(numbers);
    if(!ui->checkBoxSimilar->isChecked())
        allowables.append(similars);

    QString randomString;
    auto randomStringLength = ui->lineEditPasswLength->text().toInt();
    for(int i=0; i<randomStringLength; ++i)
    {
        int index = qrand() % allowables.length();
        QChar nextChar = allowables.at(index);
        randomString.append(nextChar);
    }
    ui->lineEditNewPassword->setText(randomString);
}

GeneratePassword::~GeneratePassword()
{
    delete ui;
}
