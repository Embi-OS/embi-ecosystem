#ifndef PATTERNROLE_H
#define PATTERNROLE_H

#include "singlerole.h"
#include <QRegularExpression>

namespace QQsfpm {

class PatternRole : public QQsfpm::SingleRole
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QString, pattern, Pattern, {})

public:
    explicit PatternRole(QObject *parent = nullptr);

    void updateRoles(const QQmlSortFilterProxyModel& proxyModel) override;

protected slots:
    void updatePattern();

private:
    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;

    QString m_string;
    QStringList m_captures;
    QStringList m_roleNames;
    QList<int> m_roles;
    QList<int> m_precisions;
};

}

#endif // PATTERNROLE_H
