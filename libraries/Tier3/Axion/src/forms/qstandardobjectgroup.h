#ifndef QSTANDARDOBJECTGROUP_H
#define QSTANDARDOBJECTGROUP_H

#include <QDefs>
#include "qstandardobjectmodel.h"

class QStandardObjectGroup: public QObject,
                            public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(StandardObjectGroup)
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)
    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_WRITABLE_VAR_PROPERTY(bool, enabledWarning, EnabledWarning, false)
    Q_WRITABLE_VAR_PROPERTY(bool, enabledDirty, EnabledDirty, false)

    Q_READONLY_VAR_PROPERTY(int, visibleCount, VisibleCount, 0)
    Q_WRITABLE_VAR_PROPERTY(bool, warning, Warning, false)
    Q_READONLY_VAR_PROPERTY(bool, isDirty, IsDirty, false)

public:
    explicit QStandardObjectGroup(QObject* parent=nullptr);

    void classBegin() override;
    void componentComplete() override;

    int count() const { return m_objects.count(); };
    int size() const { return count(); };
    int length() const { return count(); };
    bool isEmpty() const { return count() == 0; };

public slots:
    void addObject(QObject* object);
    void removeObject(QObject* object);

    void queueInvalidate();
    void invalidate();

signals:
    void aboutToBeInvalidated();
    void invalidated();

    void countChanged(int count);
    void emptyChanged(bool empty);

protected slots:
    void onObjectDestroyed(QObject* item);
    void countInvalidate();

    void onEnabledWarningChanged(bool enabledWarning);
    void onEnabledDirtyChanged(bool enabledDirty);

private:
    void referenceObject(QStandardObject* object);
    void dereferenceObject(QStandardObject* object);

    QList<QStandardObject*> m_objects;

    bool m_completed=false;
    int m_count=0;
    bool m_invalidateQueued = false;
};

#endif // QSTANDARDOBJECTGROUP_H
