#ifndef DIALOGOBJECT_H
#define DIALOGOBJECT_H

#include <QDefs>

Q_ENUM_CLASS(DialogTypes, DialogType,
             None,
             Action,
             Text,
             Tree,
             Busy,
             Date,
             Time,
             FileTree,
             Input,
             Form,
             Alarm)

Q_ENUM_CLASS(DialogSeverities, DialogSeverity,
             None,
             Message,
             Info,
             Warning,
             Error,
             Fatal,)

class DialogManager;
class DialogObject : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Uncreatable")

    Q_READONLY_VAR_PROPERTY(bool, isVisible, IsVisible, false)
    Q_READONLY_VAR_PROPERTY(bool, fullscreen, Fullscreen, false)

    Q_READONLY_VAR_PROPERTY(DialogTypes::Enum, type, Type, DialogTypes::None)
    Q_WRITABLE_VAR_PROPERTY(DialogSeverities::Enum, severity, Severity, DialogSeverities::None)
    Q_WRITABLE_REF_PROPERTY(QVariant, settings, Settings, {})
    Q_WRITABLE_REF_PROPERTY(QString, title, Title, "")
    Q_WRITABLE_REF_PROPERTY(QString, message, Message, "")
    Q_CALLABLE_REF_PROPERTY(QString, infos, Infos, "")
    Q_CALLABLE_REF_PROPERTY(QString, traces, Traces, "")
    Q_CALLABLE_VAR_PROPERTY(int, progress, Progress, 0)

    Q_WRITABLE_REF_PROPERTY(QString, buttonApply, ButtonApply, "")
    Q_WRITABLE_REF_PROPERTY(QString, buttonAccept, ButtonAccept, "")
    Q_WRITABLE_REF_PROPERTY(QString, buttonReject, ButtonReject, "")
    Q_WRITABLE_REF_PROPERTY(QString, buttonDiscard, ButtonDiscard, "")
    Q_WRITABLE_REF_PROPERTY(QString, buttonHelp, ButtonHelp, "")

    Q_WRITABLE_VAR_PROPERTY(bool, diagnose, Diagnose, true)

protected:
    friend DialogManager;
    explicit DialogObject(const QVariant& settings, QObject* parent = nullptr);

public:
    QMetaObject::Connection onAboutToHide(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onAboutToShow(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onClosed(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onOpened(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);

    QMetaObject::Connection onAccepted(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onApplied(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onDiscarded(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onHelpRequested(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onRejected(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onReset(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);

    QMetaObject::Connection onDateSelected(std::function<void(const QDate& date)> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onTimeSelected(std::function<void(const QTime& time)> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onPathSelected(std::function<void(const QString& path)> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onInputAccepted(std::function<void(const QString& value)> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onInputApplied(std::function<void(const QString& value)> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onFormValidated(std::function<void(const QVariantMap& formValues)> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    QMetaObject::Connection onAlarmValidated(std::function<void(const QVariantMap& alarmMap)> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);

    void markForDeletion();
    bool isMarkedForDeletion() const;

public slots:
    void show();
    void hide();

signals:
    // QtQuick.Controls Popup
    void aboutToHide();
    void aboutToShow();
    void closed();
    void opened();

    // QtQuick.Controls Dialog
    void accepted();
    void applied();
    void discarded();
    void helpRequested();
    void rejected();
    void reset();

    // Eco.Tier3.Axion Dialog
    void dateSelected(const QDate& date);
    void timeSelected(const QTime& time);
    void pathSelected(const QString& path);
    void inputAccepted(const QString& value);
    void inputApplied(const QString& value);
    void formValidated(const QVariantMap& formValues);
    void alarmValidated(const QVariantMap& alarmMap);

protected slots:
    void onSettingsChanged(const QVariant& settings);

protected:
    bool m_isMarkedForDeletion;
};

struct DialogObjectDeleter
{
    static inline void cleanup(DialogObject* pointer)
    {
        pointer->hide();
    }
};
typedef QScopedPointer<DialogObject, DialogObjectDeleter> ScopedDialogObject;

#endif // DIALOGOBJECT_H
