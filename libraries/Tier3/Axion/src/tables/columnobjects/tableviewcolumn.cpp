#include "tableviewcolumn.h"
#include "tableviewmodel.h"
#include "axion_log.h"

#include "axion_helpertypes.h"

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, regExpChained, (R"(^(.*)\[(.*)\]\.(.*)$)"))

TableViewColumn::TableViewColumn(QObject* parent):
    QObject(parent)
{
    m_font = Theme::Get()->getTextTheme()->headline7;
    m_color = Theme::Get()->getWhite();
    m_defaultValue = QString();

    connect(this, &TableViewColumn::roleNameChanged, this, [this](const QString& roleName){
        if(m_title.isEmpty())
            emit this->titleChanged(roleName);
    });
}

void TableViewColumn::viewModelCompleted(const TableViewModel &viewModel)
{
    Q_UNUSED(viewModel)
}

void TableViewColumn::updateRoles(const TableViewModel& viewModel)
{
    m_role = -1;
    m_chainedRoleNames.isValid = false;
    if (m_roleName.contains('[') && m_roleName.contains(']'))
    {
        m_chainedRoleNames = parseChainedRoleName(m_roleName, viewModel, m_roleNameSeparator);
        m_role = m_chainedRoleNames.listRole;
        m_subRoleNames = m_chainedRoleNames.listSubRoleNames;
    }
    else if(m_roleName=="*")
    {
        m_role = -2;
        m_subRoleNames.clear();
    }
    else if(!m_roleName.isEmpty())
    {
        QStringList roleNames = m_roleName.split(m_roleNameSeparator, Qt::SkipEmptyParts);
        const QString roleName = roleNames.takeFirst();
        m_role = viewModel.roleForName(roleName);
        m_subRoleNames = roleNames;
    }
}

void TableViewColumn::invalidate()
{
    emit this->invalidated();
}

QColor TableViewColumn::color(int row, const TableViewModel& viewModel)
{
    Q_UNUSED(row)
    Q_UNUSED(viewModel)

    return getColor();
}

QVariant TableViewColumn::data(int row, const TableViewModel& viewModel)
{
    const QVariant value = sourceData(row, viewModel);
    if(value.isValid())
        return value;
    return m_defaultValue;
}

bool TableViewColumn::setData(int row, const QVariant &value, const TableViewModel& viewModel)
{
    return setSourceData(row, value, viewModel);
}

QVariant TableViewColumn::sourceData(int row, const TableViewModel& viewModel)
{
    if(m_role==-2)
        return viewModel.sourceDatas(row);

    QVariant variant = viewModel.sourceData(row, m_role);

    if(!m_subRoleNames.isEmpty())
    {
        bool ok = true;
        variant = qVariantGetNestedValue(variant, m_subRoleNames, &ok);
        if(!ok) {
            AXIONLOG_DEBUG()<<getRoleName()<<"does not exist";
        }
    }

    if(m_chainedRoleNames.isValid)
    {
        const QVariantList list = variant.toList();
        variant = chainedValueData(m_chainedRoleNames, list, row, viewModel);
    }

    return formatValue(variant);
}

bool TableViewColumn::setSourceData(int row, const QVariant &value, const TableViewModel& viewModel)
{
    return viewModel.setSourceData(row, value, m_role);
}

TableViewColumn::ChainedRoleNames TableViewColumn::parseChainedRoleName(const QString& roleName, const TableViewModel& viewModel, const QString& roleNameSeparator)
{
    ChainedRoleNames chainedRoleNames;
    QRegularExpressionMatch match = regExpChained->match(roleName.trimmed());

    if (match.hasMatch()) {
        chainedRoleNames.listRoleName = match.captured(1);
        QStringList roleNames = chainedRoleNames.listRoleName.split(roleNameSeparator, Qt::SkipEmptyParts);
        const QString roleName = roleNames.takeFirst();
        chainedRoleNames.listRole = viewModel.roleForName(roleName);
        chainedRoleNames.listSubRoleNames = roleNames;

        chainedRoleNames.indexRoleName = match.captured(2);
        QStringList indexRoleNames = chainedRoleNames.indexRoleName.split(roleNameSeparator, Qt::SkipEmptyParts);
        const QString indexRoleName = indexRoleNames.takeFirst();
        chainedRoleNames.indexRole = viewModel.roleForName(indexRoleName);
        chainedRoleNames.indexSubRoleNames = indexRoleNames;

        chainedRoleNames.objectRoleName = match.captured(3);
        chainedRoleNames.objectSubRoleNames = chainedRoleNames.objectRoleName.split(roleNameSeparator, Qt::SkipEmptyParts);

        chainedRoleNames.isValid = true;
    }

    return chainedRoleNames;
}

