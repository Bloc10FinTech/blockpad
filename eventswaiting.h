#ifndef EVENTSWAITING_H
#define EVENTSWAITING_H
#include <QObject>
#include <QMetaEnum>
#include <QDebug>
#include <QEvent>
#include <QSettings>

class EventsWaiting: public QObject
{
    Q_OBJECT

public:
    explicit EventsWaiting(QObject *parent = 0);
    ~EventsWaiting();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void timerEvent(QTimerEvent *event);
private:
    int idTimer {0};
    int iTimeLockScreen {30};
    void restartTimer();
    QSettings settings;
public slots:
    void slotSetTimeLockScreen(int time);
signals:
    void StartLockScreen();
};

#endif // EVENTSWAITING_H
