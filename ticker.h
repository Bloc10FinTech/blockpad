#ifndef TICKER_H
#define TICKER_H

#include <QWidget>
#include <QList>
#include <QNetworkAccessManager>
#include <QMap>
#include <QPointer>


class QGraphicsItem;
class QGraphicsTextItem;
class QGraphicsSvgItem;
class QSvgRenderer;

namespace Ui {
class Ticker;
}

class Ticker : public QWidget
{
    Q_OBJECT

public:
    explicit Ticker(QWidget *parent = 0);
    ~Ticker();
protected:
    void timerEvent(QTimerEvent *event);
    void resizeEvent(QResizeEvent *event);
private:
    enum specialId {noId, firstId, arrowId};
    Ui::Ticker *ui;
    //test graphics view
    QList<QGraphicsItem *> graphItems;
    int tickerId {0};
    bool beginRightBorder {false};
    void RePaint();
    void clearScene();
    int shiftElements {7};
    int updateCryptoPricesId {0};
    void updateCryptoPrices();
    QNetworkAccessManager nam;
    QMap<QString, QMap<QString, QString>> cryptoPrices;
    QGraphicsTextItem *addTextItem(QString html,
                                   QFont font,
                                   QList<QGraphicsItem *> & items,
                                   int id = specialId::noId);
    QGraphicsSvgItem * addSvgItem(QSvgRenderer *render, QList<QGraphicsItem *> &items);
    int leftPosition();
    QString roundDouble(QString dbString);
    const int numberRounding {2};
    const int fontSizeDefault {16};
public slots:
    void slotStart(bool bOn);
private slots:
    void slotUpdateCryptoPricesFinished(QNetworkReply *reply);
};

#endif // TICKER_H
