#ifndef QOBJECTMAPPER_H
#define QOBJECTMAPPER_H

#include "qvariantmapper.h"
#include <QAbstractItemModel>

class QObjectMapper: public QVariantMapper
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ObjectMapper)

    Q_WRITABLE_PTR_PROPERTY(QObject, object, Object, nullptr)

public:
    explicit QObjectMapper(QObject *parent = nullptr);
    explicit QObjectMapper(const QString& baseName, QObject *parent = nullptr);
    ~QObjectMapper() override;

protected:
    bool doSelect() final override;
    bool doSubmit(const QStringList& dirtyKeys) final override;

private slots:
    void onObjectAboutToChange(QObject* oldObject, QObject* newObject);
    void onObjectChanged(QObject* object);

    void onObjectPropertyChanged();

private:
    QVariantMap selectObject(bool* result=nullptr);
    QVariantMap fromObject(bool* result=nullptr);

    bool submitObject(const QStringList& dirtyKeys);
    bool toObject(const QVariantMap& map) const;
};

#endif // QOBJECTMAPPER_H
