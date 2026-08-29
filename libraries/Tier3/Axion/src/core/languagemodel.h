#ifndef LANGUAGEMODEL_H
#define LANGUAGEMODEL_H

#include <QModels>
#include <QDefs>

class LanguageObject : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_CONSTANT_REF_PROPERTY(QString, name, "")
    Q_PROPERTY(QString language READ language CONSTANT FINAL)
    Q_PROPERTY(QString display READ display CONSTANT FINAL)

public:
    explicit LanguageObject(const QString& name, QObject *parent = nullptr);

    QString language() const;
    QString display() const;
};

class LanguageModel : public QObjectListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit LanguageModel(QObject* parent = nullptr);

    static QList<LanguageObject*> generateModel();

public slots:
    void refresh();

signals:
    void ready();
};

#endif // LANGUAGEMODEL_H
