#ifndef SQLPRIMARYCOLUMNPREPARATOR_H
#define SQLPRIMARYCOLUMNPREPARATOR_H

#include "sqlcolumnpreparator.h"
class SqlPrimaryColumnPreparator : public SqlColumnPreparator
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(int, startValue, StartValue, 0)

public:
    explicit SqlPrimaryColumnPreparator(QObject* parent = nullptr);
    explicit SqlPrimaryColumnPreparator(const QString& name, int startValue, QObject* parent = nullptr);
};

#endif // SQLPRIMARYCOLUMNPREPARATOR_H
