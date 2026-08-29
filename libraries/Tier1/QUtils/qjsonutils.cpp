#include "qjsonutils.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QCborValue>
#include <QCborStreamWriter>
#include <QCborStreamReader>
#include <QIODevice>
#include <QFile>
#include <QMetaProperty>
#include "qutildefs.h"

#define JSON_VARIANT_WRITING
#define JSON_VARIANT_PARSING
#define CBOR_VARIANT_WRITING
#define CBOR_VARIANT_PARSING

#include <QJsonVariant>

QByteArray QUtils::Log::variantToLog(const QVariant& variant, bool compact)
{
    return QJsonVariantWriter::fromVariantDebug(variant, compact);
}

QByteArray QUtils::Log::variantToJson(const QVariant& variant, bool compact)
{
    return QJsonVariantWriter::fromVariant(variant, compact);
}

QByteArray QUtils::Log::variantToCbor(const QVariant& variant, int opt)
{
    return QCborVariantWriter::fromVariant(variant, opt);
}

QByteArray QUtils::Json::variantToJson(const QVariant& variant, bool compact, int doublePrecision)
{
#ifdef JSON_VARIANT_WRITING
    return QJsonVariantWriter::fromVariant(variant, compact, doublePrecision);
#else
    return QJsonDocument::fromVariant(variant).toJson(compact?QJsonDocument::Compact:QJsonDocument::Indented);
#endif
}

void QUtils::Json::variantToJsonStream(const QVariant& variant, QIODevice* device, bool compact, int doublePrecision)
{
#ifdef JSON_VARIANT_WRITING
    QJsonVariantWriter::fromVariant(variant, device, compact, doublePrecision);
#else
    if (!device)
        return;
    device->write(QJsonDocument::fromVariant(variant).toJson(compact?QJsonDocument::Compact:QJsonDocument::Indented));
#endif
}

QVariant QUtils::Json::jsonToVariant(const QByteArray& json, QJsonParseError* error)
{
#ifdef JSON_VARIANT_PARSING
    return QJsonVariantReader::fromJson(json, error);
#else
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(json, error ? error : &parseError);
    if ((error ? error->error : parseError.error) != QJsonParseError::NoError)
        return QVariant();
    return document.toVariant();
#endif
}

QVariant QUtils::Json::jsonStreamToVariant(QIODevice* device, QJsonParseError* error)
{
#ifdef JSON_VARIANT_PARSING
    return QJsonVariantReader::fromJson(device, error);
#else
    if (!device) {
        if (error) {
            error->error = QJsonParseError::IllegalValue;
            error->offset = 0;
        }
        return QVariant();
    }
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(device->readAll(), error ? error : &parseError);
    if ((error ? error->error : parseError.error) != QJsonParseError::NoError)
        return QVariant();
    return document.toVariant();
#endif
}

QByteArray QUtils::Cbor::variantToCbor(const QVariant& variant, int opt)
{
#ifdef CBOR_VARIANT_WRITING
    return QCborVariantWriter::fromVariant(variant, opt);
#else
    return QCborValue::fromVariant(variant).toCbor((QCborValue::EncodingOptions)opt);
#endif
}

void QUtils::Cbor::variantToCborStream(const QVariant& variant, QIODevice* device, int opt)
{
#ifdef CBOR_VARIANT_WRITING
    return QCborVariantWriter::fromVariant(variant, device, opt);
#else
    QCborStreamWriter writer(device);
    QCborValue::fromVariant(variant).toCbor(writer, (QCborValue::EncodingOptions)opt);
#endif
}

QVariant QUtils::Cbor::cborToVariant(const QByteArray& cbor, QCborParserError* error)
{
#ifdef CBOR_VARIANT_PARSING
    return QCborVariantReader::fromCbor(cbor, error);
#else
    return QCborValue::fromCbor(cbor, error).toVariant();
#endif
}

QVariant QUtils::Cbor::cborStreamToVariant(QIODevice* device, QCborParserError* error)
{
#ifdef CBOR_VARIANT_PARSING
    return QCborVariantReader::fromCbor(device, error);
#else
    QCborStreamReader reader(device);
    QVariant variant = QCborValue::fromCbor(reader).toVariant();
    if (error) {
        error->error = reader.lastError();
        error->offset = reader.currentOffset();
    }
    return variant;
#endif
}

QVariantList QUtils::Variant::serializeSeries3D(const QVector<QVector3D>& points, int precision, const QString& xHeader, const QString& yHeader, const QString& zHeader)
{
    QVariantList pointsArray;
    pointsArray.reserve(points.size());
    for(const QVector3D &vector3D: points)
    {
        pointsArray.append(QVariantMap({{xHeader, qFuzzyRound(vector3D.x(), precision)},
                                        {yHeader, qFuzzyRound(vector3D.y(), precision)},
                                        {zHeader, qFuzzyRound(vector3D.z(), precision)}}));
    }

    return pointsArray;
}

QVariantMap QUtils::Variant::serializePointF(const QPointF& point, int precision, const QString& xHeader, const QString& yHeader)
{
    return QVariantMap({{xHeader, qFuzzyRound(point.x(), precision)},
                        {yHeader, qFuzzyRound(point.y(), precision)}});
}

