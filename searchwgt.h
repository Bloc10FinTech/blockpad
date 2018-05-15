#ifndef SEARCHWGT_H
#define SEARCHWGT_H

#include <QWidget>

namespace Ui {
class SearchWgt;
}

class SearchWgt : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWgt(QWidget *parent = 0);
    ~SearchWgt();
protected:
    bool event(QEvent *e);
private:
    Ui::SearchWgt *ui;
private slots:
    //transparency
    void slotSliderTransparencyChanged(int value);
    void slotTransparencyGroupClicked(bool checked);
    void slotOnLosingFocusClicked();
    void slotAllwaysClicked();

    //Find
    void slotFindAllCurrent();
signals:
    void sigFindAllCurrentFile(QString strSearch, bool bRegExp,
                               bool bMatchWholeWord, bool bMatchCase);
    void sigFindAllAllFiles(QString strSearch, bool bRegExp,
                            bool bMatchWholeWord, bool bMatchCase);
    void sigFindNext(QString strSearch);
    void sigFindPrev(QString strSearch);
};

#endif // SEARCHWGT_H
