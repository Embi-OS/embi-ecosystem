#ifndef SYSTEMCTLUNITCONTROLLER_H
#define SYSTEMCTLUNITCONTROLLER_H

#include <QDefs>
#include <QProcess>

class SystemCtlUnitController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, unit, Unit, "")
    Q_READONLY_VAR_PROPERTY(bool, unitExists, UnitExists, false)
    Q_READONLY_VAR_PROPERTY(bool, processing, Processing, false)

    Q_READONLY_VAR_PROPERTY(bool, loaded, Loaded, false)
    Q_READONLY_VAR_PROPERTY(bool, active, Active, false)
    Q_READONLY_REF_PROPERTY(QString, status, Status, "")

public:
    explicit SystemCtlUnitController(QObject *parent = nullptr);

    Q_INVOKABLE static QStringList units(const QString& pattern=QString());

public slots:
    void refreshStatus();

    void enable();
    void disable();
    void start();
    void stop();
    void restart();

signals:
    void enableFinished(bool success, const QString &message);
    void disableFinished(bool success, const QString &message);

private:
    void checkUnit();
    void checkStatus();
    void parseStatusOutput(const QString &output);
};

#endif // SYSTEMCTLUNITCONTROLLER_H
