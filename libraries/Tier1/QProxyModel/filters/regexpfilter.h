#ifndef REGEXPFILTER_H
#define REGEXPFILTER_H

#include <QRegularExpression>
#include "rolefilter.h"

namespace QQsfpm {

class RegExpFilter : public QQsfpm::RoleFilter {
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QString, pattern, Pattern, {})
    Q_WRITABLE_VAR_PROPERTY(Qt::CaseSensitivity, caseSensitivity, CaseSensitivity, Qt::CaseInsensitive)

public:
    explicit RegExpFilter(QObject *parent = nullptr);

protected:
    bool filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;

protected slots:
    void updatePattern();
    void updateCaseSensitivity();

private:
    QRegularExpression m_regExp;
};

}

#endif // REGEXPFILTER_H
