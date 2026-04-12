#ifndef HAPPYMIGRATION_H
#define HAPPYMIGRATION_H

#include <QDefs>
#include <Sql>

#define HAPPY_MIGRATIONS_TABLE "happy_migrations"

class HappyServer;
class HappyMigration : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit HappyMigration(QObject *parent = nullptr);

    bool run(HappyServer* happyServer);

protected:
    HappyServer* m_happyServer;

private:
    struct MigrationObject {
        QString app;
        QString name;
        QMetaMethod method;
    };

    bool runMigration(const MigrationObject& migration);
};

#endif // HAPPYMIGRATION_H
