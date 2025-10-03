#include "qnetworkping.h"
#include "qutils_log.h"

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, timeRegExp, (R"(time=([\d.]+)\s*ms)"))

QNetworkPing::QNetworkPing(QObject *parent):
    QObject(parent)
{

}
void QNetworkPing::ping()
{
    QNetworkPing::ping(m_address, this, [this](double result) {
        emit this->finished(result);
    }, m_timeout, m_interval);
}

double QNetworkPing::ping(const QString &address, int timeout, int interval)
{
#if QT_CONFIG(process)

    QEventLoop loop;
    QElapsedTimer elapsed;
    elapsed.start();

    double ping=-1;
    QNetworkPing::ping(address, &loop, [&](double result){
        ping = result;
        if(loop.isRunning())
            loop.exit(result);
    }, timeout, interval);

    loop.exec();

    if(ping>=0)
        QUTILSLOG_DEBUG()<<"Ping"<<address<<"ended with ping of"<<ping<<"ms after"<<elapsed.nsecsElapsed()/1000000.0<<"ms";
    else
        QUTILSLOG_WARNING()<<"Ping"<<address<<"timed out after"<<elapsed.nsecsElapsed()/1000000.0<<"ms";

    return ping;

#else

    return 0;

#endif
}

void QNetworkPing::ping(const QString &address, const QObject* context, std::function<void(double)> callback, int timeout, int interval)
{
#if QT_CONFIG(process)

    const QString program = "ping";
#ifdef Q_OS_WIN
    const QStringList parameters = {"-n", "1", "-W", QString::number(::qMin(interval, timeout)), address};
#else
    const QStringList parameters = {"-c", "1", "-W", QString::number(::qMax(1.0, ::qMin(interval, timeout)/1000.0)), address};
#endif

    QElapsedTimer* timer = new QElapsedTimer;
    QProcess *process = new QProcess();

    QObject::connect(process, &QProcess::finished, context, [timer, process, context, address, timeout, interval, callback](int exitCode, QProcess::ExitStatus exitStatus) {
        const QString processOutput = process->readAllStandardOutput();
        const QString processError = process->readAllStandardOutput();
        const double elapsed = timer->nsecsElapsed()/1000000.0;
        double ping = -1;

        const QRegularExpressionMatch match = timeRegExp->match(processOutput);
        if (exitCode==0 && match.hasMatch())
            ping = match.captured(1).toDouble();

        process->deleteLater();
        delete timer;

        if(exitCode!=0 && timeout>elapsed)
        {
            QUTILSLOG_INFO()<<"Try to ping"<<address<<"- remaining time:"<<qRound((timeout-elapsed)/1000.0)<<"sec";
            QNetworkPing::ping(address, context, callback, timeout-elapsed, interval);
            return;
        }

        if(callback)
            callback(ping);
    });

    process->setWorkingDirectory("/");
    process->setProgram(program);
    process->setArguments(parameters);
    process->start();
    timer->start();

#else

    if(callback)
        callback(0);

#endif
}
