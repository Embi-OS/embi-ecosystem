#include "tableviewpatterncolumn.h"
#include "tableviewmodel.h"
#include "axion_log.h"

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, regExpPattern, (R"(\?<(.*?)(?:,(\d+))?>)"))

TableViewPatternColumn::TableViewPatternColumn(QObject* parent):
    TableViewColumn(parent)
{
    connect(this, &TableViewPatternColumn::patternChanged, this, &TableViewPatternColumn::invalidate);
}

void TableViewPatternColumn::updateRoles(const TableViewModel& viewModel)
{
    QRegularExpressionMatchIterator i = regExpPattern->globalMatch(m_pattern);

    m_string = m_pattern;
    m_captures.clear();
    m_roleNames.clear();
    m_subRoleNames.clear();
    m_roles.clear();
    m_precisions.clear();

    // Parcourir chaque correspondance
    int index = 1;  // Index de départ pour %1, %2, etc.
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        const QString capture = match.captured(0);  // Capturer le terme dans la chaîne originale
        const QString name = match.captured(1);  // Capturer le texte entre "< >"
        const QString precision = match.captured(2);  // Capturer la précision (si elle existe)

        QStringList roleNames = name.split(m_roleNameSeparator, Qt::SkipEmptyParts);
        const QString roleName = roleNames.takeFirst();
        int role = viewModel.roleForName(roleName);

        m_captures.append(capture);
        m_roleNames.append(roleName);
        m_subRoleNamesNames.append(roleNames);
        m_roles.append(role);
        m_precisions.append(precision.isEmpty()?-1:precision.toUInt());

        m_string.replace(capture, "%" % QString::number(index));
        index++;
    }

    invalidate();
}

QVariant TableViewPatternColumn::data(int row, const TableViewModel& viewModel)
{
    QString result = m_string;

    for (int i = 0; i < m_roles.size(); ++i) {
        int role = m_roles[i];
        if(role<0) {
            result = result.arg(m_captures[i]);
        }
        else {
            QVariant value = viewModel.sourceData(row, role);
            if(!m_subRoleNamesNames.at(i).isEmpty())
            {
                bool ok = true;
                value = qVariantGetNestedValue(value, m_subRoleNamesNames.at(i), &ok);
                if(!ok) {
                    AXIONLOG_DEBUG()<<getRoleName()<<"does not exist";
                }
            }

            int precision = m_precisions[i];
            if(precision<0)
                result = result.arg(value.toString());
            else
                result = result.arg(value.toDouble(), 0, 'f', precision);
        }
    }

    return result;
}
