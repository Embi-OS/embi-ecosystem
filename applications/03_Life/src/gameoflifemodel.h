#ifndef GAMEOFLIFEMODEL_H
#define GAMEOFLIFEMODEL_H

#include <QAbstractTableModel>
#include <QQmlParserStatus>
#include <QVector>
#include <QTimer>
#include <QElapsedTimer>
#include <QDefs>

class QIODevice;

class GameOfLifeModel : public QAbstractTableModel,
                        public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    // Resizing must update dimensions and storage inside one model reset.
    Q_PROPERTY(int width READ getWidth WRITE setWidth RESET resetWidth NOTIFY widthChanged FINAL)
    Q_PROPERTY(int height READ getHeight WRITE setHeight RESET resetHeight NOTIFY heightChanged FINAL)

    Q_WRITABLE_VAR_PROPERTY(bool, running, Running, false)
    Q_WRITABLE_VAR_PROPERTY(int, stepsPerSecond, StepsPerSecond, 60)
    Q_READONLY_VAR_PROPERTY(quint64, generation, Generation, 0)
    Q_READONLY_FUZ_PROPERTY(qreal, actualStepsPerSecond, ActualStepsPerSecond, 0.0)

    Q_READONLY_VAR_PROPERTY(int, alive, Alive, 0)
    Q_READONLY_VAR_PROPERTY(int, dead, Dead, 0)
    Q_READONLY_VAR_PROPERTY(int, total, Total, 0)

public:
    explicit GameOfLifeModel(QObject *parent = nullptr);

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    bool setWidth(int width);
    bool setHeight(int height);
    bool resetWidth() { return setWidth(64); }
    bool resetHeight() { return setHeight(64); }

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
    void widthChanged(int width);
    void heightChanged(int height);

public slots:
    void nextStep();
    void restart();
    bool loadFile(const QString &fileName);
    void loadPattern(const QString &plainText);
    void clear();
    void randomize();

private:
    QTimer m_stepTimer;
    QElapsedTimer m_stepClock;
    QElapsedTimer m_rateClock;
    quint64 m_rateGeneration = 0;
    qreal m_stepCredit = 0;
    void updateStepTimer();
    void advanceSimulation();
    void resetTimeline();

    int m_width = 64;
    int m_height = 64;
    bool m_resizing = false;
    bool m_completed;
    int m_stride;
    QVector<quint8> m_initialState;
    QVector<quint8> m_currentState;
    QVector<quint8> m_nextState;
    QVector<int> m_aliveCells;

    bool loadDevice(QIODevice *device);
    bool applyCellValue(int row, int column, bool alive);
    bool resizeBoard(int width, int height);
    void emitFullBoardChanged();
    qsizetype cellIndex(int row, int column) const;
};

#endif // GAMEOFLIFEMODEL_H
