#ifndef STRINGSORTER_H
#define STRINGSORTER_H

#include "rolesorter.h"
#include <QCollator>

namespace QQsfpm {

class StringSorter : public QQsfpm::RoleSorter
{
    Q_OBJECT

    Q_WRITABLE_VAR_PROPERTY(Qt::CaseSensitivity, caseSensitivity, CaseSensitivity, Qt::CaseSensitive)
    Q_WRITABLE_VAR_PROPERTY(bool, ignorePunctation, IgnorePunctation, false)
    Q_WRITABLE_REF_PROPERTY(QLocale, locale, Locale, {})
    Q_WRITABLE_VAR_PROPERTY(bool, numericMode, NumericMode, false)

public:
    explicit StringSorter(QObject *parent = nullptr);

protected slots:
    void updateCaseSensitivity();
    void updateIgnorePunctation();
    void updateLocale();
    void updateNumericMode();

protected:
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const override;

private:
    QCollator m_collator;
};

}

#endif // STRINGSORTER_H
