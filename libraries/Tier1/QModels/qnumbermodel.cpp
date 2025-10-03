#include "qnumbermodel.h"

QNumberModel::QNumberModel(QObject *parent):
    QAbstractListModel(parent)
{
    m_roleNames[DisplayRole] ="display";
    m_roleNames[ValueRole] ="value";

    connect(this, &QNumberModel::fromChanged, this, &QNumberModel::queueInvalidate);
    connect(this, &QNumberModel::toChanged, this, &QNumberModel::queueInvalidate);
    connect(this, &QNumberModel::stepSizeChanged, this, &QNumberModel::queueInvalidate);
    connect(this, &QNumberModel::formattingOptionsChanged, this, &QNumberModel::queueInvalidate);
}

void QNumberModel::classBegin()
{

}

void QNumberModel::componentComplete()
{
    invalidate();
}

QVariant QNumberModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case QNumberModel::DisplayRole: {
        auto locale = QLocale();
        locale.setNumberOptions(m_formattingOptions);
        return QVariant(locale.toString(determinateValue(index)));
    }
    case QNumberModel::ValueRole:
        return QVariant(determinateValue(index));
    }
    return QVariant();
}

int QNumberModel::rowCount(const QModelIndex &index) const
{
    if (index.parent().isValid())
        return 0;
    return m_rowCount;
}

QHash<int, QByteArray> QNumberModel::roleNames() const
{
    return m_roleNames;
}

double QNumberModel::determinateValue(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0.0;
    return m_minimumValue + m_stepValue * index.row();
}

int QNumberModel::determinateCount() const
{
    if (m_stepSize == 0)
        return 1;
    // 1 initial entry (the minimumValue) + the number of valid steps afterwards
    return 1 + std::max(0, qFloor((m_to - m_from) / m_stepSize));
}

void QNumberModel::queueInvalidate()
{
    if(!m_completed)
        return;

    if (m_delayed) {
        if (!m_invalidateQueued) {
            m_invalidateQueued = true;
            QMetaObject::invokeMethod(this, &QNumberModel::invalidate, Qt::QueuedConnection);
        }
    } else {
        invalidate();
    }
}

void QNumberModel::invalidate()
{
    m_completed = true;
    m_invalidateQueued = false;

    int newRowCount = determinateCount();
    if(newRowCount==m_rowCount)
    {
        m_minimumValue = m_from;
        m_maximumValue = m_to;
        m_stepValue = m_stepSize;
        emit this->dataChanged(index(0), index(m_rowCount-1), QVector<int>{DisplayRole, ValueRole});
    }
    else
    {
        beginResetModel();

        m_minimumValue = m_from;
        m_maximumValue = m_to;
        m_stepValue = m_stepSize;
        m_rowCount = newRowCount;

        endResetModel();
    }
}

void QNumberModel::countInvalidate()
{
    const int aCount = count();
    bool emptyChanged=false;

    if(m_count==aCount)
        return;

    if((m_count==0 && aCount!=0) || (m_count!=0 && aCount==0))
        emptyChanged=true;

    m_count=aCount;
    emit this->countChanged(count());

    if(emptyChanged)
        emit this->emptyChanged(isEmpty());
}
