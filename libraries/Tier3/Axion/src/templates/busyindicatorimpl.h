#ifndef BUSYINDICATORIMPL_H
#define BUSYINDICATORIMPL_H

#include <QDefs>
#include <QQuickItem>

class BusyIndicatorImpl : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QColor, color, Color, Qt::white)
    Q_WRITABLE_REF_PROPERTY(QColor, backgroundColor, BackgroundColor, Qt::black)
    Q_WRITABLE_FUZ_PROPERTY(double, lineWidth, LineWidth, 1)
    Q_WRITABLE_VAR_PROPERTY(Qt::PenCapStyle, capStyle, CapStyle, Qt::RoundCap)

    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged FINAL)

public:
    explicit BusyIndicatorImpl(QQuickItem *parent = nullptr);

    bool isRunning() const;
    void setRunning(bool running);

    int elapsed() const;

signals:
    void runningChanged();

protected:
    void itemChange(ItemChange change, const ItemChangeData &data) override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    int m_elapsed = 0;
};

#endif // BUSYINDICATORIMPL_H
