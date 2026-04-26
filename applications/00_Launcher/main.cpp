#include <Solid>
#include <Axion>

int main(int argc, char *argv[])
{
    Application *app = new Application(argc, argv, "00_Launcher");

    ApplicationController::init(false);
    LocaleSettings::init();

    AxionHelper::onApplicationLoaded(Swupdate::init);

    return app->run("L00_Launcher", "AW00_Launcher");
}
