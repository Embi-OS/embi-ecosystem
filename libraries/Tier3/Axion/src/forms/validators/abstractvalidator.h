#ifndef ABSTRACTVALIDATOR_H
#define ABSTRACTVALIDATOR_H

#include <QDefs>
#include <QValidator>
class AbstractValidator : public QValidator,
                          public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_INTERFACES(QQmlParserStatus)

    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)

    Q_WRITABLE_REF_PROPERTY(QString, summary, Summary, "")

public:
    explicit AbstractValidator(QObject * parent = nullptr);

    void classBegin() override;
    void componentComplete() override;

    bool isReady() const;

public slots:
    virtual void queueInvalidate();
    virtual void invalidate();

protected:
    virtual void updateSummary() {};

    bool m_completed = false;
    bool m_invalidateQueued = false;
};

class EmptyValidator : public QValidator
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit EmptyValidator(QObject *parent = nullptr) : QValidator(parent) {}
    QValidator::State validate(QString &, int &) const final override { return QValidator::Acceptable; }
};

struct QtValidatorForeign {
    Q_GADGET
    QML_FOREIGN(QValidator)
    QML_NAMED_ELEMENT(Validator)
    QML_UNCREATABLE("Abstract")
};

#endif // ABSTRACTVALIDATOR_H
