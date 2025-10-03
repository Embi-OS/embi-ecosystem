#ifndef QEMPTYMODEL_H
#define QEMPTYMODEL_H

#include <QDefs>
#include <QAbstractItemModel>
#include <QTransposeProxyModel>

class QEmptyModel : public QAbstractItemModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(EmptyModel)

public:
    explicit QEmptyModel(QObject *parent = 0) : QAbstractItemModel(parent) {}
    QModelIndex index(int, int, const QModelIndex &) const override final { return QModelIndex(); }
    QModelIndex parent(const QModelIndex &) const override final { return QModelIndex(); }
    int rowCount(const QModelIndex &) const override final { return 0; }
    int columnCount(const QModelIndex &) const override final { return 0; }
    bool hasChildren(const QModelIndex &) const override final { return false; }
    QVariant data(const QModelIndex &, int) const override final { return QVariant(); }
};

struct QTransposeProxyModelForeign {
    Q_GADGET
    QML_FOREIGN(QTransposeProxyModel)
    QML_NAMED_ELEMENT(TransposeProxyModel)
};

#endif // QEMPTYMODEL_H
