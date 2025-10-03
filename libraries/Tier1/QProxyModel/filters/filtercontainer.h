#ifndef FILTERCONTAINER_H
#define FILTERCONTAINER_H

#include <QList>
#include <QQmlListProperty>
#include <qqml.h>
#include <QPointer>

class QQmlSortFilterProxyModel;

namespace QQsfpm {

class Filter;

class FilterContainer {
public:
    virtual ~FilterContainer() = default;

    const QList<Filter*>& filters() const;
    void appendFilter(Filter* filter);
    void removeFilter(Filter* filter);
    void clearFilters();

    QQmlListProperty<QQsfpm::Filter> filtersListProperty();

protected:
    QList<Filter*> m_filters;

private:
    virtual void onFilterAppended(Filter* filter) = 0;
    virtual void onFilterRemoved(Filter* filter) = 0;
    virtual void onFiltersCleared() = 0;

#if QT_VERSION_MAJOR < 6
    using q_size_type = int;
#else
    using q_size_type = qsizetype;
#endif
    static void append_filter(QQmlListProperty<QQsfpm::Filter>* list, Filter* filter);
    static q_size_type count_filter(QQmlListProperty<QQsfpm::Filter>* list);
    static Filter* at_filter(QQmlListProperty<QQsfpm::Filter>* list, q_size_type index);
    static void clear_filters(QQmlListProperty<QQsfpm::Filter>* list);
};

class FilterContainerAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject* container READ container WRITE setContainer NOTIFY containerChanged FINAL)

public:
    FilterContainerAttached(QObject* object);
    ~FilterContainerAttached();

    QObject* container() const;
    void setContainer(QObject* object);

    static FilterContainerAttached* qmlAttachedProperties(QObject* object);

Q_SIGNALS:
    void containerChanged();

private:
    QPointer<QObject> m_container = nullptr;
    Filter* m_filter = nullptr;
};

}

#define FilterContainer_iid "fr.grecko.SortFilterProxyModel.FilterContainer"
Q_DECLARE_INTERFACE(QQsfpm::FilterContainer, FilterContainer_iid)

#endif // FILTERCONTAINER_H
