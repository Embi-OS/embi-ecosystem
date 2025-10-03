#ifndef SNACKBAROBJECT_H
#define SNACKBAROBJECT_H

#include <QDefs>
#include <QPropertyAnimation>

Q_ENUM_CLASS(SnackbarSeverities, SnackbarSeverity,
             None,
             Info,
             Warning,
             Error,
             Fatal,
             Success,)

class SnackbarManager;
class SnackbarObject : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Uncreatable")

    Q_WRITABLE_VAR_PROPERTY(SnackbarSeverities::Enum, severity, Severity, SnackbarSeverities::None)
    Q_WRITABLE_REF_PROPERTY(QVariant, settings, Settings, {})
    Q_WRITABLE_REF_PROPERTY(QString, title, Title, {})
    Q_WRITABLE_REF_PROPERTY(QString, caption, Caption, {})
    Q_WRITABLE_REF_PROPERTY(QString, details, Details, {})
    Q_WRITABLE_REF_PROPERTY(QString, button, Button, {})
    Q_WRITABLE_VAR_PROPERTY(bool, closable, Closable, false)
    Q_WRITABLE_VAR_PROPERTY(int, timeout, Timeout, 5000)
    Q_CALLABLE_VAR_PROPERTY(int, progress, Progress, 0)

    Q_WRITABLE_VAR_PROPERTY(bool, visible, Visible, false)

    Q_WRITABLE_VAR_PROPERTY(bool, diagnose, Diagnose, true)

protected:
    friend SnackbarManager;
    explicit SnackbarObject(const QVariant& settings, QObject* parent = nullptr);

public:
    QMetaObject::Connection onClosed(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onAccepted(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);

    void markForDeletion();
    bool isMarkedForDeletion() const;

public slots:
    void close();
    void accept();

signals:
    void closed();
    void accepted();

private slots:
    void updateTimeout();

protected:
    QPropertyAnimation* m_progressAnimation=nullptr;
    bool m_isMarkedForDeletion;
};

struct SnackbarObjectDeleter
{
    static inline void cleanup(SnackbarObject* pointer)
    {
        pointer->close();
    }
};
typedef QScopedPointer<SnackbarObject, SnackbarObjectDeleter> ScopedSnackbarObject;

#endif // SNACKBAROBJECT_H
