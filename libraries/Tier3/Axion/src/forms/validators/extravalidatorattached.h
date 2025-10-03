#ifndef EXTRAVALIDATORATTACHED_H
#define EXTRAVALIDATORATTACHED_H

#include <QDefs>
#include <QValidator>

class ExtraValidatorAttached : public QObject 
{
    Q_OBJECT
    QML_ATTACHED(ExtraValidatorAttached)
    QML_UNCREATABLE("Attached")
    QML_NAMED_ELEMENT(ExtraValidator)

    Q_PROPERTY(QString locale READ localeName WRITE setLocaleName RESET resetLocaleName NOTIFY localeNameChanged FINAL)
    Q_WRITABLE_REF_PROPERTY(QString, errorMessage, ErrorMessage, "")

public:
    static ExtraValidatorAttached* wrap(const QObject* object);
    static ExtraValidatorAttached* qmlAttachedProperties(QObject* object);

    QString localeName() const;
    void setLocaleName(const QString &name);
    void resetLocaleName();

signals:
    void localeNameChanged();

private:
    explicit ExtraValidatorAttached(QValidator* parent);

    QValidator* m_validator=nullptr;
};

#endif // EXTRAVALIDATORATTACHED_H
