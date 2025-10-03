#ifndef SVGICONHELPER_H
#define SVGICONHELPER_H

#include <QDefs>
#include <QUtils>

#include <QTimer>
#include <QColor>
#include <QQmlProperty>
#include <QQmlParserStatus>
#include <QQmlPropertyValueSource>

#ifdef QT_CONCURRENT_LIB
#include <QFutureWatcher>
#endif

struct SvgIconData {
    QSize size;
    Qt::AspectRatioMode aspectRatio;
    QColor color;
    QString icon;

    QString hash;
    QString sourcePath;
    QString cachedPath;

    bool result;
};

class SvgIconHelper: public QObject,
                     public QQmlParserStatus,
                     public QQmlPropertyValueSource
{
    Q_OBJECT
    QML_ELEMENT

    Q_INTERFACES (QQmlParserStatus)
    Q_INTERFACES (QQmlPropertyValueSource)

    Q_READONLY_VAR_PROPERTY(bool, ready, Ready, false)
    Q_WRITABLE_VAR_PROPERTY(int, iconWidth, IconWidth, 0)
    Q_WRITABLE_VAR_PROPERTY(int, iconHeight, IconHeight, 0)
    Q_WRITABLE_VAR_PROPERTY(Qt::AspectRatioMode, aspectRatio, AspectRatio, Qt::IgnoreAspectRatio)
    Q_WRITABLE_VAR_PROPERTY(bool, asynchronous, Asynchronous, true)
    Q_WRITABLE_VAR_PROPERTY(int, inhibitTime, InhibitTime, 0)
    Q_WRITABLE_REF_PROPERTY(QColor, color, Color, Qt::transparent)
    Q_WRITABLE_REF_PROPERTY(QString, icon, Icon, {})

public:
    explicit SvgIconHelper(QObject* parent=nullptr);

    static void setCachePath(const QString& cachePath);

    void classBegin() override final;
    void componentComplete() override final;
    void setTarget(const QQmlProperty& target) override final;

protected slots:
    void scheduleRefresh();
    void onInhibitTimeChanged(int inhibitTime);

    void doForceRegen();
    void doUpdateProperty();
    void doProcessIcon();

protected:
    void doProcessSvgToPng(const SvgIconData& iconData);

private:
    bool m_complete;
    QTimer m_inhibitTimer;
    QQmlProperty m_property;

    QString m_hash;
    QString m_sourcePath;
    QString m_cachedPath;

#ifdef QT_CONCURRENT_LIB
    QFutureWatcher<SvgIconData> *m_futureIconData = nullptr;
    QPointer<QFutureWatcher<bool>> m_futureRenderSvgToPng = nullptr;
#endif
};

#endif // SVGICONHELPER_H
