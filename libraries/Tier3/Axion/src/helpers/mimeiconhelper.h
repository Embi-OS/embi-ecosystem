#ifndef MIMEICONHELPER_H
#define MIMEICONHELPER_H

#include <QDefs>
#include <QUtils>
#include <QColor>

class MimeIconHelper : public QObject,
                       public QQmlSingleton<MimeIconHelper>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

protected:
    friend QQmlSingleton<MimeIconHelper>;
    explicit MimeIconHelper(QObject *parent = nullptr);

public:
    Q_INVOKABLE QString getSvgIconPathForUrl(const QString& url, bool isOpen=false) const;
    Q_INVOKABLE QColor getIconColorForUrl(const QString& url) const;
    Q_INVOKABLE QString getThemeIconNameForUrl(const QString& url) const;
};

#endif // MIMEICONHELPER_H
