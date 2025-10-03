#ifndef SVGIMAGEHELPER_H
#define SVGIMAGEHELPER_H

#include <QDefs>
#include <QUtils>

#include <QTimer>
#include <QQmlProperty>
#include <QQmlParserStatus>
#include <QQmlPropertyValueSource>

#ifdef QT_CONCURRENT_LIB
#include <QFutureWatcher>
#endif

class SvgImageHelper: public QObject,
                      public QQmlParserStatus,
                      public QQmlPropertyValueSource
{
    Q_OBJECT
    QML_ELEMENT

    Q_INTERFACES (QQmlParserStatus)
    Q_INTERFACES (QQmlPropertyValueSource)

    Q_READONLY_VAR_PROPERTY(bool, ready, Ready, false)
    Q_READONLY_VAR_PROPERTY(QSize, sourceSize, SourceSize, {})

    Q_WRITABLE_VAR_PROPERTY(int, iconWidth, IconWidth, 0)
    Q_WRITABLE_VAR_PROPERTY(int, iconHeight, IconHeight, 0)
    Q_WRITABLE_VAR_PROPERTY(Qt::AspectRatioMode, aspectRatio, AspectRatio, Qt::IgnoreAspectRatio)
    Q_WRITABLE_VAR_PROPERTY(bool, asynchronous, Asynchronous, true)
    Q_WRITABLE_VAR_PROPERTY(int, inhibitTime, InhibitTime, 0)
    Q_WRITABLE_REF_PROPERTY(QString, icon, Icon, {})

public:
    explicit SvgImageHelper(QObject* parent=nullptr);

    void classBegin() override final;
    void componentComplete() override final;
    void setTarget(const QQmlProperty& target) override final;

    static QSize processSvgSize(const QString& svgPath, const QSize& size, Qt::AspectRatioMode aspectRatio);

protected slots:
    void scheduleRefresh();
    void onInhibitTimeChanged(int inhibitTime);

    void doProcessIcon();

protected:
    void doUpdateSourceSize(const QSize& sourceSize);
    void doUpdateProperty();

private:
    bool m_complete=false;
    QTimer m_inhibitTimer;
    QQmlProperty m_property;

#ifdef QT_CONCURRENT_LIB
    QPointer<QFutureWatcher<QSize>> m_futureSizeWatcher;
#endif
};

#endif // SVGIMAGEHELPER_H
