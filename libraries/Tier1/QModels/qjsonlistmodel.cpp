#include "qjsonlistmodel.h"
#include "qmodels_log.h"

QJsonListModel::QJsonListModel(QObject *parent) :
    QJsonListModel("", "", parent)
{

}

QJsonListModel::QJsonListModel(const QString& baseName, QObject *parent) :
    QJsonListModel("", baseName, parent)
{

}

QJsonListModel::QJsonListModel(const QString& jsonPath, const QString& baseName, QObject *parent) :
    QVariantListModel(baseName, parent),
    m_jsonPath(jsonPath),
    m_jsonFormat(QJsonListModel::Indented)
{
    connect(this, &QJsonListModel::jsonPathChanged, this, &QVariantListModel::queueSelect);
    connect(this, &QJsonListModel::jsonFormatChanged, this, &QVariantListModel::queueSubmit);
}

QJsonListModel::~QJsonListModel()
{
    if(m_submitPolicy<QVariantListModelPolicies::Manual && m_isDirty) {
        submit();
        waitForSubmit();
    }
}

QVariantList QJsonListModel::selectSource(bool* result)
{
    if(!m_jsonPath.isEmpty() && !m_baseName.isEmpty())
        return fromFile(result);

    if(m_source.canConvert<QString>())
        return fromJson(m_source.toString().toUtf8(), result);

    return QVariantListModel::selectSource(result);
}

bool QJsonListModel::doSubmit()
{
    bool result = submitJson();
    emitSubmitDone(result);

    return result;
}

bool QJsonListModel::submitJson()
{
    m_json = QJsonDocument::fromVariant(getStorage()).toJson(QJsonDocument::JsonFormat(m_jsonFormat));
    emit this->jsonChanged(m_json);

    if(m_jsonPath.isEmpty() || m_baseName.isEmpty())
        return false;

    return toFile(m_json.toUtf8());
}

bool QJsonListModel::toFile(const QString& json) const
{
    if(!QFileInfo::exists(m_jsonPath))
    {
        QMODELSLOG_WARNING()<<"Path:"<<m_jsonPath<<"does not exists";
        return false;
    }

    const QString filePath = QDir(m_jsonPath).filePath(m_baseName+".json");
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QMODELSLOG_WARNING()<<"Cannot open file:"<<filePath;
        return false;
    }

    bool ret = file.write(json.toUtf8());
    file.close();

    return ret;
}

QVariantList QJsonListModel::fromFile(bool* result)
{
    if(!QFileInfo::exists(m_jsonPath))
    {
        QMODELSLOG_WARNING()<<"Path:"<<m_jsonPath<<"does not exists";
        if(result)
            *result=false;
        return QVariantList();
    }

    const QString filePath = QDir(m_jsonPath).filePath(m_baseName+".json");
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        QMODELSLOG_WARNING()<<"Error opening file:"<<file.errorString();
        if(result)
            *result=false;
        return QVariantList();
    }

    const QByteArray json = file.readAll();
    file.close();

    return fromJson(json, result);
}

QVariantList QJsonListModel::fromJson(const QString& json, bool* result)
{
    if(json.isEmpty())
    {
        if(result)
            *result=true;
        return QVariantList();
    }

    QJsonParseError parseError;
    const QJsonDocument& jdoc = QJsonDocument::fromJson(json.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        QMODELSLOG_WARNING()<<"Error loading json:"<<parseError.errorString();
        if(result)
            *result=true;
        return QVariantList();
    }

    if (jdoc.isNull())
    {
        QMODELSLOG_WARNING()<<"cannot load json";
        if(result)
            *result=true;
        return QVariantList();
    }

    if(!jdoc.isArray())
    {
        QMODELSLOG_WARNING()<<"cannot load non array json";
        if(result)
            *result=true;
        return QVariantList();
    }

    if(m_json!=json)
    {
        m_json = json;
        emit this->jsonChanged(m_json);
    }

    if(result)
        *result=true;

    return jdoc.array().toVariantList();
}
