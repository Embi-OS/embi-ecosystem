#include "qtextlistmodel.h"

QTextListModel::QTextListModel(QObject *parent) :
    QTextListModel("", parent)
{

}

QTextListModel::QTextListModel(const QString& text, QObject *parent) :
    QStringListModel(text.split('\n'), parent),
    m_text(text)
{
    connect(this, &QTextListModel::textChanged, this, &QTextListModel::onTextChanged, Qt::UniqueConnection);
}

void QTextListModel::onTextChanged(const QString& text)
{
    const QStringList strings = text.split('\n');

    setStringList(std::move(strings));
}