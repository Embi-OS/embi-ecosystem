#include "axion_helpertypes.h"
#include "axion_log.h"

#include "version.h"
#include "dialogmanager.h"
#include "snackbarmanager.h"

#include "variantimageprovider.h"
#include "qrcodeimageprovider.h"

#include <QFontDatabase>
#include <QDirIterator>

TextTheme::TextTheme(QObject* parent) :
    QObject(parent)
{
    headline1.setFamily(m_primaryFont);
    headline1.setPixelSize(96);
    headline1.setWeight(QFont::Weight::Thin);

    headline2.setFamily(m_primaryFont);
    headline2.setPixelSize(60);
    headline2.setWeight(QFont::Weight::Thin);

    headline3.setFamily(m_primaryFont);
    headline3.setPixelSize(48);
    headline3.setWeight(QFont::Weight::Light);

    headline4.setFamily(m_primaryFont);
    headline4.setPixelSize(34);
    headline4.setWeight(QFont::Weight::Light);

    headline5.setFamily(m_primaryFont);
    headline5.setPixelSize(24);
    headline5.setWeight(QFont::Weight::Normal);

    headline6.setFamily(m_primaryFont);
    headline6.setPixelSize(20);
    headline6.setWeight(QFont::Weight::Normal);

    headline7.setFamily(m_primaryFont);
    headline7.setPixelSize(18);
    headline7.setWeight(QFont::Weight::Normal);

    headline8.setFamily(m_primaryFont);
    headline8.setPixelSize(16);
    headline8.setWeight(QFont::Weight::Normal);

    title1.setFamily(m_primaryFont);
    title1.setPixelSize(32);
    title1.setWeight(QFont::Weight::DemiBold);

    title2.setFamily(m_primaryFont);
    title2.setPixelSize(24);
    title2.setWeight(QFont::Weight::DemiBold);

    subtitle1.setFamily(m_primaryFont);
    subtitle1.setPixelSize(18);
    subtitle1.setWeight(QFont::Weight::Medium);

    subtitle2.setFamily(m_primaryFont);
    subtitle2.setPixelSize(16);
    subtitle2.setWeight(QFont::Weight::Medium);

    body1.setFamily(m_primaryFont);
    body1.setPixelSize(16);
    body1.setWeight(QFont::Weight::Normal);

    body2.setFamily(m_primaryFont);
    body2.setPixelSize(14);
    body2.setWeight(QFont::Weight::Normal);

    button.setFamily(m_primaryFont);
    button.setPixelSize(18);
    button.setWeight(QFont::Weight::Bold);

    capital.setFamily(m_primaryFont);
    capital.setPixelSize(16);
    capital.setWeight(QFont::Weight::Bold);
    capital.setCapitalization(QFont::Capitalization::AllUppercase);

    caption1.setFamily(m_primaryFont);
    caption1.setPixelSize(12);
    caption1.setWeight(QFont::Weight::Normal);

    caption2.setFamily(m_primaryFont);
    caption2.setPixelSize(8);
    caption2.setWeight(QFont::Weight::Normal);

    overline.setFamily(m_primaryFont);
    overline.setPixelSize(12);
    overline.setWeight(QFont::Weight::Normal);
    overline.setCapitalization(QFont::Capitalization::AllUppercase);
    overline.setLetterSpacing(QFont::SpacingType::AbsoluteSpacing, 1.5);

    hint1.setFamily(m_primaryFont);
    hint1.setPixelSize(11);
    hint1.setWeight(QFont::Weight::Medium);

    hint2.setFamily(m_primaryFont);
    hint2.setPixelSize(11);
    hint2.setWeight(QFont::Weight::Normal);

    code.setFamily("Monospace");
    code.setPixelSize(14);
    code.setWeight(QFont::Weight::Normal);

    connect(this, &TextTheme::primaryFontChanged, this, [this](){
        headline1.setFamily(m_primaryFont);
        headline2.setFamily(m_primaryFont);
        headline3.setFamily(m_primaryFont);
        headline4.setFamily(m_primaryFont);
        headline5.setFamily(m_primaryFont);
        headline6.setFamily(m_primaryFont);
        headline7.setFamily(m_primaryFont);
        headline8.setFamily(m_primaryFont);
        title1.setFamily(m_primaryFont);
        title2.setFamily(m_primaryFont);
        subtitle1.setFamily(m_primaryFont);
        subtitle2.setFamily(m_primaryFont);
        body1.setFamily(m_primaryFont);
        body2.setFamily(m_primaryFont);
        button.setFamily(m_primaryFont);
        caption1.setFamily(m_primaryFont);
        caption2.setFamily(m_primaryFont);
        overline.setFamily(m_primaryFont);
        hint1.setFamily(m_primaryFont);
        hint2.setFamily(m_primaryFont);

        emit this->familyChanged();
    });
}

AxionHelper::AxionHelper(QObject *parent) :
    QObject(parent)
{
    const QStringList args = QCoreApplication::arguments();
    QVariantMap arguments;
    for(const QString& arg: args)
    {
        if(!arg.contains('='))
            continue;
        QStringList lineSplit = arg.split("=");
        const QString key = lineSplit.takeFirst();
        const QVariant value = lineSplit.join("=");
        arguments.insert(key, value);
    }

    m_arguments = arguments;

    AXIONLOG_TRACE().noquote()<<QUtils::Log::variantToLog(arguments);
}

