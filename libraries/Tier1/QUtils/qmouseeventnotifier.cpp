#include "qmouseeventnotifier.h"
#include <QMouseEvent>

QMouseEventNotifier::QMouseEventNotifier(QObject *parent) :
    QObject(parent)
{

}

void QMouseEventNotifier::classBegin()
{

}
void QMouseEventNotifier::componentComplete()
{
    qApp->installEventFilter(this);
}

bool QMouseEventNotifier::eventFilter(QObject *obj, QEvent *event)
{
    if(!m_enabled || !event || obj!=m_window) {
        return QObject::eventFilter(obj, event);
    }

    if(event->type() == QEvent::MouseButtonPress ||
       event->type() == QEvent::MouseButtonRelease ||
       event->type() == QEvent::MouseButtonDblClick ||
       event->type() == QEvent::MouseMove ||
       event->type() == QEvent::TouchBegin ||
       event->type() == QEvent::TouchUpdate ||
       event->type() == QEvent::TouchEnd ||
       event->type() == QEvent::TouchCancel)
    {
        QPointF scenePosition;
        if(QPointerEvent *pointerEvent = dynamic_cast<QPointerEvent *>(event))
        {
            const QList<QEventPoint> points = pointerEvent->points();
            for(const QEventPoint& eventPoint: points) {
                scenePosition += eventPoint.scenePosition();
            }
            scenePosition/=(float)points.size();
        }
        setScenePosition(scenePosition);

        if(event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick || event->type() == QEvent::TouchBegin)
        {
            setPressed(true);
            emit this->mousePressed();
        }
        else if(event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::TouchEnd || event->type() == QEvent::TouchCancel)
        {
            setPressed(false);
            emit this->mouseReleased();
        }
        else if(event->type() == QEvent::MouseMove || event->type() == QEvent::TouchUpdate)
        {
            emit this->mouseMoved();
        }

        emit this->mouseEvent();
    }

    // standard event processing
    return QObject::eventFilter(obj, event);
}
