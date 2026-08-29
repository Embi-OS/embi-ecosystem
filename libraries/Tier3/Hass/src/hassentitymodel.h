#ifndef HASSENTITYMODEL_H
#define HASSENTITYMODEL_H

#include "RestModel/restmodel.h"

class HassEntityModel: public RestModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit HassEntityModel(QObject* parent=nullptr);

protected:
    QSWorker* createSubmitWorker() override;
};

#endif // HASSENTITYMODEL_H
