#ifndef DATABASE_HELPERTYPES_H
#define DATABASE_HELPERTYPES_H

#include <QDefs>
#include <QUtils>

class DatabaseHelper : public QObject,
                       public QQmlSingleton<DatabaseHelper>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

protected:
    friend QQmlSingleton<DatabaseHelper>;
    explicit DatabaseHelper(QObject *parent = nullptr);

public slots:
};

#endif // DATABASE_HELPERTYPES_H
