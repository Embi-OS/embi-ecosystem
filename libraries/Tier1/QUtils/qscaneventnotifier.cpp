#include "qscaneventnotifier.h"
#include "qutils_log.h"
#include <QKeyEvent>
#include <QDebug>

QScanEventNotifier::QScanEventNotifier(QObject *parent) :
    QObject(parent)
{

}

void QScanEventNotifier::classBegin()
{

}
void QScanEventNotifier::componentComplete()
{
    qApp->installEventFilter(this);
}

void QScanEventNotifier::clearLastValue()
{
    resetLastValue();
}

bool QScanEventNotifier::eventFilter(QObject *obj, QEvent *event)
{
    if(!m_enabled || !event)
        return QObject::eventFilter(obj, event);

    if(event->spontaneous() && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
        const int elapsed = keyEvent->timestamp()-m_timestamp;

        if(elapsed>m_elapsedThreshold)
        {
            m_scan.clear();
        }

        if(!keyEvent->isAutoRepeat())
        {
            if(keyEvent->text() != "\r" && keyEvent->text() != "\n")
            {
                m_scan.append(keyEvent->text());
            }
            else if(m_scan!="")
            {
                QUTILSLOG_TRACE()<<m_scan;
                emit this->newScan(m_scan);
                setLastValue(m_scan);
                m_scan.clear();
            }
        }
        m_timestamp = keyEvent->timestamp();
    }

    // standard event processing
    return QObject::eventFilter(obj, event);
}
