#ifndef REGEXPROLE_H
#define REGEXPROLE_H

#include "proxyrole.h"
#include <QRegularExpression>

namespace QQsfpm {

class RegExpRole : public QQsfpm::ProxyRole
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QString, roleName, RoleName, {})
    Q_WRITABLE_VAR_PROPERTY(int, role, Role, -1)
    Q_WRITABLE_REF_PROPERTY(QString, pattern, Pattern, {})
    Q_WRITABLE_VAR_PROPERTY(Qt::CaseSensitivity, caseSensitivity, CaseSensitivity, Qt::CaseInsensitive)

public:
    explicit RegExpRole(QObject *parent = nullptr);

    QStringList names() override;
    void updateRoles(const QQmlSortFilterProxyModel& proxyModel) override;

protected slots:
    void updatePattern();
    void updateCaseSensitivity();

private:
    QRegularExpression m_regExp;
    QVariant data(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel &proxyModel, const QString &name) override;
};

}

#endif // REGEXPROLE_H
