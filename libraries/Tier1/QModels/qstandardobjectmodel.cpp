#include "qstandardobjectmodel.h"
#include "qobjectlistmodeliterator.h"

QStandardObject::QStandardObject(QObject* parent):
    QTreeObject(parent)
{

}

QStandardObject* QStandardObject::wrap(const QObject* object)
{
    return qobject_cast<QStandardObject*>(qmlAttachedPropertiesObject<QStandardObject>(object, true));
}

QStandardObject* QStandardObject::qmlAttachedProperties(QObject* object)
{
    if (!object)
    {
        QMODELSLOG_CRITICAL()<<object;
        QMODELSLOG_FATAL("QStandardObject must be attached to a QObject*");
        return nullptr;
    }

    QStandardObject* standardObject = qobject_cast<QStandardObject*>(object);
    if (standardObject)
    {
        QMODELSLOG_DEBUG("QStandardObject is attached to a QStandardObject*");
        return standardObject;
    }

    QStandardObject* helper = new QStandardObject(object);
    QQmlEngine::setObjectOwnership(helper, QQmlEngine::CppOwnership);

    return helper;
}

QStandardObject* QStandardObject::colorObject(const QColor& color, const QString& text, QObject* parent)
{
    QStandardObject* object = new QStandardObject(parent);
    object->setText(text);
    object->setColor(color);
    return object;
}

QStandardObject* QStandardObject::fontObject(const QFont& font, const QString& text, QObject* parent)
{
    QStandardObject* object = new QStandardObject(parent);
    object->setText(text);
    object->setFont(font);
    return object;
}

QStandardObject* QStandardObject::valueObject(const QVariant& value, const QString& text, QObject* parent)
{
    QStandardObject* object = new QStandardObject(parent);
    object->setText(text);
    object->setValue(value);
    return object;
}

void QStandardObject::emitAction()
{
    emit this->action();
}

void QStandardObject::toggle()
{
    if(isEmpty())
        return;

    if(m_unfolded)
        collapse();
    else
        expand();
}

void QStandardObject::expand()
{
    setUnfolded(true);
}

void QStandardObject::collapse()
{
    if(!m_unfolded)
        return;

    for(QTreeObject* child: m_content)
    {
        if(QStandardObject* object = qobject_cast<QStandardObject*>(child))
            object->collapse();
    }

    setUnfolded(false);
}

QStandardObjectModel::QStandardObjectModel(QObject* parent,
                                           const QMetaObject* metaObject,
                                           const QByteArrayList& exposedRoles,
                                           const QByteArrayList& hiddenRoles) :
    QObjectTreeModel(parent, metaObject, exposedRoles, hiddenRoles)
{

}

QStandardObjectModel* QStandardObjectModel::colorModel(QObject* parent)
{
    QStandardObjectModel* model = new QStandardObjectModel(parent,
                                                           &QStandardObject::staticMetaObject,
                                                           {"text", "color"});
    return model;
}

QStandardObjectModel* QStandardObjectModel::fontModel(QObject* parent)
{
    QStandardObjectModel* model = new QStandardObjectModel(parent,
                                                           &QStandardObject::staticMetaObject,
                                                           {"text", "font"});
    return model;
}

QStandardObjectModel* QStandardObjectModel::valueModel(QObject* parent)
{
    QStandardObjectModel* model = new QStandardObjectModel(parent,
                                                           &QStandardObject::staticMetaObject,
                                                           {"text", "value"});
    return model;
}

QString QStandardObjectModel::textAt(int index)
{
    if(index<0 || index>=size())
        return QString();

    return at<QStandardObject>(index)->getText();
}

QColor QStandardObjectModel::colorAt(int index)
{
    if(index<0 || index>=size())
        return QColor();

    return at<QStandardObject>(index)->getColor();
}

QFont QStandardObjectModel::fontAt(int index)
{
    if(index<0 || index>=size())
        return QFont();

    return at<QStandardObject>(index)->getFont();
}

QVariant QStandardObjectModel::valueAt(int index)
{
    if(index<0 || index>=size())
        return QVariant();

    return at<QStandardObject>(index)->getValue();
}

int QStandardObjectModel::indexOfText(const QString& text)
{
    for(const QStandardObject* object: modelIterator<QStandardObject>())
    {
        if(object->getText()==text)
            return indexOf(object);
    }
    return -1;
}

int QStandardObjectModel::indexOfColor(const QColor& color)
{
    for(const QStandardObject* object: modelIterator<QStandardObject>())
    {
        if(object->getColor()==color)
            return indexOf(object);
    }
    return -1;
}

int QStandardObjectModel::indexOfFont(const QFont& font)
{
    for(const QStandardObject* object: modelIterator<QStandardObject>())
    {
        if(object->getFont()==font)
            return indexOf(object);
    }
    return -1;
}

int QStandardObjectModel::indexOfValue(const QVariant& value)
{
    for(const QStandardObject* object: modelIterator<QStandardObject>())
    {
        if(object->getValue()==value)
            return indexOf(object);
    }
    return -1;
}

QString QStandardObjectModel::textOfColor(const QColor& color)
{
    for(const QStandardObject* object: modelIterator<QStandardObject>())
    {
        if(object->getColor()==color)
            return object->getText();
    }
    return QString();
}

QString QStandardObjectModel::textOfFont(const QFont& font)
{
    for(const QStandardObject* object: modelIterator<QStandardObject>())
    {
        if(object->getFont()==font)
            return object->getText();
    }
    return QString();
}

QString QStandardObjectModel::textOfValue(const QVariant& value)
{
    for(const QStandardObject* object: modelIterator<QStandardObject>())
    {
        if(object->getValue()==value)
            return object->getText();
    }
    return QString();
}