QVariant QUtils::Variant::serializeDouble(double d, int precision)
{
    return QVariant(qFuzzyRound(d, precision));
}

QVector<QVector3D> QUtils::Variant::deserializeSeries3D(const QVariantList& pointsArray, const QString& xHeader, const QString& yHeader, const QString& zHeader)
{
    QVector<QVector3D> points;
    points.reserve(pointsArray.size());
    for(const QVariant &pointValue: pointsArray)
    {
        const QVariantMap& pointObject = *reinterpret_cast<const QVariantMap*>(pointValue.constData());
        points.append(QVector3D(pointObject.value(xHeader).toDouble(),
                                pointObject.value(yHeader).toDouble(),
                                pointObject.value(zHeader).toDouble()));
    }
    return points;
}

QPointF QUtils::Variant::deserializePointF(const QVariantMap& point, const QString& xHeader, const QString& yHeader)
{
    return QPointF(point.value(xHeader).toDouble(),
                   point.value(yHeader).toDouble());
}

QVariantMap QUtils::Object::objectToVariant(const QObject* object)
{
    QVariantMap values;
    if(!object)
        return values;

    const QMetaObject* metaObject = object->metaObject();
    for(int propertyIndex = 0; propertyIndex < metaObject->propertyCount(); ++propertyIndex)
    {
        const QMetaProperty property = metaObject->property(propertyIndex);
        if(!property.isReadable())
            continue;

        const QVariant value = property.read(object);
        if(value.isValid())
            values.insert(QString::fromLatin1(property.name()), value);
    }

    return values;
}

bool QUtils::Object::variantToObject(const QVariantMap& variant, QObject* object)
{
    if(!object)
        return false;

    bool result = true;
    const QMetaObject* metaObject = object->metaObject();
    for(auto [key,value]: variant.asKeyValueRange())
    {
        const QByteArray propertyName = key.toUtf8();
        const int propertyIndex = metaObject->indexOfProperty(propertyName.constData());
        if(propertyIndex < 0)
            continue;

        const QMetaProperty property = metaObject->property(propertyIndex);
        if(property.isWritable())
            result &= property.write(object, value);
    }

    return result;
}

QString QUtils::Dom::toString(const QDomNode& node)
{
    QString output = node.nodeValue();
    if(!output.isEmpty())
        return output;

    QDomElement element = node.toElement();
    output += element.tagName();
    output += "; ";

    // Add attribute values to the line
    QDomNamedNodeMap attributeMap = element.attributes();
    for(int i = 0; i < attributeMap.count(); ++i) {
        QDomAttr attribute = attributeMap.item(i).toAttr();
        output += " " + QString("%1=%2").arg(attribute.name(),attribute.value());
    }

    output += "; " + element.firstChild().nodeValue();

    return output;
}

void QUtils::Dom::recurseQDomNode(const QDomNode& node, int depth, const std::function<void (const QDomNode&, int depth)>& perNodeAction, bool verbose)
{
    if(verbose && node.isElement()) {
        QString output(depth*2, ' ');

        QDomElement element = node.toElement();
        output += toString(node);
        qDebug().noquote()<<output;
    }

    if(perNodeAction)
        perNodeAction(node, depth);

    QDomNodeList paths = node.childNodes();
    for (int i = 0; i < paths.count(); ++i) {
        recurseQDomNode(paths.at(i), depth + 1, perNodeAction, verbose);
    }
}

QDomNode QUtils::Dom::recurseNamedItem(const QDomNode& node, QString name)
{
    if(name.startsWith('/'))
        name.remove(0,1);
    if(name.endsWith('/'))
        name.remove(name.size()-1,1);

    const QStringList names = name.split('/');
    if(names.isEmpty())
        return QDomNode();

    QDomNode childNode = node;
    for(const QString& name: names)
    {
        childNode = childNode.namedItem(name);
    }

    return childNode;
}

void QUtils::Dom::listNamedItem(const QDomNode& node, const QString& name, QList<QDomNode>* foundElements)
{
    recurseQDomNode(node, 0, [&name, foundElements](const QDomNode& node, int)
                    {
                        if(node.nodeName()==name)
                            foundElements->append(node);
                    });
}

QDomNode QUtils::Dom::firstOfNamedItem(const QDomNode& node, const QString& name)
{
    QList<QDomNode> foundElements;
    listNamedItem(node, name, &foundElements);

    if(foundElements.isEmpty())
        return QDomNode();

    return foundElements.first();
}

QVariant QUtils::Dom::xmlToVariant(const QDomNode &node)
{
    if (node.isElement())
        return QVariant();

    return QVariant();
}

QDomDocument QUtils::Dom::readXmlDocument(const QString &filePath)
{
    QDomDocument document;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return document;

    document.setContent(&file);
    file.close();

    return document;
}

bool QUtils::Dom::writeXmlDocument(const QDomDocument &document, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite))
        return false;

    QTextStream out(&file);
    out<<document.toString();
    file.close();
    return true;
}

QVariant QUtils::Dom::readXml(const QString &filePath)
{
    const QDomDocument document = readXmlDocument(filePath);
    const QDomElement root=document.documentElement();

    return xmlToVariant(root);
}
