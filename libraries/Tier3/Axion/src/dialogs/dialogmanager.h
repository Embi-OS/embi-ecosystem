#ifndef DIALOGMANAGER_H
#define DIALOGMANAGER_H

#include <QModels>
#include "qsingleton.h"
#include "dialogobject.h"

class DialogManager : public QObject,
                      public QQmlSingleton<DialogManager>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_COMPOSITION_PROPERTY(QObjectListModel, children, nullptr)

private:
    friend QQmlSingleton<DialogManager>;
    explicit DialogManager(QObject *parent = nullptr);

public:
    Q_INVOKABLE static void saveFileContent(const QByteArray &fileContent, const QString &fileNameHint);
    static void getOpenFileContent(const QString &nameFilter, const std::function<void(const QString&, const QByteArray&)>& fileContentReady);

public slots:
    DialogObject* show(const QVariant& settings={});

    DialogObject* showError(const QVariant& settings={});
    DialogObject* showWarning(const QVariant& settings={});
    DialogObject* showMessage(const QVariant& settings={});
    DialogObject* showAction(const QVariant& settings={});

    DialogObject* showQuit(const QVariant& settings={});
    DialogObject* showCancel(const QVariant& settings={});
    DialogObject* showValidate(const QVariant& settings={});
    DialogObject* showDelete(const QVariant& settings={});

    DialogObject* showText(const QVariant& settings={});
    DialogObject* showTree(const QVariant& settings={});
    DialogObject* showBusy(const QVariant& settings={});

    DialogObject* showDate(const QVariant& settings={});
    DialogObject* showTime(const QVariant& settings={});
    DialogObject* showFileTree(const QVariant& settings={});
    DialogObject* showInput(const QVariant& settings={});
    DialogObject* showForm(const QVariant& settings={});
    DialogObject* showAlarm(const QVariant& settings={});

    void diagnose(DialogSeverities::Enum severity, QString log) const;

private:
    void onObjectInserted(DialogObject* object) const;
};

#endif // DIALOGMANAGER_H
