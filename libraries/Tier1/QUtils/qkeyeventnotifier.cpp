#include "qkeyeventnotifier.h"
#include <QMouseEvent>

QKeyEventNotifier::QKeyEventNotifier(QObject *parent) :
    QObject(parent)
{

}

void QKeyEventNotifier::classBegin()
{

}
void QKeyEventNotifier::componentComplete()
{
    qApp->installEventFilter(this);
}

bool QKeyEventNotifier::eventFilter(QObject *obj, QEvent *event)
{
    if(!m_enabled || !event || obj!=m_window)
        return QObject::eventFilter(obj, event);

    if(event->type() == QEvent::KeyPress ||
       event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);

        if(event->type() == QEvent::KeyPress)
        {
            emit this->keyPressed(keyEvent->key());
        }
        else if(event->type() == QEvent::KeyRelease)
        {
            emit this->keyReleased(keyEvent->key());
        }

        emit this->keyEvent(keyEvent->key());
    }

    // standard event processing
    return QObject::eventFilter(obj, event);
}
