#include "version.h"
#include "axion_log.h"

Version::Version(QObject *parent) :
    QObject(parent),
    m_globalVersion(PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH),
    m_versionSuffix(PROJECT_DESCRIPTION),
    m_company(PROJECT_COMPANY),
    m_website(PROJECT_WEBSITE),
    m_copyright(PROJECT_COPYRIGHT),
    m_version(m_versionSuffix.isEmpty() ? QString("%1").arg(m_globalVersion.toString()) : QString("%1 [%2]").arg(m_globalVersion.toString(),m_versionSuffix)),
    m_productName(PROJECT_NAME)
{

}

void Version::dumpInfos() const
{
    QString infos;
    infos += QString("\n\n");
    infos += m_productName.isEmpty() ? "" : QString("\t %1\n").arg(m_productName);
    infos += m_version.isEmpty() ? "" : QString("\t Version: %1\n").arg(m_version);
    infos += m_company.isEmpty() ? "" : QString("\t Company: %1\n").arg(m_company);
    infos += m_website.isEmpty() ? "" : QString("\t Website: %1\n").arg(m_website);
    infos += m_copyright.isEmpty() ? "" : QString("\t %1\n").arg(m_copyright);
    AXIONLOG_INFO().noquote()<<infos;
}

QString Version::about() const
{
    QStringList about;
    if(!m_productName.isEmpty())
        about += tr("Produit: %1").arg(m_productName);
    if(!m_version.isEmpty())
        about += tr("Version: %1").arg(m_version);
    about += tr("Version Qt: %1").arg(qVersion());
    if(!m_company.isEmpty())
        about += tr("Entreprise: %1").arg(m_company);
    if(!m_website.isEmpty())
        about += tr("Site web: %1").arg(m_website);
    if(!m_copyright.isEmpty())
        about += tr("Copyright: %1").arg(m_copyright);
    return about.join("\n");
}

QString Version::aboutQt() const
{
    QString about = QString(
                        "<h2>This program runs on Qt version %1</h2>"
                        "<p>It was built against Qt version %2</p>"
                        "<p></p>"
                        "<p>Qt is a C++ toolkit for cross-platform application development.</p>"
                        "<p>Qt provides single-source portability across all major desktop "
                        "operating systems. It is also available for embedded Linux and other "
                        "embedded and mobile operating systems.</p>"
                        ).arg(qVersion(), QT_VERSION_STR);
    return about;
}

QString Version::versionToString(const QVersionNumber& version)
{
    return version.toString();
}
