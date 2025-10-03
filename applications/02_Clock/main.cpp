#include <System>
#include <Solid>
#include <Axion>
#include <Swupdate>

int main(int argc, char *argv[])
{
    Application *app = new Application(argc, argv, "02_Clock");

    ApplicationController::init();
    LocaleSettings::init();
    Swupdate::init();

    AxionHelper::onRestartAccepted(Power::restart);
    AxionHelper::onRebootAccepted(Power::reboot);

    return app->run("L02_Clock", "AW02_Clock");
}
