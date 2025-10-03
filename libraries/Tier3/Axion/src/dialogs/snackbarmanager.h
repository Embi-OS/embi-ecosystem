#ifndef SNACKBARMANAGER_H
#define SNACKBARMANAGER_H

#include <QModels>
#include "qsingleton.h"
#include "snackbarobject.h"

class SnackbarManager : public QObject,
                        public QQmlSingleton<SnackbarManager>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_COMPOSITION_PROPERTY(QObjectListModel, children, nullptr)

private:
    friend QQmlSingleton<SnackbarManager>;
    explicit SnackbarManager(QObject *parent = nullptr);

public slots:
    SnackbarObject* show(const QVariant& settings={});
    SnackbarObject* showInfo(const QVariant& settings={});
    SnackbarObject* showWarning(const QVariant& settings={});
    SnackbarObject* showError(const QVariant& settings={});
    SnackbarObject* showFatal(const QVariant& settings={});
    SnackbarObject* showSuccess(const QVariant& settings={});

    void diagnose(SnackbarSeverity severity, QString log) const;

private:
    void onObjectInserted(SnackbarObject* object) const;
};

#endif // SNACKBARMANAGER_H
