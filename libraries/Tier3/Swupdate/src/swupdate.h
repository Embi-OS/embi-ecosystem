#ifndef SWUPDATE_H
#define SWUPDATE_H

#include <QDefs>
#include <QSocketNotifier>
#include "qsingleton.h"

Q_ENUM_CLASS(SwupdateRecoveryStatuses, SwupdateRecoveryStatus,
             Idle,
             Start,
             Run,
             Success,
             Failure,
             Download,
             Done,
             Subprocess,
             Progress)

Q_ENUM_CLASS(SwupdateSourceTypes, SwupdateSourceType,
             Unknown,
             WebServer,
             Suricatta,
             Downloader,
             Local,
             ChunckDownloader)

class SwupdateProgressMessage
{
    Q_GADGET
    QML_VALUE_TYPE(swupdateProgressMessage)
    QML_STRUCTURED_VALUE

    Q_PROPERTY(QString apiVersion MEMBER apiVersion FINAL)
    Q_PROPERTY(SwupdateRecoveryStatuses::Enum status MEMBER status FINAL)
    Q_PROPERTY(SwupdateSourceTypes::Enum source MEMBER source FINAL)
    Q_PROPERTY(QString info MEMBER info FINAL)
    Q_PROPERTY(int downloadPercent MEMBER downloadPercent FINAL)
    Q_PROPERTY(long long downloadBytes MEMBER downloadBytes FINAL)
    Q_PROPERTY(int nbSteps MEMBER nbSteps FINAL)
    Q_PROPERTY(int currentStep MEMBER currentStep FINAL)
    Q_PROPERTY(int currentStepPercent MEMBER currentStepPercent FINAL)
    Q_PROPERTY(QString currrentImage MEMBER currrentImage FINAL)
    Q_PROPERTY(QString handlerName MEMBER handlerName FINAL)

public:
    SwupdateProgressMessage();
    ~SwupdateProgressMessage() = default;
    Q_INVOKABLE SwupdateProgressMessage(const SwupdateProgressMessage &other) = default;

    QString toString() const;

    QString apiVersion;
    SwupdateRecoveryStatuses::Enum status;
    SwupdateSourceTypes::Enum source;
    QString info;
    int downloadPercent;
    long long downloadBytes;
    int nbSteps;
    int currentStep;
    int currentStepPercent;
    QString currrentImage;
    QString handlerName;

    SwupdateProgressMessage& operator=(const SwupdateProgressMessage &other) = default;
    bool operator==(const SwupdateProgressMessage& rhs) const = default;
    bool operator!=(const SwupdateProgressMessage& rhs) const = default;
};
Q_DECLARE_METATYPE(SwupdateProgressMessage)
QDebug operator<<(QDebug dbg, const SwupdateProgressMessage &msg);

class Swupdate: public QObject,
                public QQmlSingleton<Swupdate>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool isReady READ isReady CONSTANT FINAL)
    Q_READONLY_VAR_PROPERTY(bool, isRunning, IsRunning, false)
    Q_READONLY_REF_PROPERTY(QString, status, Status, {})
    Q_READONLY_REF_PROPERTY(QString, file, File, {})
    Q_READONLY_REF_PROPERTY(QString, version, Version, {})
    Q_READONLY_VAR_PROPERTY(int, progress, Progress, 0)

    Q_READONLY_REF_PROPERTY(SwupdateProgressMessage, progressMessage, ProgressMessage, {})
    Q_READONLY_VAR_PROPERTY(int, progressFd, ProgressFd, -1)

protected:
    friend QQmlSingleton<Swupdate>;
    explicit Swupdate(QObject* parent=nullptr);

public:
    bool isReady() const;

public slots:
    static void init();
    static void unInit();

    bool update(const QString& file);
    bool restart();

private slots:
    void test();
    void open();
    void onProgressMessage();

private:
    QSocketNotifier* m_socketNotifier=nullptr;
};

#endif // SWUPDATE_H
