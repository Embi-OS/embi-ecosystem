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
    static void exitWithError(const QString& msg);

    QString m_exportPath;
};

#endif // BACKUPEXPORTER_H
