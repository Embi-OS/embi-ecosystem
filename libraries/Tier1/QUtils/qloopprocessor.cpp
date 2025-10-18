#include "qloopprocessor.h"
#include "qutils_log.h"

QLoopProcessor::QLoopProcessor(QObject *parent) :
    QObject(parent)
{

}

bool QLoopProcessor::run()
{
    if(m_running)
        return false;

    setRunning(true);
    setIndex(0);
    setRelease(0);

    if(!start())
    {
        setRunning(false);
        return false;
    }

    iterate();

    return true;
}

bool QLoopProcessor::start()
{
    m_elapsedSinceLastRelease.restart();
    m_elapsedTotal.restart();

    if(m_startCallback)
        return m_startCallback();

    return true;
}

void QLoopProcessor::iterate()
{
    if(m_mustRelease)
    {
        m_mustRelease = false;
        m_elapsedSinceLastRelease.restart();
    }

    if(!m_iterateCallback)
    {
        end();
        return;
    }

    bool mustContinue = m_iterateCallback();

    setIndex(m_index+1);

    if(!mustContinue)
    {
        end();
        return;
    }

    double elapsed = m_elapsedSinceLastRelease.nsecsElapsed()/1000000.0;
    if(m_asynchronous && elapsed>m_timeout)
    {
        release();
    }
    else
    {
        iterate();
    }
}

void QLoopProcessor::release()
{
    m_mustRelease = true;
    setRelease(m_release+1);

    if(m_releaseCallback)
        m_releaseCallback();

    QMetaObject::invokeMethod(this, &QLoopProcessor::iterate, Qt::QueuedConnection);
}

void QLoopProcessor::end()
{
    if(m_endCallback)
        m_endCallback();

    setRunning(false);

    QUTILSLOG_DEBUG()<<"QLoopProcessor took"<<m_elapsedTotal.nsecsElapsed()/1000000.0<<"ms for"<<m_index<<"elements";

    if(m_autoDelete)
        deleteLater();
}

QLoopProcessor* QLoopProcessor::onStart(std::function<bool()> callback)
{
    if(!callback)
    {
        QUTILSLOG_WARNING()<<"onStart: Fail to connect empty lambda";
        return this;
    }

    if(m_startCallback)
    {
        QUTILSLOG_WARNING()<<"onStart: can have only one callback";
        return this;
    }

    m_startCallback = callback;

    return this;
}

QLoopProcessor* QLoopProcessor::onIterate(std::function<bool()> callback)
{
    if(!callback)
    {
        QUTILSLOG_WARNING()<<"onIterate: Fail to connect empty lambda";
        return this;
    }

    if(m_iterateCallback)
    {
        QUTILSLOG_WARNING()<<"onIterate: can have only one callback";
        return this;
    }

    m_iterateCallback = callback;

    return this;
}

QLoopProcessor* QLoopProcessor::onRelease(std::function<void()> callback)
{
    if(!callback)
    {
        QUTILSLOG_WARNING()<<"onRelease: Fail to connect empty lambda";
        return this;
    }

    if(m_releaseCallback)
    {
        QUTILSLOG_WARNING()<<"onRelease: can have only one callback";
        return this;
    }

    m_releaseCallback = callback;

    return this;
}

QLoopProcessor* QLoopProcessor::onEnd(std::function<void()> callback)
{
    if(!callback)
    {
        QUTILSLOG_WARNING()<<"onEnd: Fail to connect empty lambda";
        return this;
    }

    if(m_endCallback)
    {
        QUTILSLOG_WARNING()<<"onEnd: can have only one callback";
        return this;
    }

    m_endCallback = callback;

    return this;
}

