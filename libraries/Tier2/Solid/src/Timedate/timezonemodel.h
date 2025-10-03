#ifndef TIMEZONEMODEL_H
#define TIMEZONEMODEL_H

#include <QModels>
#include <QDefs>
#include <QTimeZone>

class TimezoneObject : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString identifier READ getIdentifier CONSTANT)
    Q_PROPERTY(QTimeZone timezone READ getTimezone CONSTANT)
    Q_PROPERTY(QString name READ getName CONSTANT)
    Q_PROPERTY(QString territory READ getTerritory CONSTANT)

public:
    explicit TimezoneObject(const QByteArray& id, QObject *parent = nullptr);

    const QString& getIdentifier() const;
    const QTimeZone& getTimezone() const;
    const QString& getName() const;
    const QString& getTerritory() const;

private:
    QString m_identifier;
    mutable QTimeZone m_timezone;
    mutable QString m_name;
    mutable QString m_territory;
};

class TimezoneModel : public QObjectListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit TimezoneModel(QObject* parent = nullptr);

    QModelIndex indexForTimezone(const QString &timezone) const;

    static QList<TimezoneObject*> generateModel();

public slots:
    void refresh();

signals:
    void ready();
};

#endif // TIMEZONEMODEL_H
