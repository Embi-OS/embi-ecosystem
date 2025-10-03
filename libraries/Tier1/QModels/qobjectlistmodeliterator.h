#ifndef QOBJECTLISTMODELITERATOR_H
#define QOBJECTLISTMODELITERATOR_H

#include "qobjectlistmodel.h"
#include <QListIterator>

template <typename T>
class QObjectListModelIterator
{
    friend QObjectListModel;
    QObjectListModelIterator(const QObjectListModel* listModel):
        m_listModel(listModel),
        m_iterator(listModel->cbegin())
    {}

public:
    inline QObjectListModelIterator &operator=(QObjectListModel* listModel) {
        m_listModel = listModel;
        m_iterator = listModel->constBegin();
        return *this;
    }

    inline void toFront() { m_iterator = m_listModel->constBegin(); }
    inline void toBack() { m_iterator = m_listModel->constEnd(); }
    inline bool hasNext() const { return m_iterator != m_listModel->constEnd(); }
    inline T* next() { return *m_iterator++; }
    inline T* peekNext() const { return *m_iterator; }
    inline bool hasPrevious() const { return m_iterator != m_listModel->constBegin(); }
    inline T* previous() { return *--m_iterator; }
    inline T* peekPrevious() const { const_iterator p = m_iterator; return *--p; }

    inline bool findNext(const T* t) {
        while (m_iterator != m_listModel->constEnd()) {
            if (*m_iterator++ == t)
                return true;
        }
        return false;
    }
    inline bool findPrevious(const T* t) {
        while (m_iterator != m_listModel->constBegin()) {
            if (*(--m_iterator) == t)
                return true;
        }
        return false;
    }

    class const_iterator {
        QObjectListModel::const_iterator i;
    public:
        using pointer = const QObjectListModel::const_iterator *;
        using reference = const QObjectListModel::const_iterator &;

        inline constexpr const_iterator() = default;
#ifndef QT_STRICT_QLIST_ITERATORS
        inline constexpr explicit const_iterator(const T* *n) : i(n) {}
#endif
        inline constexpr const_iterator(QObjectListModel::const_iterator o): i(o) {}
        inline T* operator*() const { return qobject_cast<T*>(*i); }
        inline T* operator->() const { return qobject_cast<T*>(i); }
        inline T* operator[](qsizetype j) const { return qobject_cast<T*>(i[j]); }
        inline constexpr bool operator==(const_iterator o) const { return i == o.i; }
        inline constexpr bool operator!=(const_iterator o) const { return i != o.i; }
        inline constexpr bool operator<(const_iterator other) const { return i < other.i; }
        inline constexpr bool operator<=(const_iterator other) const { return i <= other.i; }
        inline constexpr bool operator>(const_iterator other) const { return i > other.i; }
        inline constexpr bool operator>=(const_iterator other) const { return i >= other.i; }
        inline constexpr bool operator==(pointer p) const { return &i == p; }
        inline constexpr bool operator!=(pointer p) const { return &i != p; }
        inline const_iterator &operator++() { ++i; return *this; }
        inline const_iterator operator++(int) { auto copy = *this; ++*this; return copy; }
        inline const_iterator &operator--() { --i; return *this; }
        inline const_iterator operator--(int) { auto copy = *this; --*this; return copy; }
        inline qsizetype operator-(const_iterator j) const { return i - j.i; }
        inline const_iterator &operator+=(qsizetype j) { i += j; return *this; }
        inline const_iterator &operator-=(qsizetype j) { i -= j; return *this; }
        inline const_iterator operator+(qsizetype j) const { return const_iterator(i + j); }
        inline const_iterator operator-(qsizetype j) const { return const_iterator(i - j); }
        friend inline const_iterator operator+(qsizetype j, const_iterator k) { return k + j; }
    };

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    const_iterator begin() const { return m_listModel->cbegin(); }
    const_iterator end() const { return m_listModel->cend(); }
    const_iterator cbegin() const { return m_listModel->cbegin(); }
    const_iterator cend() const { return m_listModel->cend(); }
    const_iterator constBegin() const { return m_listModel->constBegin(); }
    const_iterator constEnd() const { return m_listModel->constEnd(); }

    const_reverse_iterator rbegin() const { return m_listModel->crbegin(); }
    const_reverse_iterator rend() const { return m_listModel->crend(); }
    const_reverse_iterator crbegin() const { return m_listModel->crbegin(); }
    const_reverse_iterator crend() const { return m_listModel->crend(); }

private:
    const QObjectListModel* m_listModel;
    const_iterator m_iterator;

};

#endif
