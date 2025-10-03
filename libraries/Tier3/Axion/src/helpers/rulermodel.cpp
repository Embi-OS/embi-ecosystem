#include "rulermodel.h"

RulerModel::RulerModel(QObject *parent):
    QAbstractListModel(parent)
{
    m_roleNames[DisplayRole] ="display";
    m_roleNames[ValueRole] ="value";
    m_roleNames[PositionRole] ="position";
    m_roleNames[TypeRole] ="type";

    connect(this, &RulerModel::fromChanged, this, &RulerModel::queueInvalidate);
    connect(this, &RulerModel::toChanged, this, &RulerModel::queueInvalidate);
    connect(this, &RulerModel::decimalChanged, this, &RulerModel::queueInvalidate);
    connect(this, &RulerModel::pixelSizeChanged, this, &RulerModel::queueInvalidate);
}

void RulerModel::classBegin()
{

}

void RulerModel::componentComplete()
{
    invalidate();
}

QVariant RulerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= m_data.size())
        return QVariant();

    const RulerData data = m_data.at(index.row());

    switch (role) {
    case RulerModel::DisplayRole:
        return QString::number(qFuzzyRound(data.value, m_decimal), 'g', QLocale::FloatingPointShortest);
    case RulerModel::ValueRole:
        return data.value;
    case RulerModel::PositionRole:
        return data.position;
    case RulerModel::TypeRole:
        return data.type;
    }

    return QVariant();
}

int RulerModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : m_data.size();
}

QHash<int, QByteArray> RulerModel::roleNames() const
{
    return m_roleNames;
}

void RulerModel::queueInvalidate()
{
    if(!m_completed)
        return;

    if (m_delayed) {
        if (!m_invalidateQueued) {
            m_invalidateQueued = true;
            QMetaObject::invokeMethod(this, &RulerModel::invalidate, Qt::QueuedConnection);
        }
    } else {
        invalidate();
    }
}

void RulerModel::invalidate()
{
    m_completed = true;
    m_invalidateQueued = false;

    m_offset = m_from;
    m_slope = (m_to-m_from)/m_pixelSize;

    const QList<RulerData> data = generateData();
    beginResetModel();
    m_data = data;
    endResetModel();
}

QList<RulerData> RulerModel::generateData() const
{
    const double rulerRange = determinateRulerRange();
    const double valTickSpace = rulerRange/10.0;
    const double pixTickSpace = valTickSpace/m_slope;
    const double nbTick = pixTickSpace ? (m_pixelSize+7.0)/pixTickSpace : 0;
    const double rulerAlignement = qFuzzyModulo(m_offset,valTickSpace);

    QList<RulerData> rulerData;
    rulerData.reserve(nbTick);
    for(int i=0; i<qCeil(nbTick); i++)
    {
        RulerData data;

        data.position = (double)i*pixTickSpace + ((valTickSpace-rulerAlignement)/m_slope) - pixTickSpace;

        if(data.position<-1.0 || data.position>(m_pixelSize+1.0)) {
            continue;
        }

        data.value = data.position*m_slope+m_offset;

        if(qFuzzyIsNull(qFuzzyModulo(data.value, rulerRange, 6)))
            data.type = RulerTickTypes::Big;
        else if(qFuzzyIsNull(qFuzzyModulo(data.value, rulerRange/2., 6)))
            data.type = RulerTickTypes::Medium;
        else if(qFuzzyIsNull(qFuzzyModulo(data.value, rulerRange/10., 6)))
            data.type = RulerTickTypes::Small;
        else
            data.type = RulerTickTypes::Small;

        rulerData.append(data);
    }

    return rulerData;
}

void RulerModel::countInvalidate()
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

double RulerModel::determinateRulerRange() const
{
    static const QList<int> multipliers({1, 2, 5});
    static const int minTickSpace = 7;

    if(qFuzzyIsNull(m_slope))
        return 0;

    int decimal = 6;
    for(int i=-decimal; i<=decimal; i++)
    {
        for(int j=0;j<multipliers.count();j++)
        {
            double range = (double)multipliers[j]*(double)qPow(10,i);

            double valTickSpace = range/10.0;
            double pixTickSpace = valTickSpace/m_slope;
            if(pixTickSpace>=minTickSpace)
            {
                return range;
            }
        }
    }

    return m_to-m_from;
}
