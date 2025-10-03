#include "qitemcapturer.h"

#include <QQuickItemGrabResult>
#include <QStandardPaths>
#include <QDateTime>
#include <QFileInfo>

QItemCapturer::QItemCapturer(QObject *parent) :
    QObject(parent)
{

}

void QItemCapturer::capture(const QString& path, QQuickItem* item)
{
    if(item==nullptr)
        return;

    QString captureName = QString("capture_%1.png").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmmss"));

    QString pathTmp=path;
    if(pathTmp.isEmpty())
        pathTmp=m_defaultPath;
    if(pathTmp.isEmpty())
        pathTmp = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

    QSharedPointer<const QQuickItemGrabResult> grabResult = item->grabToImage();
    connect(grabResult.data(), &QQuickItemGrabResult::ready, this, [this, pathTmp, captureName, grabResult]() {

        const QString path = QFileInfo(pathTmp+"/"+captureName).absoluteFilePath();
        bool result = grabResult->saveToFile(path);
        emit this->captureFinished(result, path);

    });
}
