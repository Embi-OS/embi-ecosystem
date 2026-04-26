#ifndef TIMEZONEFILTERMODEL_H
#define TIMEZONEFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <QDefs>

class TimezoneFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, search, Search, "")

public:
    explicit TimezoneFilterModel(QObject* parent=nullptr);

    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

    Q_INVOKABLE int indexForTimezone(const QString &timezone) const;

private:
    int m_nameRole=-1;
    int m_territoryRole=-1;
};

#endif // TIMEZONEFILTERMODEL_H
