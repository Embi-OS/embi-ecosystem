#ifndef QQUICITEMCONTAINER_H
#define QQUICITEMCONTAINER_H

#include <QDefs>
#include <QQuickItem>

class QQuickItemContainerAttached;
class QQuickItemContainer: public QObject,
                           public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ItemContainer)
    QML_ATTACHED(QQuickItemContainerAttached)
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)
    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_WRITABLE_VAR_PROPERTY(bool, enabledHeight, EnabledHeight, false)
    Q_WRITABLE_VAR_PROPERTY(bool, enabledWidth, EnabledWidth, false)

    Q_READONLY_VAR_PROPERTY(int, visibleCount, VisibleCount, 0)
    Q_READONLY_VAR_PROPERTY(double, maxImplicitHeight, MaxImplicitHeight, 0)
    Q_READONLY_VAR_PROPERTY(double, totalImplicitHeight, TotalImplicitHeight, 0)
    Q_READONLY_VAR_PROPERTY(double, maxImplicitWidth, MaxImplicitWidth, 0)
    Q_READONLY_VAR_PROPERTY(double, totalImplicitWidth, TotalImplicitWidth, 0)

public:
    explicit QQuickItemContainer(QObject* parent=nullptr);

    void classBegin() override;
    void componentComplete() override;

    static QQuickItemContainerAttached *qmlAttachedProperties(QObject *object);

    int count() const { return m_items.count(); };
    int size() const { return count(); };
    int length() const { return count(); };
    bool isEmpty() const { return count() == 0; };

public slots:
    void addItem(QQuickItem* item);
    void removeItem(QQuickItem* item);

    void queueInvalidate();
    void invalidate();

signals:
    void aboutToBeInvalidated();
    void invalidated();

    void countChanged(int count);
    void emptyChanged(bool empty);

    void itemAdded(QQuickItem* item);
    void itemRemoved(QQuickItem* item);

protected slots:
    void onObjectDestroyed(QObject* item);
    void countInvalidate();

    void onEnabledHeightChanged(bool enabledHeight);
    void onEnabledWidthChanged(bool enabledWidth);

private:
    void referenceObject(QQuickItem* item);
    void dereferenceObject(QQuickItem* item);

    QList<QQuickItem*> m_items;

    bool m_completed=false;
    int m_count=0;
    bool m_invalidateQueued = false;
};

class QQuickItemContainerAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQuickItemContainer *container READ getContainer WRITE setContainer NOTIFY containerChanged FINAL)

protected:
    friend QQuickItemContainer;
    explicit QQuickItemContainerAttached(QObject *parent = nullptr);

public:
    static QQuickItemContainerAttached* wrap(const QObject* object);

    QQuickItemContainer* getContainer() const;
    void setContainer(QQuickItemContainer* container);

signals:
    void containerChanged(QQuickItemContainer* container);

private:
    QQuickItem* m_item;
    QQuickItemContainer* m_container;
};

#endif // QQUICITEMCONTAINER_H
