#include "graphseriesmodel.h"

GraphSeriesModel::GraphSeriesModel(QObject* parent) :
    QObjectListModel(parent, &GraphSeriesObject::staticMetaObject)
{

}

void GraphSeriesModel::reset()
{
    beginResetModel();

    for(GraphSeriesObject* item : modelIterator<GraphSeriesObject>())
        item->reset();

    endResetModel();
}

void GraphSeriesModel::resetMinMax()
{
    beginResetModel();

    for(GraphSeriesObject* item : modelIterator<GraphSeriesObject>())
        item->resetMinMax();

    endResetModel();
}
