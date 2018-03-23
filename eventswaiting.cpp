#include "eventswaiting.h"

EventsWaiting::EventsWaiting(QObject *parent): QObject(parent)
{
    if(settings.value("LockScreen_Time").type() != QVariant::Invalid)
        iTimeLockScreen = settings.value("LockScreen_Time").toInt();
    idTimer = startTimer(iTimeLockScreen * 60 * 1000);
}

EventsWaiting::~EventsWaiting()
{

}

bool EventsWaiting::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseMove
       ||
       event->type() == QEvent::KeyPress
       ||
       event->type() == QEvent::MouseButtonPress)
    {
        restartTimer();
    }
    return QObject::eventFilter(obj, event);
}

void EventsWaiting::slotSetTimeLockScreen(int time)
{
    qDebug() << "EventsWaiting::slotSetTimeLockScreen";
    iTimeLockScreen = time;
    restartTimer();
}

void EventsWaiting::restartTimer()
{
    if(idTimer)
        killTimer(idTimer);
    idTimer = startTimer(iTimeLockScreen * 60 * 1000);
    //Test
    //idTimer = startTimer(10 * 1000);
}

void EventsWaiting::timerEvent(QTimerEvent *event)
{
    restartTimer();
    emit StartLockScreen();
}
