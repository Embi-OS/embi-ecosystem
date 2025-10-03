#ifndef ABSTRACTHAPPYFIELD_H
#define ABSTRACTHAPPYFIELD_H

#include <QDefs>
#include <Sql>

#include "happyhttpparameters.h"

class HappyCrudRouter;
class HappyServer;
class AbstractHappyField: public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(HappyField)
    QML_UNCREATABLE("Abstract")

    Q_WRITABLE_REF_PROPERTY(QString, name, Name, "")
    Q_WRITABLE_REF_PROPERTY(QVariant, defaultValue, DefaultValue, {})
    Q_WRITABLE_REF_PROPERTY(QString, connection, Connection, SqlDefaultConnection)

public:
    explicit AbstractHappyField(QObject* parent=nullptr);

    int nameIndex() const;
    const QByteArray& nameJson() const;
    const QByteArray& nameCbor() const;

    virtual bool init(HappyCrudRouter* crudRouter, HappyServer* happyServer);

    virtual void preRead(const HappyHttpParameters& parameter, const QSqlRecord& baseRec);
    virtual void preWrite(const HappyHttpParameters& parameters);

    Q_INVOKABLE virtual QVariant formatValue(const QVariant& value, bool* ok=nullptr) const;
    Q_INVOKABLE virtual QVariant formatRead(const QVariant& value, bool* ok=nullptr) const;
    Q_INVOKABLE virtual QVariant formatWrite(const QVariant& value, bool* ok=nullptr) const;

    Q_INVOKABLE virtual QVariant read(const QSqlRecord& record, bool* ok=nullptr) const = 0;
    Q_INVOKABLE virtual QVariant write(const QVariantMap& value, bool creation, bool* ok=nullptr) const = 0;

protected:
    HappyCrudRouter* m_crudRouter;
    HappyServer* m_happyServer;

    int m_nameIndex;
    QByteArray m_nameJson;
    QByteArray m_nameCbor;

    QMutex m_readMutex;
    QMutex m_writeMutex;
};

#endif // ABSTRACTSTREAMER_H
