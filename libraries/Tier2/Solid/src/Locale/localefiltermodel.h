#ifndef LOCALEFILTERMODEL_H
#define LOCALEFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <QDefs>

class LocaleFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, search, Search, "")
    Q_WRITABLE_REF_PROPERTY(QStringList, filters, Filters, {})

public:
    explicit LocaleFilterModel(QObject* parent=nullptr);

    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

    Q_INVOKABLE int indexForTerritory(const QString &territory) const;

private:
    int m_localeRole=-1;
    int m_nameRole=-1;
    int m_languageRole=-1;
    int m_territoryRole=-1;
};

#endif // LOCALEFILTERMODEL_H
