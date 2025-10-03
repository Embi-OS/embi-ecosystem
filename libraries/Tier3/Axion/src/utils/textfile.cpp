#include "textfile.h"
#include "axion_log.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QLoggingCategory>
#include <QtQml/QQmlFile>

TextFile::TextFile(QObject* parent)
    : QObject(parent)
{
    connect(this, &TextFile::errorChanged, this, [](const QString& value)
    {
        if(!value.isEmpty())
        {
            AXIONLOG_WARNING()<<"File Error : "<<value;
        }
    });
}

QString TextFile::fileName() const
{
    const QString filePath = QQmlFile::urlToLocalFileOrQrc(m_fileUrl);
    auto fileName = QFileInfo(filePath).fileName();
    if(fileName.isEmpty())
        return "untitled.txt";
    return fileName;
}

QString TextFile::fileType() const
{
    return QFileInfo(fileName()).suffix();
}

bool TextFile::open(const QString& path, int mode)
{
    AXIONLOG_DEBUG()<<"open file : "<<path;

    const auto fileInfo = QFileInfo(path);
    QUrl url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());

    // Make sure url is valid as a local file
    if(url.isRelative())
        url = "file:" + url.toString();
    setFileUrl(url);
    const auto localFile = url.toLocalFile();

    if(localFile.isEmpty())
    {
        if(url.isEmpty())
            setError("filename is empty");
        else
            setError(QString("Filename \"%1\" isn't a valid file url").arg(url.toString()));
        return false;
    }

    const auto qOpenMode = [&]()
    {
        switch(TextFileOpenMode(mode))
        {
        case TextFileOpenModes::Read:
            return QFile::OpenModeFlag::ReadOnly;
        case TextFileOpenModes::Write:
            return QFile::OpenModeFlag::WriteOnly;
        default:
            return QFile::OpenModeFlag::ReadWrite;
        }
    }() | QFile::OpenModeFlag::Text;

    if(!createAndOpenFile(localFile, qOpenMode))
        return false;

    setError("");
    emit this->contentChanged();
    return true;
}

void TextFile::close()
{
    if(!fileDevice())
    {
        AXIONLOG_WARNING()<<"Fail to close file that is not opened";
        return;
    }

    if(m_writeFile)
        m_writeFile->commit();
    closeAndDeleteFile();
    setError("");
}

bool TextFile::write(const QString& string)
{
    if(!getOpen())
    {
        setError("File is close");
        return false;
    }

    const auto buffer = string.toUtf8();
    if(const auto bytesWritten = fileDevice()->write(buffer); bytesWritten != buffer.length())
    {
        setError(fileDevice()->errorString());
        closeAndDeleteFile();
        return false;
    }

    setError("");
    emit this->contentChanged();
    return true;
}

QString TextFile::readAll()
{
    if(!getOpen())
    {
        setError("File is close");
        return "";
    }

    const auto buffer = fileDevice()->readAll();

    if(auto* f = fileDevice(); f && f->error())
    {
        setError(fileDevice()->errorString());
        closeAndDeleteFile();
        return "";
    }

    setError("");
    return QString(buffer);
}

QFileDevice* TextFile::fileDevice() const
{
    if(m_writeFile)
        return m_writeFile;
    return m_readFile;
}

bool TextFile::createAndOpenFile(const QString& file, QFileDevice::OpenMode mode)
{
    if(fileDevice())
    {
        AXIONLOG_DEBUG()<<"Close file"<<fileName()<<"before opening"<<file;
        closeAndDeleteFile();
    }

    if(mode & QFile::OpenModeFlag::ReadOnly)
        m_readFile = new QFile(file, this);
    else
        m_writeFile = new QSaveFile(file, this);

    if(const auto success = fileDevice()->open(mode); !success)
    {
        setError(fileDevice()->errorString());
        closeAndDeleteFile();
        return false;
    }
    setOpen(true);
    return true;
}

void TextFile::closeAndDeleteFile()
{
    setOpen(false);
    if(fileDevice())
        fileDevice()->deleteLater();

    m_writeFile = nullptr;
    m_readFile = nullptr;
}
