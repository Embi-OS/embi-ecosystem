#include "gameoflifemodel.h"
#include <QBuffer>
#include <QLoggingCategory>
#include <QElapsedTimer>
#include <QFile>
#include <QRandomGenerator>
#include <QTextStream>
#include <QScopedValueRollback>

#define GAME_OF_LIFE_MODEL_THREADED

#ifdef QT_CONCURRENT_LIB
#include <QFuture>
#include <QFutureSynchronizer>
#include <QThread>
#include <QtConcurrentRun>
#else
#undef GAME_OF_LIFE_MODEL_THREADED
#endif

Q_LOGGING_CATEGORY(lifeModelLog, "life.model", QtWarningMsg)

namespace
{
#ifdef GAME_OF_LIFE_MODEL_THREADED
struct RowRange
{
    int index;
    int firstRow;
    int lastRow;
};

inline QVector<RowRange> rowRanges(int height)
{
    const int workerCount = qMax(1, qMin(height, QThread::idealThreadCount()));
    QVector<RowRange> ranges;
    ranges.reserve(workerCount);

    const int rowsPerWorker = height / workerCount;
    const int extraRows = height % workerCount;
    int firstRow = 0;
    for (int workerIndex = 0; workerIndex < workerCount; ++workerIndex) {
        const int rowCount = rowsPerWorker + (workerIndex < extraRows ? 1 : 0);
        if (rowCount == 0)
            continue;

        ranges.append({workerIndex, firstRow, firstRow + rowCount});
        firstRow += rowCount;
    }

    return ranges;
}
#endif

}

GameOfLifeModel::GameOfLifeModel(QObject *parent):
    QAbstractTableModel(parent),
    m_stepTimer(this),
    m_completed(false),
    m_stride(0)
{
    m_stepTimer.setTimerType(Qt::PreciseTimer);
    connect(&m_stepTimer, &QTimer::timeout, this, &GameOfLifeModel::advanceSimulation);
    connect(this, &GameOfLifeModel::runningChanged, this, &GameOfLifeModel::updateStepTimer);
    connect(this, &GameOfLifeModel::stepsPerSecondChanged, this, [this] {
        if (setStepsPerSecond(qBound(1, m_stepsPerSecond, 240)))
            return;
        updateStepTimer();
    });
}

void GameOfLifeModel::classBegin()
{

}

void GameOfLifeModel::componentComplete()
{
    if (m_completed)
        return;
    resizeBoard(m_width, m_height);
}

int GameOfLifeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_completed)
        return 0;

    return m_height;
}

int GameOfLifeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_completed)
        return 0;

    return m_width;
}

QVariant GameOfLifeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole || !m_completed)
        return QVariant();

    if (index.row() < 0 || index.row() >= m_height || index.column() < 0 || index.column() >= m_width)
        return QVariant();

    return QVariant::fromValue(bool(m_currentState[cellIndex(index.row(), index.column())]));
}

bool GameOfLifeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || (role != Qt::EditRole && role != Qt::DisplayRole) || !value.isValid())
        return false;

    return applyCellValue(index.row(), index.column(), value.toBool());
}

QVariant GameOfLifeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    if (role == Qt::DisplayRole)
        return section;
    return QVariant();
}

QHash<int, QByteArray> GameOfLifeModel::roleNames() const
{
    return {
        { Qt::DisplayRole, "display" }
    };
}

Qt::ItemFlags GameOfLifeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

bool GameOfLifeModel::value(int row, int column) const
{
    if (row < 0 || row >= m_height || column < 0 || column >= m_width || !m_completed)
        return false;

    return m_currentState[cellIndex(row, column)];
}

bool GameOfLifeModel::setValue(int row, int column, bool alive)
{
    return applyCellValue(row, column, alive);
}

bool GameOfLifeModel::toggleValue(int row, int column)
{
    if (row < 0 || row >= m_height || column < 0 || column >= m_width || !m_completed)
        return false;

    return applyCellValue(row, column, !m_currentState[cellIndex(row, column)]);
}

const QVector<quint8> &GameOfLifeModel::state() const
{
    return m_currentState;
}

const QVector<int> &GameOfLifeModel::aliveCells() const
{
    return m_aliveCells;
}

int GameOfLifeModel::stride() const
{
    return m_stride;
}

