#ifndef LOCALEMODEL_H
#define LOCALEMODEL_H

#include <QModels>
#include <QDefs>
#include <QLocale>

class LocaleObject : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_CONSTANT_REF_PROPERTY(QLocale, locale, Locale, {})
    Q_CONSTANT_REF_PROPERTY(QString, name, Name, "")
    Q_CONSTANT_REF_PROPERTY(QString, territory, Territory, "")
    Q_CONSTANT_REF_PROPERTY(QString, language, Language, "")
    Q_CONSTANT_REF_PROPERTY(QString, lang, Lang, "")
    Q_CONSTANT_REF_PROPERTY(QString, display, Display, "")

public:
    explicit LocaleObject(const QLocale& locale, QObject *parent = nullptr);
};

class LocaleModel : public QObjectListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit LocaleModel(QObject* parent = nullptr);

    QModelIndex indexForTerritory(const QString &territory) const;

    static QList<LocaleObject*> generateModel();

public slots:
    void refresh();

signals:
    void ready();
};

#endif // LOCALEMODEL_H
