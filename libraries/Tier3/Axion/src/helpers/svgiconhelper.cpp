#include "svgiconhelper.h"
#include "axion_log.h"

#include <QUrl>
#include <QDir>
#include <QHash>
#include <QFile>
#include <QImage>
#include <QImageReader>
#include <QDebug>
#include <QPainter>
#include <QDirIterator>
#include <QStringBuilder>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <QMultiHash>
#include <QByteArray>
#include <QDebug>
#include <QElapsedTimer>
#include <QVarLengthArray>
#include <QSvgRenderer>

#define SVG_ICON_HELPER_THREADED

#ifdef QT_CONCURRENT_LIB
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#else
#undef SVG_ICON_HELPER_THREADED
#endif

class SvgMetaDataCache {
public:
    explicit SvgMetaDataCache()
    {
        const QString cacheDirPath=QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        changeCachePath(cacheDirPath);
    }

    void changeCachePath(const QString& path) {
        QMutexLocker lock(&pathMutex);
        cachePath = path % "/SvgIconsCache";
        QDir().mkpath(cachePath);
    }

    bool doGenerateIconData(SvgIconData& iconData)
    {
        iconData.sourcePath = generatePath(iconData.icon);

        QFileInfo fileInfo(iconData.sourcePath);
        if(!fileInfo.exists(iconData.sourcePath) || fileInfo.suffix().compare("svg", Qt::CaseInsensitive)!=0)
        {
            AXIONLOG_WARNING()<<">>> SvgIconHelper:"<<iconData.sourcePath<<"does not exists or is not an svg file";
            return false;
        }

        iconData.hash = generateHash(iconData.sourcePath);
        iconData.cachedPath = generateCachePath(iconData.hash, iconData.size, iconData.aspectRatio, iconData.color);

        return true;
    }

#ifdef SVG_ICON_HELPER_THREADED
    QFutureWatcher<bool>* doRenderSvgToPng(const SvgIconData& iconData) {
        if(hasFuture(iconData.cachedPath)) {
            return future(iconData.cachedPath);
        }

        QFuture<bool> future = QtConcurrent::run([this](const SvgIconData& iconData) {
            if(QFile::exists(iconData.cachedPath))
                return true;
            return renderSvgToPng(iconData.sourcePath, iconData.cachedPath, iconData.size, iconData.color, iconData.aspectRatio);
        }, iconData);

        QFutureWatcher<bool>* watcher = new QFutureWatcher<bool>();
        QObject::connect(watcher, &QFutureWatcherBase::finished, watcher, [this, iconData]() {
            removeFuture(iconData.cachedPath);
        });
        QObject::connect(watcher, &QFutureWatcherBase::finished, watcher, [watcher]() {
            watcher->deleteLater();
        }, Qt::QueuedConnection);

        watcher->setFuture(future);

        addFuture(iconData.cachedPath, watcher);

        return watcher;
    }
#endif

    void doClearHash(const QString& hash) {
        removeHashFromIndex(hash);
    }

    QString cacheFile(const QString& name = "") {
        QMutexLocker lock(&pathMutex);
        return(cachePath % "/" % name);
    }

    QString hashFile(const QString& filePath) {
        QString ret;
        QFile file(filePath);
        if(file.open(QFile::ReadOnly)) {
            QCryptographicHash hasher(QCryptographicHash::Md5);
            hasher.addData(&file);
            ret = hasher.result().toHex();
            file.close();
        }
        return ret;
    }

    QString hashData(const QByteArray& data) {
        QString ret;
        if(!data.isEmpty()) {
            QCryptographicHash hasher(QCryptographicHash::Md5);
            hasher.addData(data);
            ret = hasher.result().toHex();
        }
        return ret;
    }

    bool hasHashInIndex(const QString& hash) {
        QMutexLocker lock(&checksumsMutex);
        return checksumsIndex.contains(hash);
    }

    void addHashInIndex(const QString& hash, const QString& checksum) {
        QMutexLocker lock(&checksumsMutex);
        checksumsIndex.insert(hash, checksum);
    }

    void removeHashFromIndex(const QString& hash) {
        QMutexLocker lock(&checksumsMutex);
        checksumsIndex.remove(hash);
    }

#ifdef QT_CONCURRENT_LIB
    bool hasFuture(const QString& cachedPath) {
        QMutexLocker lock(&saveMutex);
        return runningSaves.contains(cachedPath);
    }

    QFutureWatcher<bool>* future(const QString& cachedPath) {
        QMutexLocker lock(&saveMutex);
        return runningSaves.value(cachedPath);
    }

    void addFuture(const QString& cachedPath, QFutureWatcher<bool>* future) {
        QMutexLocker lock(&saveMutex);
        runningSaves.insert(cachedPath, future);
    }

    void removeFuture(const QString& cachedPath) {
        QMutexLocker lock(&saveMutex);
        runningSaves.remove(cachedPath);
    }
#endif

