#ifndef GAMEOFLIFEMODEL_H
#define GAMEOFLIFEMODEL_H

#include <QAbstractTableModel>
#include <QQmlParserStatus>
#include <QVector>
#include <QDefs>

class QIODevice;

class GameOfLifeModel : public QAbstractTableModel,
                        public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    Q_WRITABLE_VAR_PROPERTY(int, width, Width, 64)
    Q_WRITABLE_VAR_PROPERTY(int, height, Height, 64)

    Q_READONLY_VAR_PROPERTY(int, alive, Alive, 0)
    Q_READONLY_VAR_PROPERTY(int, dead, Dead, 0)
    Q_READONLY_VAR_PROPERTY(int, total, Total, 0)

public:
    explicit GameOfLifeModel(QObject *parent = nullptr);

    void classBegin() override;
    void componentComplete() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    Q_INVOKABLE bool value(int row, int column) const;
    Q_INVOKABLE bool setValue(int row, int column, bool alive);
    Q_INVOKABLE bool toggleValue(int row, int column);
    const QVector<quint8> &state() const;
    const QVector<int> &aliveCells() const;
    int stride() const;

signals:
    void boardChanged();

public slots:
    void nextStep();
    bool loadFile(const QString &fileName);
    void loadPattern(const QString &plainText);
    void clear();
    void randomize();

private:
    bool m_completed;
    int m_stride;
    QVector<quint8> m_currentState;
    QVector<quint8> m_nextState;
    QVector<int> m_aliveCells;

    bool loadDevice(QIODevice *device);
    bool applyCellValue(int row, int column, bool alive);
    void resetBoard();
    void emitFullBoardChanged();
    qsizetype cellIndex(int row, int column) const;
};

#endif // GAMEOFLIFEMODEL_H
