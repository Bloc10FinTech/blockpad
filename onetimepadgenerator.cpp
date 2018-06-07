#include "onetimepadgenerator.h"
#include "ui_onetimepadgenerator.h"
#include "global.h"
#include <QTime>
#include <QRegExpValidator>
#include <QTextTable>
#include <QWhatsThis>
#include "math.h"
#include <QCryptographicHash>
#include <QPrinter>
#include <QPrintDialog>
#include <QDesktopWidget>

OneTimePadGenerator::OneTimePadGenerator(QWidget *parent) :
    QWidget(parent),
    randomGenerator(QTime::currentTime().msecsSinceStartOfDay()),
    ui(new Ui::OneTimePadGenerator)
{
    ui->setupUi(this);
    Utilities::setAppFamilyFont(ui->labelTitle, 16,QFont::Bold);
    setAttribute(Qt::WA_DeleteOnClose);
    //fill comboBoxCases
    {
        QStringList list = {"Mixed case", "Lower case", "Upper case"};
        ui->comboBoxCases->addItems(list);
    }
    //validators
    {
        auto lineEdits = findChildren<QLineEdit *>();
        foreach(auto edit, lineEdits)
        {
            QRegExp exp;
            if(edit == ui->lineEditKeyLength)
                exp.setPattern("[1-9][0-9]{0,2}");
            else
                exp.setPattern("[1-9][0-9]{0,1}");
            QRegExpValidator * val = new QRegExpValidator(exp,edit);
            edit->setValidator(val);
        }
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
    //signals-slots connects
    {
        connect(ui->pushButtonGenerate, &QPushButton::clicked,
                this, &OneTimePadGenerator::slotGenerateClicked);
        connect(ui->groupBoxComposition, &QGroupBox::clicked,
                this, &OneTimePadGenerator::slotCompositionChecked);
        connect(ui->groupBoxFormat, &QGroupBox::clicked,
                this, &OneTimePadGenerator::slotFormatChecked);
        connect(ui->groupBoxOutput, &QGroupBox::clicked,
                this, &OneTimePadGenerator::slotOutputChecked);
        connect(ui->pushButtonClear, &QPushButton::clicked,
                ui->textEditKeys, &QTextEdit::clear);
        connect(ui->pushButtonSelect, &QPushButton::clicked,
                ui->textEditKeys, &QTextEdit::selectAll);
        connect(ui->pushButtonHelp, &QPushButton::clicked,
                this, &OneTimePadGenerator::slotHelpClicked);
        connect(ui->pushButtonPrint, &QPushButton::clicked,
                this, &OneTimePadGenerator::slotPrintClicked);
    }
    //what's this
    {
        {
            QString whats = "Enter the number of keys you'd like to generate. If you generate more than fit in the results text box, you can use the scroll bar to view the additional lines.";
            ui->lineEditN_Keys->setWhatsThis(whats);
            ui->labelN_Keys->setWhatsThis(whats);
        }
        {
            QString whats = "The key will be broken into groups of the given number of characters by separators. Humans find it easier to read and remember sequences of characters when divided into groups of five or fewer characters.";
            ui->lineEditGroupLength->setWhatsThis(whats);
            ui->labelGroupLength->setWhatsThis(whats);
        }
        {
            QString whats = "Each key will contain this number of characters, not counting separators between groups.";
            ui->lineEditKeyLength->setWhatsThis(whats);
            ui->labelKeyLength->setWhatsThis(whats);
        }
        {
            QString whats = "Lines in the output will be limited to the given length (or contain only one key if the line length is less than required for a single key). If the line length is greater than the width of the results box, you can use the horizontal scroll bar to view the rest of the line. Enter 0 to force one key per line; this is handy when you're preparing a list of keys to be read by a computer program.";
            ui->lineEditLineLength->setWhatsThis(whats);
            ui->labelLineLength->setWhatsThis(whats);
        }
        {
            QString whats = "This set of radio buttons lets you select the character set used in the keys. The alternatives are listed in order of increasing security.";
            ui->labelKeyText->setWhatsThis(whats);
        }
        {
            QString whats = "Keys contain only the decimal digits “0” through “9”. Least secure.";
            ui->radioButtonNumeric->setWhatsThis(whats);
        }
        {
            QString whats = "Keys consist of letters of the alphabet chosen at random. Each character has an equal probability of being one of the 26 letters.";
            ui->radioButtonAlphabetic->setWhatsThis(whats);
        }
        {
            QString whats = "Keys use most of the printable ASCII character set. This option provides the greatest security for a given key length, but most people find keys like this difficult to memorise or even transcribe from a printed pad. If a human is in the loop, it's often better to use a longer alphabetic or word-like key. Most secure.";
            ui->radioButtonGibberish->setWhatsThis(whats);
        }
        {
            QString whats = "The case of letters in keys generated with  Alphabetic and Gibberish key text will be as chosen. Most people find it easier to read lower case letters than all capitals, but for some applications (for example, where keys must be scanned optically by hardware that only recognises capital letters), capitals are required. Selecting “Mixed case” creates keys with a mix of upper- and lower-case letters; such keys are more secure than those with uniform letter case, but do not pass the “telephone test”: you can't read them across a (hopefully secure) voice link without having to indicate whether each letter is or is not a capital.";
            ui->labelLetters->setWhatsThis(whats);
            ui->comboBoxCases->setWhatsThis(whats);
        }
        {
            QString whats = "When the Key length is longer than Group length specification, the key is divided into sequences of the given group length by separator characters. By default, a hyphen, “-”, is used to separate groups. If you check this box, separators will be chosen at random among punctuation marks generally acceptable for applications such as passwords. If you're generating passwords for a computer system, random separators dramatically increase the difficulty of guessing passwords by exhaustive search.";
            ui->checkBoxRandomSeparators->setWhatsThis(whats);
        }
        {
            QString whats = "When this box is checked, at the end of the list of keys, preceded by a line beginning with ten dashes “-”, the 128 bit MD5 signature of each key is given, one per line, with signatures expressed as 32 hexadecimal digits. Key signatures can be used to increase security when keys are used to control access to computer systems or databases. Instead of storing a copy of the keys, the computer stores their signatures. When the user enters a key, its signature is computed with the same MD5 algorithm used to generate it initially, and the key is accepted only if the signature matches. Since discovering a key which will generate a given signature is believed to be computationally prohibitive, even if the list of signatures stored on the computer is compromised, that information will not permit an intruder to deduce a valid key.";
            ui->checkBoxIncludeSignatures->setWhatsThis(whats);
        }
        {
            QString whats = "The seed is the starting value which determines all subsequent values in the pseudorandom sequence used to generate the one-time pad. Given the seed, the pad can be reproduced. The seed is a 31-bit number which can be derived from the date and time at which the one-time pad was requested, or from a user-defined seed value.";
            ui->labelSeed->setWhatsThis(whats);
            ui->radioButtonFromClock->setWhatsThis(whats);
            ui->radioButtonUserDefined->setWhatsThis(whats);
            ui->lineEditUserDefined->setWhatsThis(whats);
        }
    }
}

void OneTimePadGenerator::slotHelpClicked()
{
    QWhatsThis::enterWhatsThisMode();
}

void OneTimePadGenerator::slotGenerateClicked()
{
    ui->textEditKeys->clear();
    int nKeys = ui->lineEditN_Keys->text().toInt();
    int nKeysLine = ui->lineEditLineLength->text().toInt();
    int lengthGroup = ui->lineEditGroupLength->text().toInt();
    int lengthKey = ui->lineEditKeyLength->text().toInt();
    QString data;
    //fill data
    {
        if(!ui->radioButtonAlphabetic->isChecked())
            data.append(numbers);
        if(!ui->radioButtonNumeric->isChecked())
        {
            if(ui->comboBoxCases->currentText()
                    !=
               "Lower case")
                data.append(uppercases);
            if(ui->comboBoxCases->currentText()
                    !=
               "Upper case")
                data.append(lowercases);
        }
        if(ui->radioButtonGibberish->isChecked())
            data.append(gibberishs);
    }
    if(ui->radioButtonFromClock->isChecked())
        randomGenerator.seed(QTime::currentTime().msecsSinceStartOfDay());
    else
        randomGenerator.seed(ui->lineEditUserDefined->text().toInt());
    QStringList keys;
    //fill keys
    {
        int iLineKey=0;
        for(int iKey=0; iKey<nKeys; iKey++)
        {
            QString key;
            for (int iChar=0; iChar<lengthKey; iChar++)
            {
                if(lengthGroup != 0)
                {
                    QChar separator = defaultSeparator;
                    if(ui->checkBoxRandomSeparators->isChecked())
                    {
                        int idx = randomGenerator.bounded(gibberishs.size());
                        separator = gibberishs[idx];
                    }
                    if(iChar!=0 && iChar%lengthGroup == 0)
                        key.append(separator);
                }
                int index = randomGenerator.bounded(data.size());
                key.append(data[index]);
            }
            QString keyText = QString::number(iKey+1)
                                + ") " + key;
            keys.append(keyText);
        }
    }
    QTextTableFormat tf;
    tf.setBorder(0);
    tf.setCellPadding(0);
    tf.setCellSpacing(10);
    QTextCursor cursor = ui->textEditKeys->textCursor();
    if (!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
    auto table = cursor.insertTable(ceil((double)nKeys/(double)nKeysLine),
                                    nKeysLine,
                                    tf);

    for(int iKey=0; iKey<nKeys; iKey++)
    {
        table->cellAt(iKey/nKeysLine, iKey%nKeysLine).
                firstCursorPosition().insertText(keys[iKey]);
    }
    if(ui->checkBoxIncludeSignatures->isChecked())
    {
        ui->textEditKeys->append(QString());
        ui->textEditKeys->append(QString());
        ui->textEditKeys->append(QString("------------------------------------  MD5 Signatures  ------------------------------------"));
        ui->textEditKeys->append(QString());
        for(int iKey=0; iKey<nKeys; iKey++)
        {
            ui->textEditKeys->append("         " + QString::number(iKey+1)
                                     + ") "
                                     +
                                     QCryptographicHash::hash(keys[iKey].toLatin1(),
                                                              QCryptographicHash::Md5).toHex());
        }
    }
}

void OneTimePadGenerator::slotPrintClicked()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print One-Time pads"));
    if (dialog.exec() == QDialog::Accepted && printer.isValid())
    {
        ui->textEditKeys->document()->print(&printer);
    }
}

void OneTimePadGenerator::slotFormatChecked(bool checked)
{
    auto wgts = ui->groupBoxFormat->findChildren<QWidget *>();
    foreach(auto wgt, wgts)
    {
        if(checked)
            wgt->show();
        else
            wgt->hide();
    }
}

void OneTimePadGenerator::slotOutputChecked(bool checked)
{
    auto wgts = ui->groupBoxOutput->findChildren<QWidget *>();
    foreach(auto wgt, wgts)
    {
        if(checked)
            wgt->show();
        else
            wgt->hide();
    }
}

void OneTimePadGenerator::slotCompositionChecked(bool checked)
{
    auto wgts = ui->groupBoxComposition->findChildren<QWidget *>();
    foreach(auto wgt, wgts)
    {
        if(checked)
            wgt->show();
        else
            wgt->hide();
    }
}

OneTimePadGenerator::~OneTimePadGenerator()
{
    delete ui;
}
