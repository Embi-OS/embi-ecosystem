#ifndef QCSVLISTMODEL_H
#define QCSVLISTMODEL_H

#include "qvariantlistmodel.h"

class QCsvListModel: public QVariantListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(CsvListModel)

    Q_WRITABLE_REF_PROPERTY(QString, csvPath, CsvPath, "")
    Q_WRITABLE_VAR_PROPERTY(char, csvSeparator, CsvSeparator, ';')
    Q_READONLY_REF_PROPERTY(QString, csv, Csv, "")

public:
    explicit QCsvListModel(QObject * parent = nullptr);
    explicit QCsvListModel(const QString& baseName, QObject *parent = nullptr);
    explicit QCsvListModel(const QString& csvPath, const QString& baseName, QObject *parent = nullptr);
    ~QCsvListModel() override;

protected:
    QVariantList selectSource(bool* result=nullptr) override final;
    bool doSubmit() override final;

    QVariantList fromFile(bool* result=nullptr);
    QVariantList fromCsv(const QString& csv, bool* result=nullptr);

    bool submitCsv();
    bool toFile(const QString& csv) const;
};

#endif // QCSVLISTMODEL_H