QVariant TableViewColumn::chainedValueData(const ChainedRoleNames& chainedRoleNames, const QVariantList& list, int row, const TableViewModel& viewModel)
{
    const QVariant& variant = viewModel.sourceData(row, chainedRoleNames.indexRole);

    bool ok = true;
    int index = variant.toInt();
    if(!chainedRoleNames.indexSubRoleNames.isEmpty())
    {
        const QVariant& nestedVariant = qVariantGetNestedValue(variant, chainedRoleNames.indexSubRoleNames, &ok);
        if(!ok)
        {
            AXIONLOG_DEBUG()<<chainedRoleNames.indexRoleName<<"does not exist";
            return "N/A";
        }
        index = nestedVariant.toInt();
    }

    if(index<0 || index>=list.size())
    {
        AXIONLOG_DEBUG()<<index<<"does not exist in"<<chainedRoleNames.listRoleName;
        return "N/A";
    }

    const QVariant object = list.value(index);
    const QVariant& objectVariant = qVariantGetNestedValue(object, chainedRoleNames.objectSubRoleNames, &ok);
    if(!ok)
    {
        AXIONLOG_DEBUG()<<chainedRoleNames.objectSubRoleNames<<"does not exist";
        return "N/A";
    }

    return objectVariant;
}

QVariant TableViewColumn::formatValue(const QVariant& value) const
{
    switch (m_valueType) {
    case TableValueTypes::String:
        return value.toString();
    case TableValueTypes::Integer:
        return value.toInt();
    case TableValueTypes::Double:
        return m_decimals<0 ? value.toDouble() : qFuzzyRound(value.toDouble(), m_decimals);
    case TableValueTypes::Bool:
        return value.toBool();
    case TableValueTypes::Color:
        return value.value<QColor>();
    case TableValueTypes::Date:
        return QLocale().toString(value.toDate(), "yyyy-MM-dd");
    case TableValueTypes::Time:
        return QLocale().toString(value.toTime(), "hh:mm:ss");
    case TableValueTypes::DateTime:
        return QLocale().toString(value.toDateTime(), "yyyy-MM-dd hh:mm:ss");
    case TableValueTypes::List:
        return QString::fromUtf8(QUtils::Json::variantToJson(value));
    case TableValueTypes::Map:
        return QString::fromUtf8(QUtils::Json::variantToJson(value));
    case TableValueTypes::Auto:
        switch (value.userType()) {
        case QMetaType::QDate:
            return QLocale().toString(value.toDate(), "yyyy-MM-dd");
        case QMetaType::QTime:
            return QLocale().toString(value.toTime(), "hh:mm:ss");
        case QMetaType::QDateTime:
            return QLocale().toString(value.toDateTime(), "yyyy-MM-dd hh:mm:ss");
        case QMetaType::Float:
        case QMetaType::Double:
            return m_decimals<0 ? value.toDouble() : qFuzzyRound(value.toDouble(), m_decimals);
        case QMetaType::QByteArray:
            return hex(value.toByteArray());
        default:
            return value;
        }
    default:
        return value;
    }
}

QString TableViewColumn::formattedField(const QString& op) const
{
    QString formatted = m_roleName.split(m_roleNameSeparator, Qt::SkipEmptyParts).join("__");
    formatted.replace('[', "__idx_start__");
    formatted.replace(']', "__idx_end");
    if(!m_roleType.isEmpty())
        formatted.append(QString("__%1").arg(m_roleType.toLower()));
    if(!op.isEmpty())
        formatted.append(QString("__%1").arg(op.toLower()));
    return formatted;
}

const QString& TableViewColumn::getTitle() const
{
    if(m_title.isEmpty())
        return m_roleName;
    return m_title;
}

bool TableViewColumn::setTitle(const QString& title)
{
    if(m_title==title)
        return false;
    m_title = title;
    emit this->titleChanged(m_title);
    return true;
}
