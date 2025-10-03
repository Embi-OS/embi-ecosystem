#ifndef QPROXYOBJECT_H
#define QPROXYOBJECT_H

#include <QDefs>

class QQmlSortFilterProxyModel;

class QProxyObject : public QObject,
                     public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ProxyObject)
    Q_INTERFACES(QQmlParserStatus)

    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)

public:
    explicit QProxyObject(QObject* parent = nullptr);

    void classBegin() override {};
    void componentComplete() override {};

    virtual void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel);
    virtual void updateRoles(const QQmlSortFilterProxyModel& proxyModel);

Q_SIGNALS:
    void invalidated();
    void roleNamesInvalidated();

protected slots:
    virtual void invalidate();
};

#endif // QPROXYOBJECT_H
