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
    static void exitWithError(const QString& msg);

    QString m_importPath;
};

#endif // BACKUPIMPORTER_H
