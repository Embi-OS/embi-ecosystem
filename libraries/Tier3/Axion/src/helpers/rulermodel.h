#ifndef RULERMODEL_H
#define RULERMODEL_H

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <QDefs>

Q_ENUM_CLASS(RulerTickTypes, RulerTickType,
             Big,
             Medium,
             Small,)

struct RulerData {
    double value;
    double position;
    RulerTickType type;
};

class RulerModel: public QAbstractListModel,
                  public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)

    Q_WRITABLE_FUZ_PROPERTY(double, from, From, 0.0)
    Q_WRITABLE_FUZ_PROPERTY(double, to, To, 0.0)
    Q_WRITABLE_VAR_PROPERTY(int, decimal, Decimal, 3)
    Q_WRITABLE_FUZ_PROPERTY(double, pixelSize, PixelSize, 0.0)

public:
    explicit RulerModel(QObject *parent = nullptr);

    enum Roles {
        DisplayRole = Qt::DisplayRole,
        ValueRole = Qt::UserRole,
        PositionRole,
        TypeRole,
    };

    void classBegin() override;
    void componentComplete() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    int count() const { return rowCount(); };
    int size() const { return count(); };
    int length() const { return count(); };
    bool isEmpty() const { return count() == 0; };

public slots:
    void queueInvalidate();
    void invalidate();

signals:
    void countChanged(int count);
    void emptyChanged(bool empty);

protected:
    void countInvalidate();

    double determinateRulerRange() const;

private:
    QList<RulerData> generateData() const;

    int m_count=0;
    QHash<int, QByteArray> m_roleNames;

    QList<RulerData> m_data;

    bool m_completed = false;
    bool m_invalidateQueued = false;
    double m_offset = 0.0;
    double m_slope = 0.0;
};

#endif // RULERMODEL_H
