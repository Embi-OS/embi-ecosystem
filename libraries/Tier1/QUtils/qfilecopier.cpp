#include "qfilecopier.h"
#include "vendors/qtfilecopier/qtfilecopier.h"

QFileCopier::QFileCopier(QObject* parent) :
    QObject(parent)
{
    connect(this, &QFileCopier::finished, this, [this](){
        if(m_autoDelete)
            this->deleteLater();
    });
}

bool QFileCopier::copy(const QString& source, const QString& destination, bool overwrite)
{
    if(m_worker || m_running)
        return false;

    m_worker = new QtFileCopier(this);

    return true;
}
bool QFileCopier::copy(const QStringList& sources, const QString& destination, bool overwrite)
{
    if(m_worker || m_running)
        return false;

    return true;
}

bool QFileCopier::move(const QString& sourceFile, const QString& destinationPath, bool overwrite)
{
    if(m_worker || m_running)
        return false;

    return true;
}
bool QFileCopier::move(const QStringList& sourceFile, const QString& destinationPath, bool overwrite)
{
    if(m_worker || m_running)
        return false;

    return true;
}

bool QFileCopier::remove(const QString& sourceFile)
{
    if(m_worker || m_running)
        return false;

    return true;
}
bool QFileCopier::remove(const QStringList& sourceFile)
{
    if(m_worker || m_running)
        return false;

    return true;
}

bool QFileCopier::cancel()
{
    if(!m_worker)
        return false;

    m_worker->cancelAll();
    return true;
}
