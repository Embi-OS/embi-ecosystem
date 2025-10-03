#ifndef REGEXPUTILS_H
#define REGEXPUTILS_H

#include <QDefs>
#include <QColor>
#include "qsingleton.h"

class RegExpUtils : public QObject,
                    public QQmlSingleton<RegExpUtils>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QRegularExpression qrcRegExp READ qrcRegExp CONSTANT FINAL)
    Q_PROPERTY(QRegularExpression markdownRegExp READ markdownRegExp CONSTANT FINAL)
    Q_PROPERTY(QRegularExpression htmlRegExp READ htmlRegExp CONSTANT FINAL)
    Q_PROPERTY(QRegularExpression jsonRegExp READ jsonRegExp CONSTANT FINAL)
    Q_PROPERTY(QRegularExpression xmlRegExp READ xmlRegExp CONSTANT FINAL)
    Q_PROPERTY(QRegularExpression charRegExp READ charRegExp CONSTANT FINAL)
    Q_PROPERTY(QRegularExpression alphanumericsRegExp READ alphanumericsRegExp CONSTANT FINAL)
    Q_PROPERTY(QRegularExpression numberRegExp READ numberRegExp CONSTANT FINAL)
    Q_PROPERTY(QRegularExpression uncPathRegExp READ uncPathRegExp CONSTANT FINAL)
    Q_PROPERTY(QRegularExpression pathRegExp READ pathRegExp CONSTANT FINAL)
    Q_PROPERTY(QRegularExpression nonSpaceRegExp READ nonSpaceRegExp CONSTANT FINAL)

protected:
    friend QQmlSingleton<RegExpUtils>;
    explicit RegExpUtils(QObject *parent = nullptr);

public:
    static const QRegularExpression& qrcRegExp();
    static const QRegularExpression& markdownRegExp();
    static const QRegularExpression& htmlRegExp();
    static const QRegularExpression& jsonRegExp();
    static const QRegularExpression& xmlRegExp();
    static const QRegularExpression& charRegExp();
    static const QRegularExpression& alphanumericsRegExp();
    static const QRegularExpression& numberRegExp();
    static const QRegularExpression& uncPathRegExp();
    static const QRegularExpression& pathRegExp();
    static const QRegularExpression& nonSpaceRegExp();
};

#endif // REGEXPUTILS_H
