#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    engine.loadFromModule("L03_Life", "AW03_Life");
    if (engine.rootObjects().isEmpty())
        return -1;

    return QGuiApplication::exec();
}
