#ifndef QITEMCAPTURER_H
#define QITEMCAPTURER_H

#include <QObject>
#include <QQuickItem>
#include <QDefs>

class QItemCapturer : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ItemCapturer)

    Q_WRITABLE_REF_PROPERTY(QString, defaultPath, DefaultPath, "")

public:
    explicit QItemCapturer(QObject *parent = nullptr);

    Q_INVOKABLE void capture(const QString& path="/", QQuickItem* item=nullptr);

signals:
    void captureFinished(bool result, const QString& path);
};

#endif // QITEMCAPTURER_H
