#ifndef SYSTEM_HELPERTYPES_H
#define SYSTEM_HELPERTYPES_H

#include <QDefs>
#include <QUtils>

class SystemHelper : public QObject,
                     public QQmlSingleton<SystemHelper>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

protected:
    friend QQmlSingleton<SystemHelper>;
    explicit SystemHelper(QObject *parent = nullptr);

public slots:
    void showTimedateCtl();
    void clearLogs();
    void exportLogs();
    void clearCache();
    void clearSettings();
    void clearAll();
};

#endif // SYSTEM_HELPERTYPES_H
