#ifndef QTEXTLISTMODEL_H
#define QTEXTLISTMODEL_H

#include <QDefs>
#include <QStringListModel>

class QTextListModel: public QStringListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(TextListModel)

    Q_WRITABLE_REF_PROPERTY(QString, text, Text, "")

public:
    explicit QTextListModel(QObject * parent = nullptr);
    explicit QTextListModel(const QString& text, QObject *parent = nullptr);

protected slots:
    void onTextChanged(const QString& text);
};

#endif // QTEXTLISTMODEL_H
