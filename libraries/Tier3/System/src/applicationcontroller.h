#ifndef APPLICATIONCONTROLLER_H
#define APPLICATIONCONTROLLER_H

#include <QDefs>

class ApplicationController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool hasAppController READ hasAppController CONSTANT FINAL)
    Q_PROPERTY(QString currentDefault READ currentDefault NOTIFY currentDefaultChanged FINAL)

public:
    explicit ApplicationController(QObject* parent=nullptr);

    static void init(bool makeDefault=true);

    bool hasAppController();
    QString currentDefault() const;

public slots:
    void update();
    void update(const QString& path);

    void install();
    void install(const QString& path);

    void launch();
    void launch(const QString& path);

    void makeDefault();
    void makeDefault(const QString& path);

    void removeDefault();

signals:
    void errorOccurred(const QString& error);
    void currentDefaultChanged();

private:
    static void exitWithError(const QString& msg);

    bool m_hasAppController;
    QString m_newVersionPath;
};

#endif // APPLICATIONCONTROLLER_H