void AxionHelper::parseArguments()
{
    Get();
}

void AxionHelper::registerComponents(QQmlEngine& engine)
{
    AxionHelper::registerImageProvider(engine);
    AxionHelper::registerFontsPath();

#if defined(Q_DEVICE_APALIS_IMX8) && defined(Q_MANUAL_CORE_AFFINITY)
    QUtils::setAppMainCoreAffinity(4);
#endif
}

void AxionHelper::registerImageProvider(QQmlEngine& engine)
{
    engine.addImageProvider(QLatin1String("VariantImage"), new VariantImageProvider());
    engine.addImageProvider(QLatin1String("QrCodeImage"), new QrCodeImageProvider());
}

void AxionHelper::registerFontsPath(const QString& path)
{
    QDirIterator lister(path, QDir::Files, QDirIterator::Subdirectories);
    while(lister.hasNext())
    {
        const QString fileUrl = lister.next();
        if(QFontDatabase::addApplicationFont(fileUrl) >= 0)
            AXIONLOG_DEBUG() << "Load font" << fileUrl;
        else
            AXIONLOG_WARNING() << "Fail to load font" << fileUrl;
    }
}

void AxionHelper::warningWrongPage(const QString& page, int index)
{
    QVariantMap settings;
    settings["title"] = tr("Page inexistante!");
    settings["caption"] = QString("%1 - %2").arg(page).arg(index);
    settings["timeout"] = 2000;
    SnackbarManager::Get()->showWarning(settings);
}

void AxionHelper::showAbout()
{
    QVariantMap settings;
    settings["message"] = tr("A propos!");
    settings["infos"] = Version::Get()->about();
    settings["diagnose"] = false;
    DialogManager::Get()->showMessage(settings);
}

void AxionHelper::showAboutQt()
{
    QVariantMap settings;
    settings["message"] = tr("A propos de Qt!");
    settings["infos"] = Version::Get()->aboutQt();
    settings["diagnose"] = false;
    DialogManager::Get()->showMessage(settings);
}

void AxionHelper::warningRestart(const QString& title, const QString& details)
{
    if(AxionHelper::Get()->getPendingRestart())
        return;
    AxionHelper::markForRestart();

    QVariantMap settings;
    settings["title"] = title;
    settings["caption"] = tr("L'application doit être relancée pour appliquer tous les changements");
    settings["details"] = details;
    settings["button"] = tr("Relancer");
    SnackbarObject* snackbar = SnackbarManager::Get()->showWarning(settings);
    snackbar->onAccepted([](){
        emit AxionHelper::Get()->restartAccepted();
    });
}

void AxionHelper::warningReboot(const QString& title, const QString& details)
{
    if(AxionHelper::Get()->getPendingReboot())
        return;
    AxionHelper::markForReboot();

    QVariantMap settings;
    settings["title"] = title;
    settings["caption"] = tr("Le système doit redémarrer pour appliquer tous les changements");
    settings["details"] = details;
    settings["button"] = tr("Redémarrer");
    SnackbarObject* snackbar = SnackbarManager::Get()->showWarning(settings);
    snackbar->onAccepted([](){
        emit AxionHelper::Get()->rebootAccepted();
    });
}

void AxionHelper::criticalRestart(const QString& message, const QString& traces)
{
    AxionHelper::markForRestart();

    QVariantMap settings;
    settings["message"] = message;
    settings["infos"] = tr("L'application doit être relancée pour appliquer tous les changements");
    settings["traces"] = traces;
    settings["buttonAccept"] = tr("Relancer");
    DialogObject* dialog = DialogManager::Get()->showWarning(settings);
    dialog->onAccepted([]() mutable {
        emit AxionHelper::Get()->restartAccepted();
    });
}

void AxionHelper::criticalReboot(const QString& message, const QString& traces)
{
    AxionHelper::markForReboot();

    QVariantMap settings;
    settings["message"] = message;
    settings["infos"] = tr("Le système doit redémarrer pour appliquer tous les changements");
    settings["traces"] = traces;
    settings["buttonAccept"] = tr("Redémarrer");
    DialogObject* dialog = DialogManager::Get()->showWarning(settings);
    dialog->onAccepted([](){
        emit AxionHelper::Get()->rebootAccepted();
    });
}

bool AxionHelper::markForRestart()
{
    AXIONLOG_INFO()<<"Application has been mark for restart";
    return Get()->setPendingRestart(true);
}

bool AxionHelper::markForReboot()
{
    AXIONLOG_INFO()<<"System has been mark for reboot";
    return Get()->setPendingReboot(true);
}

QMetaObject::Connection AxionHelper::onRestartAccepted(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(AxionHelper::Get(), &AxionHelper::restartAccepted, AxionHelper::Get(), [callback](){
        callback();
    }, connection);
}

QMetaObject::Connection AxionHelper::onRebootAccepted(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(AxionHelper::Get(), &AxionHelper::rebootAccepted, AxionHelper::Get(), [callback](){
        callback();
    }, connection);
}
