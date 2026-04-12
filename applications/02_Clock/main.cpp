#include <System>
#include <Solid>
#include <Axion>
// #include <QtWebView>

int main(int argc, char *argv[])
{
    // QtWebView::initialize();
    Application *app = new Application(argc, argv, "02_Clock");

    ApplicationController::init();
    LocaleSettings::init();

    return app->run("L02_Clock", "AW02_Clock");
}
