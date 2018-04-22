#include "ticker.h"
#include "ui_ticker.h"
#include <QGraphicsSvgItem>
#include <QGraphicsTextItem>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSvgRenderer>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Ticker::Ticker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ticker)
{
    ui->setupUi(this);
    QGraphicsScene* pScene = new QGraphicsScene(this);
    ui->graphicsView->setScene(pScene);
    updateCryptoPricesId = startTimer(60 * 1000);
    updateCryptoPrices();
    //connect signals/slots
    {
        connect(&nam, &QNetworkAccessManager::finished,
                this, &Ticker::slotNetworkReplyFinished);
    }
}

void Ticker::slotNetworkReplyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
        return;
    auto type = reply->property("type").toString();
    if("symbolsUSDPrices" == type)
        updateUSD_PricesFinished(reply);
    if("tcPrices" == type)
        updateTcPricesFinished(reply);
}

void Ticker::updateCryptoPrices()
{
    {
        QNetworkRequest request(QUrl("https://totalcryptos.com/api/symbolsUSDPrices"));
        auto reply = nam.get(request);
        reply->setProperty("type", "symbolsUSDPrices");
    }
    {
        QNetworkRequest request(QUrl("https://totalcryptos.com/api/tcPrices"));
        auto reply = nam.get(request);
        reply->setProperty("type", "tcPrices");
    }
}

void Ticker::updateTcPricesFinished(QNetworkReply *reply)
{
    auto data =reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    auto objectMain = document.object();
    int errorCode = objectMain.value("errCode").toInt();
    if(errorCode == 1)
    {
        auto objectData = objectMain.value("data").toObject();
        tc100Index = objectData.value("tc100").toInt();
        tcw100Index = objectData.value("tcw100").toInt();
    }
}

QString Ticker::roundDouble(QString dbString)
{
    QString res;
    //rounding off
    auto pointIndex = dbString.indexOf('.');
    if(pointIndex != -1 &&
     dbString.size() > pointIndex+1+numberRounding)
    {
        res = dbString.left(pointIndex+1+numberRounding);
    }
    return res;
}

void Ticker::updateUSD_PricesFinished(QNetworkReply *reply)
{
    auto data =reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    auto objectMain = document.object();
    int errorCode = objectMain.value("errCode").toInt();
    if(errorCode == 1)
    {
        auto arrayCryptoData = objectMain.value("data").toArray();
        for(int i=0; i<arrayCryptoData.size(); i++)
        {
            QJsonObject object = arrayCryptoData[i].toObject();
            auto currency = object.value("base_currency").toString();
            if(QFile("://totalCryptosPrices/"+currency +".svg").exists())
            {
                QMap<QString, QString> oneCurrencyData;
                oneCurrencyData["price"] = roundDouble(object.value("price").toString());
                oneCurrencyData["change_24"] = roundDouble(QString::number(object.value("change_perc_24h").toDouble()));
                cryptoPrices[currency] = oneCurrencyData;
            }
        }
    }
}

void Ticker::timerEvent(QTimerEvent *event)
{
    if(tickerId == event->timerId())
    {
        if(!underMouse() && !ui->graphicsView->underMouse())
            RePaint();
    }
    if(updateCryptoPricesId == event->timerId())
    {
        updateCryptoPrices();
    }
}

void Ticker::slotStart(bool bOn)
{
    if(bOn)
        tickerId = startTimer(100);
    else
        killTimer(tickerId);
}

void Ticker::clearScene()
{
    for(int i=0;i<graphItems.size(); i++)
    {
        auto graphItem = graphItems[i];
        ui->graphicsView->scene()->removeItem(graphItem);
        delete graphItem;
    }
    graphItems.clear();
    beginRightBorder = false;
}

int Ticker::leftPosition()
{
    int res = 0;
    if(!graphItems.isEmpty())
    {
        res = graphItems.last()->x();
        if(auto textItem = dynamic_cast<QGraphicsTextItem *>(graphItems.last()))
        {
            if(textItem->property("id").toInt() == specialId::arrowId)
            {
                res = graphItems.last()->x() + 5;
            }
        }
    }
    return res;
}

QGraphicsTextItem * Ticker::addTextItem(QString html,
                                        QFont font,
                                        QList<QGraphicsItem *> &items,
                                        int id)
{
    auto textItem = new QGraphicsTextItem();
    textItem->setHtml(html);
    ui->graphicsView->scene()->addItem(textItem);
    int coeff = 1;
    if(id == specialId::firstId)
        coeff =8;
    if(id != specialId::arrowId)
        textItem->setX(leftPosition()
                      - coeff*shiftElements - QFontMetrics(font).width(html));
    else
        textItem->setX(leftPosition()
                      - 10);
    textItem->setProperty("id", id);
    items.append(textItem);
#if defined(WIN32) || defined(WIN64)
    if(id != specialId::arrowId)
        textItem->moveBy(0,fontSizeDefault - font.pointSize());
#endif
#ifdef __APPLE__
    if(id != specialId::arrowId)
        textItem->moveBy(0,fontSizeDefault + 3- font.pointSize());
    else
        textItem->moveBy(0,3);
#endif
    textItem->setFont(font);
    return textItem;
}

