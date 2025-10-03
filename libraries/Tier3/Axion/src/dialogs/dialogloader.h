#ifndef DIALOGLOADER_H
#define DIALOGLOADER_H

#include <QDefs>
#include "dialogobject.h"

class DialogLoader : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(bool, active, Active, false)

    Q_REQUIRED_VAR_PROPERTY(DialogTypes::Enum, type, Type, DialogTypes::None)

    Q_WRITABLE_VAR_PROPERTY(DialogSeverities::Enum, severity, Severity, DialogSeverities::Message)
    Q_WRITABLE_REF_PROPERTY(QVariant, settings, Settings, {})
    Q_WRITABLE_REF_PROPERTY(QString, title, Title, "")
    Q_WRITABLE_REF_PROPERTY(QString, message, Message, "")
    Q_WRITABLE_REF_PROPERTY(QString, infos, Infos, "")
    Q_WRITABLE_REF_PROPERTY(QString, traces, Traces, "")
    Q_WRITABLE_VAR_PROPERTY(int, progress, Progress, 0)

    Q_WRITABLE_REF_PROPERTY(QString, buttonApply, ButtonApply, "")
    Q_WRITABLE_REF_PROPERTY(QString, buttonAccept, ButtonAccept, "")
    Q_WRITABLE_REF_PROPERTY(QString, buttonReject, ButtonReject, "")
    Q_WRITABLE_REF_PROPERTY(QString, buttonDiscard, ButtonDiscard, "")
    Q_WRITABLE_REF_PROPERTY(QString, buttonHelp, ButtonHelp, "")

    Q_WRITABLE_VAR_PROPERTY(bool, diagnose, Diagnose, false)

public:
    explicit DialogLoader(QObject* parent = nullptr);
    virtual ~DialogLoader();

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

private slots:
    void clearDialog();
    void onActiveChanged(bool active);

private:
    QPointer<DialogObject> m_dialog;
};

#endif // DIALOGLOADER_H
