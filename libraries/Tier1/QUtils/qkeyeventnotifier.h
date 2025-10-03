#ifndef QKEYEVENTNOTIFIER_H
#define QKEYEVENTNOTIFIER_H

#include <QQmlEngine>
#include <QObject>
#include <QQmlParserStatus>
#include <QDefs>

class QKeyEventNotifier : public QObject,
                          public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(KeyEventNotifier)
    Q_INTERFACES(QQmlParserStatus)

    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)

    Q_WRITABLE_PTR_PROPERTY(QObject, window, Window, nullptr)

public:
    explicit QKeyEventNotifier(QObject *parent = nullptr);

    void classBegin() override;
    void componentComplete() override;

signals:
    void keyPressed(int key);
    void keyReleased(int key);
    void keyEvent(int key);

protected:
    bool eventFilter(QObject *obj, QEvent *event) final override;
};


#endif // QKEYEVENTNOTIFIER_H
