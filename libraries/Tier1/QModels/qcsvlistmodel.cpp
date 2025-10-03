#include "qcsvlistmodel.h"
#include "qmodels_log.h"

#include <QDir>

QCsvListModel::QCsvListModel(QObject *parent) :
    QVariantListModel(parent)
{

}

QCsvListModel::QCsvListModel(const QString& baseName, QObject *parent) :
    QCsvListModel("", baseName, parent)
{

}

QCsvListModel::QCsvListModel(const QString& csvPath, const QString& baseName, QObject *parent) :
    QVariantListModel(csvPath, parent),
    m_csvPath(csvPath),
    m_csvSeparator(';')
{
    connect(this, &QCsvListModel::csvPathChanged, this, &QVariantListModel::queueSelect);
    connect(this, &QCsvListModel::csvSeparatorChanged, this, &QVariantListModel::queueSubmit);
}

QCsvListModel::~QCsvListModel()
{
    if(m_submitPolicy<QVariantListModelPolicies::Manual && m_isDirty) {
        submit();
        waitForSubmit();
    }
}

QVariantList QCsvListModel::selectSource(bool* result)
{
    if(!m_csvPath.isEmpty() && !m_baseName.isEmpty())
        return fromFile(result);

    if(m_source.canConvert<QString>())
        return fromCsv(m_source.toString().toUtf8(), result);

    return QVariantListModel::selectSource(result);
}

bool QCsvListModel::doSubmit()
{
    bool result = submitCsv();
    emitSubmitDone(result);

    return result;
}

QVariantList QCsvListModel::fromFile(bool* result)
{
    if(!QFileInfo::exists(m_csvPath))
    {
        QMODELSLOG_WARNING()<<"Path:"<<m_csvPath<<"does not exists";
        if(result)
            *result=false;
        return QVariantList();
    }

    const QString filePath = QDir(m_csvPath).filePath(m_baseName+".csv");
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        QMODELSLOG_WARNING()<<"Error opening file:"<<file.errorString();
        if(result)
            *result=false;
        return QVariantList();
    }

    const QByteArray csv = file.readAll();
    file.close();

    return fromCsv(csv, result);
}

QVariantList QCsvListModel::fromCsv(const QString& csv, bool* result)
{
    if(csv.isEmpty())
    {
        if(result)
            *result=true;
        return QVariantList();
    }

    QTextStream fileStream(csv.toUtf8());
    QVariantList storage;

    do
    {
        QString line = fileStream.readLine();
        const QStringList values = line.split(m_csvSeparator);
        QVariantMap map;
        int pos=0;

        for(const QString& value: values)
        {
            map.insert(QString("column_%1").arg(pos), value);
            pos++;
        }

        storage.append(map);
    }
    while(!fileStream.atEnd());

    if(m_csv!=csv)
    {
        m_csv = csv;
        emit this->csvChanged(m_csv);
    }

    if(result)
        *result=true;

    return storage;
}

bool QCsvListModel::submitCsv()
{
    QStringList csv;
    for(const QVariant& variant: getStorage())
    {
        const QVariantMap map = variant.toMap();
        QStringList csvLine;
        csvLine.reserve(map.size());
        for(auto [key, value]: map.asKeyValueRange())
            csvLine.append(value.toString());

        csv.append(csvLine.join(m_csvSeparator));
    }

    m_csv = csv.join('\n').toUtf8();
    emit this->csvChanged(m_csv);

    if(m_csvPath.isEmpty() || m_baseName.isEmpty())
        return false;

    return toFile(m_csv.toUtf8());
}

bool QCsvListModel::toFile(const QString& csv) const
{
    if(!QFileInfo::exists(m_csvPath))
    {
        QMODELSLOG_WARNING()<<"Path:"<<m_csvPath<<"does not exists";
        return false;
    }

    const QString filePath = QDir(m_csvPath).filePath(m_baseName+".csv");
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QMODELSLOG_WARNING()<<"Cannot open file:"<<filePath;
        return false;
    }

    bool ret = file.write(csv.toUtf8());
    file.close();

    return ret;
}
