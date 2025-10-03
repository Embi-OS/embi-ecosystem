#include "qjsonmapper.h"
#include "qutils_log.h"

QJsonMapper::QJsonMapper(QObject *parent) :
    QJsonMapper("", "", parent)
{

}

QJsonMapper::QJsonMapper(const QString& baseName, QObject *parent) :
    QJsonMapper(baseName, "", parent)
{

}

QJsonMapper::QJsonMapper(const QString& jsonPath, const QString& baseName, QObject *parent) :
    QVariantMapper(baseName, parent, &QJsonMapper::staticMetaObject),
    m_jsonPath(jsonPath),
    m_jsonFormat(QJsonMapper::Indented),
    m_json(QString())
{
    connect(this, &QJsonMapper::jsonPathChanged, this, &QVariantMapper::queueSelect);
    connect(this, &QJsonMapper::jsonFormatChanged, this, &QVariantMapper::queueSubmit);
}

QJsonMapper::~QJsonMapper()
{
    if(m_submitPolicy<QVariantMapperPolicies::Manual && m_isDirty) {
        submit();
        waitForSubmit();
    }
}

const QString& QJsonMapper::getJson() const
{
    return m_json;
}

void QJsonMapper::setJson(const QString& json)
{
    if(m_json==json)
        return;
    m_json = json;
    queueSelect();
    emit this->jsonChanged(m_json);
}

bool QJsonMapper::doSelect()
{
    bool result = true;
    const QVariantMap storage = selectJson(&result);
    setStorage(storage);
    emitSelectDone(result);

    return true;
}

bool QJsonMapper::doSubmit(const QStringList& dirtyKeys)
{
    bool result = submitJson(dirtyKeys);
    emitSubmitDone(result);

    return result;
}

QVariantMap QJsonMapper::selectJson(bool* result)
{
    QVariantMap json;
    if(m_jsonPath.isEmpty() || m_baseName.isEmpty())
        json = fromJson(m_json.toUtf8(), result);
    else
        json = fromFile(result);

    QVariantMap storage;
    const QStringList mapKeys = json.keys();
    for(const QString& key: mapKeys)
    {
        if(getMappedKeys().isEmpty() || getMappedKeys().contains(key))
            storage.insert(key, json.value(key));
    }

    return storage;
}

QVariantMap QJsonMapper::fromFile(bool* result)
{
    if(!QFileInfo::exists(m_jsonPath))
    {
        QUTILSLOG_WARNING()<<"Path:"<<m_jsonPath<<"does not exists";
        if(result)
            *result=false;
        return QVariantMap();
    }

    const QString filePath = QDir(m_jsonPath).filePath(m_baseName+".json");
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        QUTILSLOG_WARNING()<<"Error opening file:"<<file.errorString();
        if(result)
            *result=false;
        return QVariantMap();
    }

    const QByteArray json = file.readAll();
    file.close();

    return fromJson(json, result);
}

QVariantMap QJsonMapper::fromJson(const QString& json, bool* result)
{
    if(json.isEmpty())
    {
        if(result)
            *result=false;
        return QVariantMap();
    }

    QJsonParseError parseError;
    const QJsonDocument& jdoc = QJsonDocument::fromJson(json.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        QUTILSLOG_WARNING()<<"Error loading json:"<<parseError.errorString();
        if(result)
            *result=false;
        return QVariantMap();
    }

    if (jdoc.isNull())
    {
        QUTILSLOG_WARNING()<<"cannot load json";
        if(result)
            *result=false;
        return QVariantMap();
    }

    if(!jdoc.isObject())
    {
        QUTILSLOG_WARNING()<<"cannot load non object json";
        if(result)
            *result=false;
        return QVariantMap();
    }

    if(m_json!=json)
    {
        m_json = json;
        emit this->jsonChanged(m_json);
    }

    if(result)
        *result=true;

    return jdoc.object().toVariantMap();
}

bool QJsonMapper::submitJson(const QStringList& dirtyKeys)
{
    Q_UNUSED(dirtyKeys)

    m_json = QJsonDocument::fromVariant(getStorage()).toJson(QJsonDocument::JsonFormat(m_jsonFormat));
    emit this->jsonChanged(m_json);

    if(m_jsonPath.isEmpty() || m_baseName.isEmpty())
        return false;

    return toFile(m_json.toUtf8());
}

bool QJsonMapper::toFile(const QString& json)
{
    if(!QFileInfo::exists(m_jsonPath))
    {
        QUTILSLOG_WARNING()<<"Path:"<<m_jsonPath<<"does not exists";
        return false;
    }

    const QString filePath = QDir(m_jsonPath).filePath(m_baseName+".json");
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QUTILSLOG_WARNING()<<"Cannot open file:"<<filePath;
        return false;
    }

    bool ret = file.write(json.toUtf8());
    file.close();

    return ret;
}
