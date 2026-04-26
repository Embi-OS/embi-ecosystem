#include "gameoflifemodel.h"
#include <QBuffer>
#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QRandomGenerator>
#include <QTextStream>

#define GAME_OF_LIFE_MODEL_THREADED

#ifdef QT_CONCURRENT_LIB
#include <QFuture>
#include <QFutureSynchronizer>
#include <QThread>
#include <QtConcurrentRun>
#else
#undef GAME_OF_LIFE_MODEL_THREADED
#endif

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
    m_completed(false),
    m_stride(0)
{

}

void GameOfLifeModel::classBegin()
{

}

void GameOfLifeModel::componentComplete()
{
    connect(this, &GameOfLifeModel::heightChanged, this, &GameOfLifeModel::resetBoard);
    connect(this, &GameOfLifeModel::widthChanged, this, &GameOfLifeModel::resetBoard);

    m_completed = true;
    resetBoard();
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

    setAlive(m_aliveCells.size());
    setTotal(m_currentState.size());
    setDead(m_total - m_alive);

    qDebug()<<"nextStep"<<timer.nsecsElapsed()/1000000.0;

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

    setAlive(m_aliveCells.size());
    setTotal(m_currentState.size());
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
        synchronizer.addFuture(QtConcurrent::run([nextState, stride, width, range]() {
            QVector<int> aliveCells;
            aliveCells.reserve(((range.lastRow - range.firstRow) * width) / 2);

            QRandomGenerator generator(range.index+1);

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

    setAlive(m_aliveCells.size());
    setTotal(m_currentState.size());
    setDead(m_total - m_alive);

    qDebug()<<"randomize"<<timer.nsecsElapsed()/1000000.0;

    emitFullBoardChanged();
}

void GameOfLifeModel::resetBoard()
{
    if (!m_completed)
        return;

    QElapsedTimer timer;
    timer.start();

    beginResetModel();

    m_stride = m_width + 2;
    const qsizetype bufferSize = qsizetype(m_height + 2) * m_stride;
    m_currentState.fill(0, bufferSize);
    m_nextState.fill(0, bufferSize);
    m_aliveCells.clear();

    setAlive(m_aliveCells.size());
    setTotal(m_currentState.size());
    setDead(m_total - m_alive);

    endResetModel();
    emit boardChanged();

    qDebug()<<"resetBoard"<<timer.nsecsElapsed()/1000000.0;
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

    setAlive(m_aliveCells.size());
    setTotal(m_currentState.size());
    setDead(m_total - m_alive);

    qDebug()<<"loadDevice"<<timer.nsecsElapsed()/1000000.0;

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
    setTotal(m_currentState.size());
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

    qDebug()<<"emitFullBoardChanged"<<timer.nsecsElapsed()/1000000.0;
}

qsizetype GameOfLifeModel::cellIndex(int row, int column) const
{
    return qsizetype(row + 1) * m_stride + (column + 1);
}
