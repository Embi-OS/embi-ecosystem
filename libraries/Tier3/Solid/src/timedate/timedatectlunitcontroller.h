#ifndef TIMEDATECTLUNITCONTROLLER_H
#define TIMEDATECTLUNITCONTROLLER_H

#include <QDefs>

class TimedateCtlUnitController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_READONLY_REF_PROPERTY(QString, status, Status, "")
    Q_READONLY_REF_PROPERTY(QString, timesyncStatus, TimesyncStatus, "")
    Q_READONLY_REF_PROPERTY(QString, timesyncProperties, TimesyncProperties, "")

    Q_PROPERTY(bool available READ available CONSTANT FINAL)

public:
    explicit TimedateCtlUnitController(QObject *parent = nullptr);

    static bool available();

    Q_INVOKABLE static QString timedateCtl(const QStringList& arguments=QStringList());

public slots:
    void refreshStatus();
    void refreshTimesyncStatus();
    void refreshTimesyncProperties();
};

#endif // TIMEDATECTLUNITCONTROLLER_H
