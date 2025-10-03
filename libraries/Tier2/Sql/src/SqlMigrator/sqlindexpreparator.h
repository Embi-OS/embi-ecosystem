#ifndef SQLINDEXPREPARATOR_H
#define SQLINDEXPREPARATOR_H

#include <QDefs>
#include "../sql_helpertypes.h"
#include "../SqlCore/sqlcmd.h"

class SqlIndexPreparator : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QStringList, fields, Fields, {})

public:
    explicit SqlIndexPreparator(QObject* parent = nullptr);
    explicit SqlIndexPreparator(const QStringList& fields, QObject* parent = nullptr);

    bool isValid() const;
};

#endif // SQLINDEXPREPARATOR_H
