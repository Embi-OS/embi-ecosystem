#ifndef QVARIANTCOMPARE_H
#define QVARIANTCOMPARE_H

#include <QDefs>

class QAbstractItemModel;
namespace QQsfpm {

int ordering(const QVariant &left, const QVariant &right);
int compare(const QVariant &left, const QVariant &right, Qt::CaseSensitivity cs = Qt::CaseSensitive, bool isLocaleAware=false);
bool lessThan(const QVariant &left, const QVariant &right, Qt::CaseSensitivity cs = Qt::CaseSensitive, bool isLocaleAware=false);

int indexOf(const QAbstractItemModel* model, int role, const QVariant& val);

} // namespace QQsfpm

#endif // QVARIANTCOMPARE_H