void GameOfLifeModel::nextStep()
{
    if (!m_completed || m_width <= 0 || m_height <= 0)
        return;

    if (m_generation == 0)
        m_initialState = m_currentState;

    QElapsedTimer timer;
    timer.start();

    QVector<int> nextAliveCells;
#ifdef GAME_OF_LIFE_MODEL_THREADED
    const QVector<RowRange> ranges = rowRanges(m_height);
    const qsizetype stride = m_stride;
    const int width = m_width;
    const quint8 *currentState = m_currentState.constData();
    quint8 *nextState = m_nextState.data();

    QFutureSynchronizer<QVector<int>> synchronizer;
    for (const RowRange &range : ranges) {
        synchronizer.addFuture(QtConcurrent::run([currentState, nextState, stride, width, range]() {
            QVector<int> aliveCells;
            aliveCells.reserve(((range.lastRow - range.firstRow) * width) / 2);

            for (int row = range.firstRow; row < range.lastRow; ++row) {
                const qsizetype rowOffset = qsizetype(row + 1) * stride + 1;

                for (int column = 0; column < width; ++column) {
                    const qsizetype index = rowOffset + column;
                    const int neighbors =
                        currentState[index - stride - 1] +
                        currentState[index - stride] +
                        currentState[index - stride + 1] +
                        currentState[index - 1] +
                        currentState[index + 1] +
                        currentState[index + stride - 1] +
                        currentState[index + stride] +
                        currentState[index + stride + 1];
                    const quint8 currentValue = currentState[index];
                    const quint8 nextValue = (neighbors == 3 || (currentValue && neighbors == 2)) ? 1 : 0;

                    nextState[index] = nextValue;
                    if (nextValue != 0)
                        aliveCells.append(row * width + column);
                }
            }

            return aliveCells;
        }));
    }

    synchronizer.waitForFinished();
    for (const QFuture<QVector<int>> &future: synchronizer.futures()) {
        nextAliveCells.append(future.result());
    }
#else
    const qsizetype stride = m_stride;
    nextAliveCells.reserve(m_aliveCells.size() + m_aliveCells.size() / 4);

    for (int row = 0; row < m_height; ++row) {
        const qsizetype rowOffset = qsizetype(row + 1) * stride + 1;

        for (int column = 0; column < m_width; ++column) {
            const qsizetype index = rowOffset + column;
            const int neighbors =
                m_currentState[index - stride - 1] +
                m_currentState[index - stride] +
                m_currentState[index - stride + 1] +
                m_currentState[index - 1] +
                m_currentState[index + 1] +
                m_currentState[index + stride - 1] +
                m_currentState[index + stride] +
                m_currentState[index + stride + 1];
            const quint8 currentValue = m_currentState[index];
            const quint8 nextValue = (neighbors == 3 || (currentValue && neighbors == 2)) ? 1 : 0;

            m_nextState[index] = nextValue;
            if (nextValue != 0)
                nextAliveCells.append(row * m_width + column);
        }
    }
#endif

    m_currentState.swap(m_nextState);
    m_aliveCells.swap(nextAliveCells);

    setGeneration(m_generation + 1);
    setAlive(m_aliveCells.size());
    setTotal(m_width * m_height);
    setDead(m_total - m_alive);

    qCDebug(lifeModelLog)<<"nextStep"<<timer.nsecsElapsed()/1000000.0;

    emitFullBoardChanged();
}

bool GameOfLifeModel::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    return loadDevice(&file);
}

void GameOfLifeModel::loadPattern(const QString &plainText)
{
    if (!m_completed)
        return;

    QByteArray data = plainText.toUtf8();
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    loadDevice(&buffer);
}

void GameOfLifeModel::clear()
{
    if (!m_completed)
        return;

    m_currentState.fill(0);
    m_nextState.fill(0);
    m_aliveCells.clear();

    resetTimeline();
    setAlive(m_aliveCells.size());
    setTotal(m_width * m_height);
    setDead(m_total - m_alive);


    emitFullBoardChanged();
}

