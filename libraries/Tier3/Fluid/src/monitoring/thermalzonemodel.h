#ifndef THERMALZONEMODEL_H
#define THERMALZONEMODEL_H

#include <QDefs>
#include <QModels>

class ThermalZoneModel: public QVariantListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(int, tick, Tick, 0)

public:
    explicit ThermalZoneModel(QObject* parent=nullptr);

protected:
    bool doSelect() override final;

    static QVariantList selectZones();
};

#endif // THERMALZONEMODEL_H
