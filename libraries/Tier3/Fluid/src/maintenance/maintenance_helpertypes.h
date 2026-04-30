#ifndef MAINTENANCE_HELPERTYPES_H
#define MAINTENANCE_HELPERTYPES_H

#include <QDefs>
#include <QUtils>

class MaintenanceHelper : public QObject,
                          public QQmlSingleton<MaintenanceHelper>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool canOpenUrl READ canOpenUrl CONSTANT FINAL)

protected:
    friend QQmlSingleton<MaintenanceHelper>;
    explicit MaintenanceHelper(QObject *parent = nullptr);

public:
    static bool canOpenUrl();

public slots:
    void clearLogs();
    void exportLogs();
    void clearCache();
    void clearSettings();
    void clearAll();

    void openLocalFilePath(const QString& path);
};

#endif // MAINTENANCE_HELPERTYPES_H
