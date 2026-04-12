#ifndef CPUINFOMODEL_H
#define CPUINFOMODEL_H

#include <QDefs>
#include <QUtils>
#include <QModels>

class CpuInfoModel: public QVariantListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_CONSTANT_REF_PROPERTY(QString, model, "N/A")
    Q_CONSTANT_REF_PROPERTY(QString, revision, "N/A")
    Q_CONSTANT_REF_PROPERTY(QString, serial, "N/A")
    Q_CONSTANT_REF_PROPERTY(QString, productId, "N/A")
    Q_CONSTANT_VAR_PROPERTY(int, coreCount, 0)

public:
    explicit CpuInfoModel(QObject* parent=nullptr);

protected:
    QVariantList selectSource(bool* result=nullptr) override final;
};

#endif // CPUINFOMODEL_H
