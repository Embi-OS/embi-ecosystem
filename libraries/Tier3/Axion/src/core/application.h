#ifndef APPLICATION_H
#define APPLICATION_H

#include <QDefs>
#include <QUtils>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

class Application
{
public:
    Application(int &argc, char **argv, const QString& applicationName=QString());
    ~Application();

    int run(QAnyStringView uri, QAnyStringView typeName);

    static void putEnvironmentVariable(const char *varName, QByteArrayView value);
    static void unsetEnvironmentVariable(const char *varName);
    static const QMap<QString, QByteArray>& environmentVariable();

private:
    void installTranslators();

    QScopedPointer<QApplication> m_application;
    QScopedPointer<QQmlApplicationEngine> m_engine;
};

#endif // APPLICATION_H