    QString readChecksumFile(const QString& filePath) {
        QString ret;
        QFile file(filePath);
        if(file.open(QFile::ReadOnly)) {
            ret = QString::fromLatin1(file.readAll());
            file.close();
        }
        return ret;
    }

    void writeChecksumFile(const QString& filePath, const QString& checksum) {
        QFile file(filePath);
        if(file.open(QFile::WriteOnly)) {
            file.write(checksum.toLatin1());
            file.flush();
            file.close();
        }
    }

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

    QString generateHash(const QString& sourcePath)
    {
        QMutexLocker lock(hashMutex(sourcePath));

        const QString hash = hashData(sourcePath.toLatin1());

        if(!hasHashInIndex(hash))
        {
            const QString checkumPath = cacheFile(hash % ".md5");
            const QString reference = readChecksumFile(checkumPath);
            const QString checksum  = hashFile(sourcePath);
            if(reference != checksum)
            {
                QDirIterator it(cacheFile(),
                                QStringList(hash % "*.png"),
                                QDir::Filters(QDir::Files | QDir::NoDotAndDotDot),
                                QDirIterator::IteratorFlags(QDirIterator::NoIteratorFlags));
                while(it.hasNext())
                {
                    const QString next = it.next();
                    QFile::remove(next);
                }
                writeChecksumFile(checkumPath, checksum);
            }
            addHashInIndex(hash, checksum);
        }

        return hash;
    }

    QString generateCachePath(const QString& hash, const QSize& size, const Qt::AspectRatioMode aspectRatio, const QColor& color)
    {
        const QString cachedPath = cacheFile(hash
                                             % "_" % QString::number(size.width())
                                             % "x" % QString::number(size.height())
                                             % "_" % QString::number(aspectRatio)
                                             % "_" %(color.alpha() > 0 ? color.name(QColor::HexRgb).toUpper() : "")
                                             % ".png");

        return cachedPath;
    }

    bool renderSvgToPng(const QString& svgPath, const QString& pngPath, const QSize& size, const QColor& color, Qt::AspectRatioMode aspectRatio)
    {
        QSvgRenderer renderer;
        renderer.load(svgPath);
        if(!renderer.isValid())
        {
            AXIONLOG_WARNING()<<">>> SvgIconHelper : invalid renderer";
            return false;
        }

        const QSize imageSize = renderer.defaultSize();
        const QSize effectiveSize = imageSize.scaled(size, aspectRatio);

        QImage image(effectiveSize.width(), effectiveSize.height(), QImage::Format_ARGB32);
        image.fill(Qt::transparent);

        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::TextAntialiasing, true);

        renderer.render(&painter);
        if(color.isValid() && color.alpha() > 0)
        {
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(image.rect(), color);
        }

        if(!image.save(pngPath, "PNG"))
        {
            AXIONLOG_WARNING()<<">>> SvgIconHelper : failed to save"<<pngPath;
            return false;
        }

        return true;
    }

    QMutex* hashMutex(const QString& sourcePath) {
        QMutex* mutex = hashMutexes.value(sourcePath, nullptr);
        if(!mutex) {
            QMutexLocker lock(&hashMutexesMutex);
            mutex = new QMutex;
            hashMutexes.insert(sourcePath, mutex);
        }
        return mutex;
    }

private:
    QString cachePath;
    QHash<QString, QString> checksumsIndex;
#ifdef QT_CONCURRENT_LIB
    QHash<QString, QFutureWatcher<bool>*> runningSaves;
#endif

    QRecursiveMutex pathMutex;
    QRecursiveMutex checksumsMutex;
    QRecursiveMutex saveMutex;

    QRecursiveMutex hashMutexesMutex;
    QHash<QString, QMutex*> hashMutexes;
};

Q_GLOBAL_STATIC(SvgMetaDataCache, s_cache)

SvgIconHelper::SvgIconHelper(QObject * parent):
    QObject(parent),
    m_ready(false),
    m_iconWidth(0),
    m_iconHeight(0),
    m_aspectRatio(Qt::IgnoreAspectRatio),
    m_asynchronous(true),
    m_inhibitTime(0),
    m_color(Qt::transparent),
    m_icon(QString()),
    m_complete(false),
    m_inhibitTimer(this)
{
    m_inhibitTimer.setInterval(m_inhibitTime);
    m_inhibitTimer.setSingleShot(true);
    connect(&m_inhibitTimer, &QTimer::timeout, this, &SvgIconHelper::doProcessIcon, Qt::UniqueConnection);

    connect(this, &SvgIconHelper::iconWidthChanged, this, &SvgIconHelper::scheduleRefresh);
    connect(this, &SvgIconHelper::iconHeightChanged, this, &SvgIconHelper::scheduleRefresh);
    connect(this, &SvgIconHelper::aspectRatioChanged, this, &SvgIconHelper::scheduleRefresh);
    connect(this, &SvgIconHelper::asynchronousChanged, this, &SvgIconHelper::scheduleRefresh);
    connect(this, &SvgIconHelper::colorChanged, this, &SvgIconHelper::scheduleRefresh);
    connect(this, &SvgIconHelper::iconChanged, this, &SvgIconHelper::scheduleRefresh);
}

