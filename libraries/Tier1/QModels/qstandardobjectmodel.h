#ifndef QSTANDARDOBJECTMODEL_H
#define QSTANDARDOBJECTMODEL_H

#include "qobjecttreemodel.h"
#include <QDefs>

#include <QColor>
#include <QFont>

class QStandardObject: public QTreeObject
{
    Q_OBJECT
    QML_ATTACHED(QStandardObject)
    QML_NAMED_ELEMENT(StandardObject)

    Q_WRITABLE_PTR_PROPERTY(QQmlComponent, delegate, Delegate, nullptr)
    Q_WRITABLE_VAR_PROPERTY(bool, unfolded, Unfolded, false)

    Q_WRITABLE_VAR_PROPERTY(bool, visible, Visible, true)
    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_WRITABLE_VAR_PROPERTY(bool, editable, Editable, true)
    Q_WRITABLE_VAR_PROPERTY(bool, highlighted, Highlight, false)
    Q_WRITABLE_VAR_PROPERTY(bool, warning, Warning, false)
    Q_WRITABLE_VAR_PROPERTY(bool, asynchronous, Asynchronous, true)
    Q_WRITABLE_VAR_PROPERTY(bool, isDirty, IsDirty, false)

    Q_WRITABLE_REF_PROPERTY(QString, text, Text, "")
    Q_WRITABLE_REF_PROPERTY(QString, hint, Hint, "")
    Q_WRITABLE_REF_PROPERTY(QString, icon, Icon, "")

    Q_WRITABLE_REF_PROPERTY(QColor, color, Color, {})
    Q_WRITABLE_REF_PROPERTY(QFont, font, Font, {})
    Q_WRITABLE_REF_PROPERTY(QVariant, value, Value, {})

    Q_WRITABLE_REF_PROPERTY(QString, group, Group, "")

public:
    explicit QStandardObject(QObject* parent = nullptr);

    static QStandardObject* wrap(const QObject* object);
    static QStandardObject* qmlAttachedProperties(QObject* object);

    static QStandardObject* colorObject(const QColor& color, const QString& text="", QObject* parent = nullptr);
    static QStandardObject* fontObject(const QFont& font, const QString& text="", QObject* parent = nullptr);
    static QStandardObject* valueObject(const QVariant& value, const QString& text="", QObject* parent = nullptr);

public slots:
    void emitAction();

    void toggle();
    void expand();
    void collapse();

signals:
    void action();
};

class QStandardObjectModel: public QObjectTreeModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(StandardObjectModel)

public:
    explicit QStandardObjectModel(QObject* parent = nullptr,
                                  const QMetaObject* metaObject = &QStandardObject::staticMetaObject,
                                  const QByteArrayList& exposedRoles = {},
                                  const QByteArrayList& hiddenRoles = {});

    static QStandardObjectModel* colorModel(QObject* parent = nullptr);
    static QStandardObjectModel* fontModel(QObject* parent = nullptr);
    static QStandardObjectModel* valueModel(QObject* parent = nullptr);

    Q_INVOKABLE QString textAt(int index);
    Q_INVOKABLE QColor colorAt(int index);
    Q_INVOKABLE QFont fontAt(int index);
    Q_INVOKABLE QVariant valueAt(int index);

    Q_INVOKABLE int indexOfText(const QString& text);
    Q_INVOKABLE int indexOfColor(const QColor& color);
    Q_INVOKABLE int indexOfFont(const QFont& font);
    Q_INVOKABLE int indexOfValue(const QVariant& value);

    Q_INVOKABLE QString textOfColor(const QColor& color);
    Q_INVOKABLE QString textOfFont(const QFont& font);
    Q_INVOKABLE QString textOfValue(const QVariant& value);
};

#endif // QSTANDARDOBJECTMODEL_H
