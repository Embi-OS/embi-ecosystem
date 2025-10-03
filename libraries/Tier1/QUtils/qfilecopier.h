#ifndef QFILECOPIER_H
#define QFILECOPIER_H

#include <QObject>
#include <QDefs>

class QtFileCopier;
class QFileCopier: public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(FileCopier)

    Q_WRITABLE_VAR_PROPERTY(bool, autoDelete, AutoDelete, true)
    Q_WRITABLE_VAR_PROPERTY(bool, asynchronous, Asynchronous, true)
    Q_WRITABLE_VAR_PROPERTY(int, progressInterval, ProgressInterval, 16)

    Q_READONLY_VAR_PROPERTY(bool, running, Running, false)
    Q_READONLY_VAR_PROPERTY(int, fileProgress, FileProgress, 0)
    Q_READONLY_VAR_PROPERTY(int, overallProgress, OverallProgress, 0)
    Q_READONLY_VAR_PROPERTY(int, remainingTime, RemainingTime, 0)
    Q_READONLY_VAR_PROPERTY(int, totalTime, TotalTime, 0)
    Q_READONLY_REF_PROPERTY(QString, currentFile, CurrentFile, {})
    Q_READONLY_REF_PROPERTY(QString, status, Status, "")

public:
    explicit QFileCopier(QObject* parent=nullptr);

public slots:
    bool copy(const QString& source, const QString& destination, bool overwrite=false);
    bool copy(const QStringList& sources, const QString& destination, bool overwrite=false);

    bool move(const QString& sourceFile, const QString& destinationPath, bool overwrite=false);
    bool move(const QStringList& sourceFile, const QString& destinationPath, bool overwrite=false);

    bool remove(const QString& sourceFile);
    bool remove(const QStringList& sourceFile);

    bool cancel();

signals:
    void finished(bool result);
    void progress(const QString& status, int progress);

    void errorOccurred(const QString& error);

private:
    QtFileCopier* m_worker=nullptr;
};

#endif // QFILECOPIER_H