void GameOfLifeModel::randomize()
{
    if (!m_completed)
        return;

    QElapsedTimer timer;
    timer.start();

    m_stride = m_width + 2;
    const qsizetype bufferSize = qsizetype(m_height + 2) * m_stride;
    m_currentState.fill(0, bufferSize);
    m_nextState.fill(0, bufferSize);
    QVector<int> nextAliveCells;

#ifdef GAME_OF_LIFE_MODEL_THREADED
    const QVector<RowRange> ranges = rowRanges(m_height);
    const qsizetype stride = m_stride;
    const int width = m_width;
    quint8 *nextState = m_nextState.data();

    QFutureSynchronizer<QVector<int>> synchronizer;
    for (const RowRange &range : ranges) {
        const quint32 seed = QRandomGenerator::global()->generate();
        synchronizer.addFuture(QtConcurrent::run([nextState, stride, width, range, seed]() {
            QVector<int> aliveCells;
            aliveCells.reserve(((range.lastRow - range.firstRow) * width) / 2);

            QRandomGenerator generator(seed);

            for (int row = range.firstRow; row < range.lastRow; ++row) {
                const qsizetype rowOffset = qsizetype(row + 1) * stride + 1;

                for (int column = 0; column < width; ++column) {
                    const qsizetype index = rowOffset + column;
                    const quint8 alive = generator.bounded(2);
                    nextState[index] = alive;
                    if (alive != 0)
                        aliveCells.append(row * width + column);
                }
            }

            return aliveCells;
        }));
    }

    synchronizer.waitForFinished();
    for (const QFuture<QVector<int>> &future: synchronizer.futures()) {
        nextAliveCells.append(future.result());
    }
#else
    nextAliveCells.reserve(bufferSize / 2);
    for (int row = 0; row < m_height; ++row) {
        for (int column = 0; column < m_width; ++column) {
            const quint8 alive = QRandomGenerator::global()->bounded(2);
            m_nextState[cellIndex(row, column)] = alive;
            if (alive != 0)
                nextAliveCells.append(row * m_width + column);
        }
    }
#endif

    m_currentState.swap(m_nextState);
    m_aliveCells.swap(nextAliveCells);

    resetTimeline();
    setAlive(m_aliveCells.size());
    setTotal(m_width * m_height);
    setDead(m_total - m_alive);

    qCDebug(lifeModelLog)<<"randomize"<<timer.nsecsElapsed()/1000000.0;

    emitFullBoardChanged();
}

bool GameOfLifeModel::setWidth(int width)
{
    return resizeBoard(width, m_height);
}

bool GameOfLifeModel::setHeight(int height)
{
    return resizeBoard(m_width, height);
}

bool GameOfLifeModel::resizeBoard(int width, int height)
{
    // Bound both dimensions and area before arithmetic or allocation.
    constexpr int maxDimension = 4096;
    // Allow a full 4K grid at one cell per pixel, including intermediate
    // dimensions while width and height bindings update independently.
    constexpr qint64 maxCells = qint64(maxDimension) * maxDimension;
    if (m_resizing || width < 0 || height < 0 || width > maxDimension || height > maxDimension
        || qint64(width) * height > maxCells)
        return false;
    const bool widthChanged = width != m_width;
    const bool heightChanged = height != m_height;
    if (m_completed && !widthChanged && !heightChanged)
        return false;

    QScopedValueRollback<bool> resizing(m_resizing, true);
    const int stride = width + 2;
    QVector<quint8> currentState(qsizetype(height + 2) * stride, 0);
    QVector<quint8> nextState(currentState.size(), 0);
    QVector<int> aliveCells;
    for (int cell : std::as_const(m_aliveCells)) {
        const int row = cell / m_width;
        const int column = cell % m_width;
        if (row < height && column < width) {
            currentState[qsizetype(row + 1) * stride + column + 1] = 1;
            aliveCells.append(row * width + column);
        }
    }

    beginResetModel();
    m_width = width;
    m_height = height;
    m_stride = stride;
    m_currentState.swap(currentState);
    m_nextState.swap(nextState);
    m_aliveCells.swap(aliveCells);
    m_completed = true;
    resetTimeline();
    setAlive(m_aliveCells.size());
    setTotal(m_width * m_height);
    setDead(m_total - m_alive);
    endResetModel();
    if (widthChanged)
        emit this->widthChanged(m_width);
    if (heightChanged)
        emit this->heightChanged(m_height);
    emit boardChanged();
    return true;
}