QGraphicsSvgItem * Ticker::addSvgItem(QSvgRenderer *render,
                                      QList<QGraphicsItem *> &items)
{
    auto graphItem = new QGraphicsSvgItem();
    graphItem->setSharedRenderer(render);
    graphItem->setScale(((double)ui->graphicsView->height() - 2)/ render->defaultSize().width());
    ui->graphicsView->scene()->addItem(graphItem);
    graphItem->setX(leftPosition()
                  - shiftElements -ui->graphicsView->height());
    items.append(graphItem);
    return graphItem;
}

void Ticker::RePaint()
{
    ui->graphicsView->setSceneRect(0,0,
               ui->graphicsView->width(),
               ui->graphicsView->height());
    if(graphItems.isEmpty() && tc100Index >= 0 && tcw100Index >= 0)
    {
        //tc100Index and tcw100Index
        {
            auto firstTc100 = addTextItem("$" + QString::number(tc100Index),
                                    QFont("Roboto", 13),
                                    graphItems, specialId::firstId);
            firstTc100->setDefaultTextColor(Qt::darkGreen);
            auto secondTc100 = addTextItem("Price of Tc100: ",
                                    QFont("Roboto", 15, QFont::Bold),
                                    graphItems);
            secondTc100->setDefaultTextColor(Qt::black);
            auto firstTcw100 = addTextItem(QString::number(tcw100Index),
                                    QFont("Roboto", 13),
                                    graphItems, specialId::firstId);
            firstTcw100->setDefaultTextColor(Qt::darkGreen);
            auto secondTcw100 = addTextItem("TCw100: ",
                                    QFont("Roboto", 15, QFont::Bold),
                                    graphItems);
            secondTcw100->setDefaultTextColor(Qt::black);
        }
        QDir dir("://totalCryptosPrices/");
        for(int i=0;i<dir.entryList().size(); i++)
        {
            auto file = dir.entryList()[i];
            auto name = file;
            name.remove(".svg");
            if(cryptoPrices.contains(name))
            {
                int shiftFontSize = 0;
            #ifdef __APPLE__
                shiftFontSize = 4;
            #endif
                QString arrowText;
                QString text;
                bool bPlus = false;
                //fill text and bPlus
                {
                    if(cryptoPrices[name]["change_24"].toDouble() >=0)
                    {
                        arrowText = "<b>&#8593;</b>";
                        text = cryptoPrices[name]["change_24"];
                        bPlus = true;
                    }
                    else
                    {
                        arrowText = "<b>&#8595;</b>";
                        text = cryptoPrices[name]["change_24"];
                        bPlus = false;
                    }
                }
                auto firstItem = addTextItem(text,
                                        QFont("Roboto", 12+shiftFontSize, QFont::Bold),
                                        graphItems, specialId::firstId);
                auto arrowItem = addTextItem(arrowText,
                                             QFont("Roboto", 12+shiftFontSize, QFont::Bold),
                                             graphItems, specialId::arrowId);
                if(bPlus)
                {
                    firstItem->setDefaultTextColor(Qt::green);
                    arrowItem->setDefaultTextColor(Qt::green);
                }
                else
                {
                    firstItem->setDefaultTextColor(Qt::red);
                    arrowItem->setDefaultTextColor(Qt::red);
                }
                addTextItem("$ " + cryptoPrices[name]["price"] +"  ",
                        QFont("Roboto", 13+shiftFontSize),
                        graphItems);
                addTextItem(name,
                            QFont("Roboto", 16+shiftFontSize, QFont::Bold),
                            graphItems);
                QSvgRenderer *render = new QSvgRenderer("://totalCryptosPrices/"+ file, this);
                addSvgItem(render, graphItems);
            }
        }
    }
    auto newGraphItems = graphItems;
    for(int i=0;i<graphItems.size(); i++)
    {
        auto graphItem = graphItems[i];
        if(graphItem->x() > ui->graphicsView->width() && beginRightBorder)
        {
            newGraphItems.removeAt(i);
            ui->graphicsView->scene()->removeItem(graphItem);
            delete graphItem;
            graphItem = nullptr;
            beginRightBorder = false;
        }
        if(graphItem != nullptr
                &&
           graphItem->x() + graphItem->boundingRect().width() > ui->graphicsView->width())
        {
            if(!beginRightBorder)
            {
                if(auto svgItem = dynamic_cast<QGraphicsSvgItem *>(graphItem))
                {
                    addSvgItem(svgItem->renderer(), newGraphItems);
                }
                else if(auto textItem = dynamic_cast<QGraphicsTextItem *>(graphItem))
                {
                    auto newItem = addTextItem(textItem->toPlainText(),
                                               textItem->font(),
                                               newGraphItems,
                                               textItem->property("id").toInt());
                    newItem->setDefaultTextColor(textItem->defaultTextColor());
                }
                beginRightBorder = true;
            }
        }
    }
    graphItems = newGraphItems;
    for(int i=0;i<graphItems.size(); i++)
    {
        auto graphItem = graphItems[i];
        graphItem->setX(graphItem->x() +10);
    }
}

void Ticker::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    clearScene();
}

Ticker::~Ticker()
{
    delete ui;
}
