#ifndef GRAPHSERIESMODEL_H
#define GRAPHSERIESMODEL_H

#include <QModels>
#include <QDefs>

#include "graphseriesobject.h"

class GraphSeriesModel : public QObjectListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit GraphSeriesModel(QObject* parent = nullptr);

    void reset();
    void resetMinMax();
};

#endif // GRAPHSERIESMODEL_H
