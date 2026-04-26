#ifndef BACKUPEXPORTER_H
#define BACKUPEXPORTER_H

#include <QDefs>

class BackupExporter : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit BackupExporter(QObject* parent=nullptr);

public slots:
    void run();
    void run(const QString& path);

private:
    bool doRun(const QString& path);
    static void exitWithError(const QString& msg);
};

#endif // BACKUPEXPORTER_H
