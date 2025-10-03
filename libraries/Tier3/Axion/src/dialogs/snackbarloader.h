#ifndef SNACKBARLOADER_H
#define SNACKBARLOADER_H

#include <QDefs>
#include "snackbarobject.h"

class SnackbarLoader : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(bool, active, Active, false)

    Q_WRITABLE_VAR_PROPERTY(SnackbarSeverities::Enum, severity, Severity, SnackbarSeverities::None)
    Q_WRITABLE_REF_PROPERTY(QVariant, settings, Settings, {})
    Q_WRITABLE_REF_PROPERTY(QString, title, Title, {})
    Q_WRITABLE_REF_PROPERTY(QString, caption, Caption, {})
    Q_WRITABLE_REF_PROPERTY(QString, details, Details, {})
    Q_WRITABLE_REF_PROPERTY(QString, button, Button, {})
    Q_WRITABLE_VAR_PROPERTY(bool, closable, Closable, false)
    Q_CALLABLE_VAR_PROPERTY(int, progress, Progress, 0)

    Q_WRITABLE_VAR_PROPERTY(bool, diagnose, Diagnose, false)

public:
    explicit SnackbarLoader(QObject* parent = nullptr);
    virtual ~SnackbarLoader();

signals:
    void closed();
    void accepted();

private slots:
    void clearSnackbar();
    void onActiveChanged(bool active);

private:
    QPointer<SnackbarObject> m_snackbar;
};

#endif // SNACKBARLOADER_H
