#ifndef QSPAGINATION_H
#define QSPAGINATION_H

#include <QDefs>

class QSPagination : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SyncablePagination)

    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)

    Q_WRITABLE_VAR_PROPERTY(int, perPage, PerPage, 0)
    Q_WRITABLE_VAR_PROPERTY(int, page, Page, 0)

    Q_READONLY_VAR_PROPERTY(int, pageCount, PageCount, 0)
    Q_READONLY_VAR_PROPERTY(int, count, Count, 0)
    Q_READONLY_VAR_PROPERTY(int, total, Total, 0)
    Q_READONLY_REF_PROPERTY(QString, previous, Previous, {})
    Q_READONLY_REF_PROPERTY(QString, next, Next, {})

public:
    explicit QSPagination(QObject *parent = nullptr);

signals:
    void invalidated();

protected slots:
    void invalidate();
};

#endif // QSPAGINATION_H
