#ifndef BACKUPIMPORTER_H
#define BACKUPIMPORTER_H

#include <QDefs>

class BackupImporter : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit BackupImporter(QObject* parent=nullptr);

public slots:
    void run();
    void run(const QString& path);

private:
    bool doRun(const QString& importPath);
    static void exitWithError(const QString& msg);
};

#endif // BACKUPIMPORTER_H