void SvgIconHelper::classBegin()
{

}

void SvgIconHelper::componentComplete()
{
    m_complete = true;
    scheduleRefresh();
}

void SvgIconHelper::setTarget(const QQmlProperty& target)
{
    m_property = target;
    scheduleRefresh();
}

void SvgIconHelper::setCachePath(const QString& cachePath)
{
    s_cache->changeCachePath(cachePath);
}

void SvgIconHelper::scheduleRefresh()
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

void SvgIconHelper::onInhibitTimeChanged(int inhibitTime)
{
    m_inhibitTimer.stop();
    m_inhibitTimer.setInterval(m_inhibitTime);
    m_inhibitTimer.start();
}

void SvgIconHelper::doForceRegen()
{
    if(!m_hash.isEmpty())
    {
        s_cache->doClearHash(m_hash);
        scheduleRefresh();
    }
}

void SvgIconHelper::doUpdateProperty()
{
    if(QFile::exists(m_cachedPath))
    {
        const QUrl url = QUrl::fromLocalFile(m_cachedPath);
        if(m_property.isValid() && m_property.isWritable())
        {
            if(!m_property.write(url))
            {
                AXIONLOG_WARNING()<<">>> SvgIconHelper : Can't set"<<m_sourcePath<<m_cachedPath;
                QFile::remove(m_cachedPath);
                doForceRegen();
            }
        }
    }
    setReady(true);
}

void SvgIconHelper::doProcessIcon()
{
    if(!m_complete)
        return;

#ifdef SVG_ICON_HELPER_THREADED
    if (m_futureIconData) {
        m_futureIconData->deleteLater();
        m_futureIconData = nullptr;
    }
#endif

    SvgIconData iconData;
    iconData.size = QSize(m_iconWidth, m_iconHeight);
    iconData.aspectRatio = m_aspectRatio;
    iconData.color = m_color;
    iconData.icon = m_icon;

    if(iconData.icon.isEmpty())
    {
        m_sourcePath.clear();
        m_cachedPath.clear();
        doUpdateProperty();
        return;
    }

    if(iconData.size.isEmpty() && iconData.aspectRatio!=Qt::KeepAspectRatioByExpanding)
    {
        doUpdateProperty();
        return;
    }

#ifdef SVG_ICON_HELPER_THREADED
    QFuture<SvgIconData> future = QtConcurrent::run([](SvgIconData iconData) {
        bool result = s_cache->doGenerateIconData(iconData);
        iconData.result = result;
        return iconData;
    }, std::move(iconData));

    m_futureIconData = new QFutureWatcher<SvgIconData>(this);
    connect(m_futureIconData, &QFutureWatcherBase::finished, this, [this]() {
        if(!m_futureIconData)
            return;
        const SvgIconData iconData = m_futureIconData->future().result();
        m_futureIconData->deleteLater();
        m_futureIconData = nullptr;

        doProcessSvgToPng(iconData);
    });
    m_futureIconData->setFuture(future);

    if(!m_asynchronous)
        m_futureIconData->waitForFinished();

#else
    bool result = s_cache->doGenerateIconData(iconData);
    iconData.result = result;
    doProcessSvgToPng(iconData);
#endif
}

void SvgIconHelper::doProcessSvgToPng(const SvgIconData& iconData)
{
    if(!iconData.result)
        return;

    m_hash = iconData.hash;
    m_sourcePath = iconData.sourcePath;
    m_cachedPath = iconData.cachedPath;

#ifdef SVG_ICON_HELPER_THREADED

    if(!m_futureRenderSvgToPng.isNull())
    {
        disconnect(m_futureRenderSvgToPng, &QFutureWatcherBase::finished, this, &SvgIconHelper::doUpdateProperty);
        m_futureRenderSvgToPng.clear();
    }

    m_futureRenderSvgToPng = s_cache->doRenderSvgToPng(iconData);
    if(m_futureRenderSvgToPng->isFinished())
    {
        doUpdateProperty();
        return;
    }

    connect(m_futureRenderSvgToPng, &QFutureWatcherBase::finished, this, &SvgIconHelper::doUpdateProperty);

    if(!m_asynchronous)
        m_futureRenderSvgToPng->waitForFinished();

#else

    if(!QFile::exists(iconData.cachedPath))
        s_cache->renderSvgToPng(iconData.sourcePath, iconData.cachedPath, iconData.size, iconData.color, iconData.aspectRatio);
    doUpdateProperty();

#endif
}
