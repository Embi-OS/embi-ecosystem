#ifndef QTRANSLATORLOADER_H
#define QTRANSLATORLOADER_H

#include <QQmlEngine>
#include <QObject>
#include <QTranslator>
#include <QDefs>

class QTranslatorLoader : public QObject,
                          public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(TranslatorLoader)
    Q_INTERFACES(QQmlParserStatus)

    Q_WRITABLE_VAR_PROPERTY(QString, language, Language, "")

public:
    explicit QTranslatorLoader(QObject* parent=nullptr);
    ~QTranslatorLoader() override;

    void classBegin() override;
    void componentComplete() override;

    const QStringList getFileNames(const QString langage) const;
    static QStringList availableLanguages();
    static QString normalizedLanguage(const QString& language);

signals:
    void translationReloaded();

protected slots:
    void onLanguageChanged();
    bool loadTranslations();
    bool loadFile(const QString &fileName);

private:
    bool clearTranslations();

    QHash<QString,QTranslator*> m_translators;

    bool m_completed=false;
};

#endif // QTRANSLATORLOADER_H
