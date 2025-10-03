#ifndef QAPPLICATIONHELPER_H
#define QAPPLICATIONHELPER_H

#include <QDefs>
#include <QQuickItem>
#include <QQmlParserStatus>

class QApplicationHelper: public QObject,
                          public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ApplicationHelper)
    Q_INTERFACES(QQmlParserStatus)

    Q_WRITABLE_REF_PROPERTY(QString, windowIcon, WindowIcon, {})
    Q_WRITABLE_VAR_PROPERTY(int, cursorShape, CursorShape, -1)
    Q_WRITABLE_REF_PROPERTY(QString, font, Font, {})

    // TODO: QGuiApplication::arguments
    // TODO: QGuiApplication::organizationDomain
    // TODO: QGuiApplication::organizationName
    // TODO: QGuiApplication::applicationName
    // TODO: QGuiApplication::applicationVersion
    // TODO: QGuiApplication::doubleClickInterval
    // TODO: QGuiApplication::keyboardInputInterval

public:
    explicit QApplicationHelper(QObject* parent=nullptr);

    void classBegin() override;
    void componentComplete() override;

protected:
    void refreshWindowIcon();
    void refreshCursor();
    void refreshFont();

private:
    bool m_completed;
};

#endif // QAPPLICATIONHELPER_H
