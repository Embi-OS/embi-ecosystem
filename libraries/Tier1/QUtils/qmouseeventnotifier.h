#ifndef QMOUSEEVENTNOTIFIER_H
#define QMOUSEEVENTNOTIFIER_H

#include <QQmlEngine>
#include <QObject>
#include <QQmlParserStatus>
#include <QDefs>

class QMouseEventNotifier : public QObject,
                            public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(MouseEventNotifier)
    Q_INTERFACES(QQmlParserStatus)

    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_READONLY_REF_PROPERTY(QPointF, scenePosition, ScenePosition, {})
    Q_READONLY_VAR_PROPERTY(bool, pressed, Pressed, false)

    Q_WRITABLE_PTR_PROPERTY(QObject, window, Window, nullptr)

public:
    explicit QMouseEventNotifier(QObject *parent = nullptr);

    void classBegin() override;
    void componentComplete() override;

signals:
    void mouseMoved();
    void mousePressed();
    void mouseReleased();
    void mouseEvent();

protected:
    bool eventFilter(QObject *obj, QEvent *event) final override;
};


#endif // QMOUSEEVENTNOTIFIER_H