bool GameOfLifeModel::loadDevice(QIODevice *device)
{
    if (!m_completed || !device)
        return false;

    QElapsedTimer timer;
    timer.start();

    QTextStream in(device);
    QVector<QByteArray> rows;
    rows.reserve(64);

    int patternWidth = 0;
    while (!in.atEnd()) {
        QByteArray line = in.readLine().toUtf8();
        if (!line.isEmpty() && line.back() == '\r')
            line.chop(1);

        if (line.isEmpty() || line.startsWith('!'))
            continue;

        patternWidth = qMax(patternWidth, line.size());
        rows.append(std::move(line));
    }

    const int patternHeight = rows.size();
    const int startRow = (m_height - patternHeight) / 2;
    const int startColumn = (m_width - patternWidth) / 2;

    m_currentState.fill(0);
    m_nextState.fill(0);
    m_aliveCells.clear();

    int aliveCapacity = 0;
    for (const QByteArray &row : rows)
        aliveCapacity += row.count('O');
    m_aliveCells.reserve(aliveCapacity);

    for (int y = 0; y < patternHeight; ++y) {
        const QByteArray &line = rows[y];

        for (int x = 0; x < line.size(); ++x) {
            if (line[x] != 'O')
                continue;

            const int row = y + startRow;
            const int column = x + startColumn;
            if (row < 0 || row >= m_height || column < 0 || column >= m_width)
                continue;

            m_currentState[cellIndex(row, column)] = 1;
            m_aliveCells.append(row * m_width + column);
        }
    }

    resetTimeline();
    setAlive(m_aliveCells.size());
    setTotal(m_width * m_height);
    setDead(m_total - m_alive);

    qCDebug(lifeModelLog)<<"loadDevice"<<timer.nsecsElapsed()/1000000.0;

    emitFullBoardChanged();

    return true;
}

bool GameOfLifeModel::applyCellValue(int row, int column, bool alive)
{
    if (row < 0 || row >= m_height || column < 0 || column >= m_width || !m_completed)
        return false;

    const qsizetype index = cellIndex(row, column);
    const quint8 newValue = alive ? 1 : 0;
    if (m_currentState[index] == newValue)
        return false;

    m_currentState[index] = newValue;
    m_nextState[index] = newValue;

    const int flatIndex = row * m_width + column;
    if (newValue != 0) {
        m_aliveCells.append(flatIndex);
    } else {
        const qsizetype aliveIndex = m_aliveCells.indexOf(flatIndex);
        if (aliveIndex >= 0)
            m_aliveCells.removeAt(aliveIndex);
    }

    setAlive(m_aliveCells.size());
    setTotal(m_width * m_height);
    setDead(m_total - m_alive);

    emit dataChanged(this->index(row, column), this->index(row, column), {Qt::DisplayRole});
    emit boardChanged();

    return true;
}

void GameOfLifeModel::emitFullBoardChanged()
{
    if (!m_completed || m_height <= 0 || m_width <= 0)
        return;

    QElapsedTimer timer;
    timer.start();

    emit dataChanged(index(0, 0), index(m_height - 1, m_width - 1), {Qt::DisplayRole});
    emit boardChanged();

    qCDebug(lifeModelLog)<<"emitFullBoardChanged"<<timer.nsecsElapsed()/1000000.0;
}

qsizetype GameOfLifeModel::cellIndex(int row, int column) const
{
    return qsizetype(row + 1) * m_stride + (column + 1);
}

void GameOfLifeModel::updateStepTimer()
{
    m_stepTimer.stop();
    m_stepCredit = 0;
    m_stepClock.start();
    m_rateClock.start();
    m_rateGeneration = m_generation;
    setActualStepsPerSecond(0);
    if (m_running)
        m_stepTimer.start(qMax(1, 1000 / m_stepsPerSecond));
}

void GameOfLifeModel::advanceSimulation()
{
    // Bound catch-up work after stalls; never accumulate an unbounded backlog.
    m_stepCredit = qMin(qreal(8), m_stepCredit + m_stepClock.nsecsElapsed() * m_stepsPerSecond / 1.0e9);
    m_stepClock.restart();
    QElapsedTimer budget;
    budget.start();
    while (m_running && m_stepCredit >= 1 && budget.elapsed() < 8) {
        m_stepCredit -= 1;
        nextStep();
    }
    const qint64 elapsed = m_rateClock.elapsed();
    if (elapsed >= 500) {
        setActualStepsPerSecond((m_generation - m_rateGeneration) * 1000.0 / elapsed);
        m_rateGeneration = m_generation;
        m_rateClock.restart();
    }
}

void GameOfLifeModel::resetTimeline()
{
    m_initialState.clear();
    setGeneration(0);
    updateStepTimer();
}

void GameOfLifeModel::restart()
{
    if (!m_completed || m_generation == 0 || m_initialState.size() != m_currentState.size())
        return;
    setRunning(false);
    m_currentState = m_initialState;
    m_nextState.fill(0);
    m_aliveCells.clear();
    for (int row = 0; row < m_height; ++row) {
        for (int column = 0; column < m_width; ++column) {
            if (m_currentState[cellIndex(row, column)])
                m_aliveCells.append(row * m_width + column);
        }
    }
    resetTimeline();
    setAlive(m_aliveCells.size());
    setDead(m_total - m_alive);
    emitFullBoardChanged();
}
