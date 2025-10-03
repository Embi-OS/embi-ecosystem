#ifndef SORTERSSORTERCONTAINER_H
#define SORTERSSORTERCONTAINER_H

#include <QList>
#include <QQmlListProperty>
#include <qqml.h>
#include <QPointer>

class QQmlSortFilterProxyModel;

namespace QQsfpm {

class Sorter;

class SorterContainer {
public:
    virtual ~SorterContainer() = default;

    const QList<Sorter*>& sorters() const;
    void appendSorter(Sorter* sorter);
    void removeSorter(Sorter* sorter);
    void clearSorters();

    QQmlListProperty<QQsfpm::Sorter> sortersListProperty();

protected:
    QList<Sorter*> m_sorters;
    QList<Sorter*> m_sortedSorters;

private:
    virtual void onSorterAppended(Sorter* sorter) = 0;
    virtual void onSorterRemoved(Sorter* sorter) = 0;
    virtual void onSortersCleared() = 0;

#if QT_VERSION_MAJOR < 6
    using q_size_type = int;
#else
    using q_size_type = qsizetype;
#endif
    static void append_sorter(QQmlListProperty<QQsfpm::Sorter>* list, Sorter* sorter);
    static q_size_type count_sorter(QQmlListProperty<QQsfpm::Sorter>* list);
    static Sorter* at_sorter(QQmlListProperty<QQsfpm::Sorter>* list, q_size_type index);
    static void clear_sorters(QQmlListProperty<QQsfpm::Sorter>* list);
};

class SorterContainerAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject* container READ container WRITE setContainer NOTIFY containerChanged FINAL)

public:
    SorterContainerAttached(QObject* object);
    ~SorterContainerAttached();

    QObject* container() const;
    void setContainer(QObject* object);

    static SorterContainerAttached* qmlAttachedProperties(QObject* object);

Q_SIGNALS:
    void containerChanged();

private:
    QPointer<QObject> m_container = nullptr;
    Sorter* m_sorter = nullptr;
};

}

#define SorterContainer_iid "fr.grecko.SortFilterProxyModel.SorterContainer"
Q_DECLARE_INTERFACE(QQsfpm::SorterContainer, SorterContainer_iid)

#endif // SORTERSSORTERCONTAINER_H
