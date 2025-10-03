#ifndef TEXTFILE_H
#define TEXTFILE_H

#include <QFile>
#include <QSaveFile>

#include <QDefs>

Q_ENUM_CLASS(TextFileOpenModes, TextFileOpenMode,
             Read,
             Write,
             ReadWrite,)

class TextFile : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_READONLY_REF_PROPERTY(QUrl, fileUrl, FileUrl, {})
    Q_READONLY_VAR_PROPERTY(bool, isOpen, Open, false)
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileUrlChanged FINAL)
    Q_PROPERTY(QString fileType READ fileType NOTIFY fileUrlChanged FINAL)
    Q_PROPERTY(QString content READ readAll NOTIFY contentChanged FINAL)
    Q_READONLY_REF_PROPERTY(QString, error, Error, "")

public:
    explicit TextFile(QObject* parent = nullptr);

    QString fileName() const;
    QString fileType() const;

public slots:
    bool open(const QString& path, int mode);
    void close();

    bool write(const QString& string);
    QString readAll();

signals:
    void contentChanged();

private:
    QSaveFile* m_writeFile = nullptr;
    QFile* m_readFile = nullptr;

    QFileDevice* fileDevice() const;

    bool createAndOpenFile(const QString& file, QFileDevice::OpenMode mode);
    void closeAndDeleteFile();
};

#endif // TEXTFILE_H
