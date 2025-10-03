#include "svgimagehelper.h"
#include "axion_log.h"

#include <QSvgRenderer>

#define SVG_IMAGE_HELPER_THREADED

#ifdef QT_CONCURRENT_LIB
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#else
#undef SVG_IMAGE_HELPER_THREADED
#endif

QString generatePath(const QString& path)
{
    QString sourcePath = path;

    if (sourcePath.startsWith ("qrc:///")) {
        sourcePath.replace("qrc:///", ":/");
    }
    else if (sourcePath.startsWith ("qrc://")) {
        sourcePath.replace("qrc://", ":/");
    }
    else if (sourcePath.startsWith ("qrc:/")) {
        sourcePath.replace("qrc:/", ":/");
    }
    else if (sourcePath.startsWith ("file://")) {
        sourcePath = QUrl(sourcePath).toLocalFile();
    }

    return sourcePath;
}

SvgImageHelper::SvgImageHelper(QObject * parent):
    QObject(parent),
    m_ready(false),
    m_iconWidth(0),
    m_iconHeight(0),
    m_aspectRatio(Qt::IgnoreAspectRatio),
    m_asynchronous(true),
    m_inhibitTime(0),
    m_icon(QString()),
    m_complete(false),
    m_inhibitTimer(this)
{
    m_inhibitTimer.setInterval(m_inhibitTime);
    m_inhibitTimer.setSingleShot(true);
    connect(&m_inhibitTimer, &QTimer::timeout, this, &SvgImageHelper::doProcessIcon, Qt::UniqueConnection);

    connect(this, &SvgImageHelper::iconWidthChanged, this, &SvgImageHelper::scheduleRefresh);
    connect(this, &SvgImageHelper::iconHeightChanged, this, &SvgImageHelper::scheduleRefresh);
    connect(this, &SvgImageHelper::aspectRatioChanged, this, &SvgImageHelper::scheduleRefresh);
    connect(this, &SvgImageHelper::asynchronousChanged, this, &SvgImageHelper::scheduleRefresh);
    connect(this, &SvgImageHelper::iconChanged, this, &SvgImageHelper::scheduleRefresh);
}

void SvgImageHelper::classBegin()
{

}

void SvgImageHelper::componentComplete()
{
    m_complete = true;
    scheduleRefresh();
}

void SvgImageHelper::setTarget(const QQmlProperty& target)
{
    m_property = target;
    scheduleRefresh();
}

QSize SvgImageHelper::processSvgSize(const QString& svgPath, const QSize& size, Qt::AspectRatioMode aspectRatio)
{
    const QString sourcePath = generatePath(svgPath);

    QSvgRenderer renderer;
    renderer.load(sourcePath);
    if(!renderer.isValid())
    {
        AXIONLOG_WARNING()<<">>> SvgImageHelper : invalid renderer";
        return size;
    }

    const QSize imageSize = renderer.defaultSize();
    const QSize effectiveSize = imageSize.scaled(size, aspectRatio);

    return effectiveSize;
}

void SvgImageHelper::scheduleRefresh()
{
    setReady(false);
    m_inhibitTimer.stop();

    if(m_inhibitTime<0)
    {
        doProcessIcon();
        return;
    }

    m_inhibitTimer.start();
}

void SvgImageHelper::onInhibitTimeChanged(int inhibitTime)
{
    m_inhibitTimer.stop();
    m_inhibitTimer.setInterval(inhibitTime);
    m_inhibitTimer.start();
}

void SvgImageHelper::doProcessIcon()
{
    if(!m_complete)
        return;

    const QSize iconSize = QSize(m_iconWidth, m_iconHeight);

    if(m_icon.isEmpty() || m_aspectRatio==Qt::IgnoreAspectRatio)
    {
        doUpdateSourceSize(iconSize);
        return;
    }

#ifdef SVG_IMAGE_HELPER_THREADED

    if(!m_futureSizeWatcher.isNull()) {
        m_futureSizeWatcher->cancel();
        m_futureSizeWatcher->deleteLater();
    }

    if(m_asynchronous)
    {
        QFuture<QSize> future = QtConcurrent::run(SvgImageHelper::processSvgSize, m_icon, iconSize, m_aspectRatio);
        m_futureSizeWatcher = new QFutureWatcher<QSize>(this);
        connect(m_futureSizeWatcher, &QFutureWatcherBase::finished, this, [this]() {
            if(m_futureSizeWatcher.isNull())
                return;
            const QSize size = m_futureSizeWatcher->future().result();
            doUpdateSourceSize(size);
        });
        m_futureSizeWatcher->setFuture(future);
    }
    else
#endif
    {
        const QSize size = SvgImageHelper::processSvgSize(m_icon, iconSize, m_aspectRatio);
        doUpdateSourceSize(size);
    }
}

void SvgImageHelper::doUpdateSourceSize(const QSize& sourceSize)
{
    setSourceSize(sourceSize);
    doUpdateProperty();
}

void SvgImageHelper::doUpdateProperty()
{
    if(m_property.isValid() && m_property.isWritable())
    {
        if(!m_property.write(m_icon))
        {
            AXIONLOG_WARNING()<<">>> SvgImageHelper : Can't set"<<m_icon;
        }
    }

    setReady(true);
}
