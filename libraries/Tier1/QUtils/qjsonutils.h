#ifndef QJSONUTILS_H
#define QJSONUTILS_H

#include <QVariant>
#include <QLocale>
#include <QByteArray>
#include <QDomDocument>
#include <QVector>
#include <QPointF>
#include <QVector3D>

struct QJsonParseError;
struct QCborParserError;
class QObject;

namespace QUtils
{

namespace Log
{

QByteArray variantToLog(const QVariant& variant, bool compact = false);
QByteArray variantToJson(const QVariant& variant, bool compact = false);
QByteArray variantToCbor(const QVariant& variant, int opt = 0);

}

namespace Json
{

    QByteArray variantToJson(const QVariant& variant, bool compact = true, int doublePrecision = QLocale::FloatingPointShortest);
    void variantToJsonStream(const QVariant& variant, QIODevice* device, bool compact = true, int doublePrecision = QLocale::FloatingPointShortest);

    QVariant jsonToVariant(const QByteArray& json, QJsonParseError* error = nullptr);
    QVariant jsonStreamToVariant(QIODevice* device, QJsonParseError* error = nullptr);

}

namespace Cbor
{

    QByteArray variantToCbor(const QVariant& variant, int opt = 0);
    void variantToCborStream(const QVariant& variant, QIODevice* device, int opt = 0);

    QVariant cborToVariant(const QByteArray& cbor, QCborParserError* error = nullptr);
    QVariant cborStreamToVariant(QIODevice* device, QCborParserError* error = nullptr);

}

namespace Variant
{

    QVariantList serializeSeries3D(const QVector<QVector3D>& points, int precision=6, const QString& xHeader="x", const QString& yHeader="y", const QString& zHeader="z");
    QVariantMap serializePointF(const QPointF& point, int precision=6, const QString& xHeader="x", const QString& yHeader="y");
    QVariant serializeDouble(double d, int precision=6);

    QVector<QVector3D> deserializeSeries3D(const QVariantList& pointsArray, const QString& xHeader="x", const QString& yHeader="y", const QString& zHeader="z");
    QPointF deserializePointF(const QVariantMap& point, const QString& xHeader="x", const QString& yHeader="y");

}

namespace Object
{

    QVariantMap objectToVariant(const QObject* object);
    bool variantToObject(const QVariantMap& variant, QObject* object);

}

namespace Dom
{

    QString toString(const QDomNode& node);

    void recurseQDomNode(const QDomNode& node, int depth, const std::function<void (const QDomNode&, int depth)>& perNodeAction, bool verbose=false);
    QDomNode recurseNamedItem(const QDomNode& node, QString name);
    void listNamedItem(const QDomNode& node, const QString& name, QList<QDomNode>* foundElements);
    QDomNode firstOfNamedItem(const QDomNode& node, const QString& name);

    QVariant xmlToVariant(const QDomNode &xmlNode);

    /* --------------------------------------------------------------------------------
     * Read/Write from/to XML file.
     * -------------------------------------------------------------------------------- */
    QDomDocument readXmlDocument(const QString &filePath);
    bool writeXmlDocument(const QDomDocument &document, const QString &filePath);

    QVariant readXml(const QString &filePath);

}

}

#endif // QUTILS_H
