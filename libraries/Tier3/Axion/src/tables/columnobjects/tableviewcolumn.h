#ifndef TABLEVIEWCOLUMN_H
#define TABLEVIEWCOLUMN_H

#include <QModels>
#include <QDefs>

Q_ENUM_CLASS(TableViewColumnDelegates, TableViewColumnDelegate,
             Text,
             Dot,
             Color,)

Q_ENUM_CLASS(TableValueTypes, TableValueType,
             Auto,
             String,
             Integer,
             Double,
             Bool,
             Color,
             Date,
             Time,
             DateTime,
             List,
             Map,)

class TableViewModel;
class TableViewColumn : public QObject,
                        public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    Q_WRITABLE_VAR_PROPERTY(bool, visible, Visible, true)
    Q_WRITABLE_VAR_PROPERTY(bool, essential, Essential, false)
    Q_WRITABLE_VAR_PROPERTY(int, width, Width, -1)
    Q_WRITABLE_VAR_PROPERTY(bool, resizable, Resizable, true)
    Q_WRITABLE_VAR_PROPERTY(bool, sortable, Sortable, true)
    Q_WRITABLE_VAR_PROPERTY(bool, filterable, Filterable, true)

    Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged FINAL)
    Q_WRITABLE_REF_PROPERTY(QString, icon, Icon, "")
    Q_WRITABLE_REF_PROPERTY(QColor, color, Color, {})
    Q_WRITABLE_REF_PROPERTY(QFont, font, Font, {})
    Q_WRITABLE_VAR_PROPERTY(TableViewColumnDelegates::Enum, delegate, Delegate, TableViewColumnDelegates::Text)
    Q_WRITABLE_VAR_PROPERTY(TableValueTypes::Enum, valueType, ValueType, TableValueTypes::Auto)
    Q_WRITABLE_VAR_PROPERTY(int, decimals, Decimals, 6)

    Q_WRITABLE_REF_PROPERTY(QString, roleName, RoleName, "")
    Q_READONLY_REF_PROPERTY(QStringList, subRoleNames, SubRoleNames, {})
    Q_WRITABLE_REF_PROPERTY(QString, roleNameSeparator, RoleNameSeparator, ".")
    Q_WRITABLE_VAR_PROPERTY(int, role, Role, Qt::DisplayRole)
    Q_WRITABLE_REF_PROPERTY(QString, roleType, RoleType, "")

    Q_WRITABLE_REF_PROPERTY(QVariant, defaultValue, DefaultValue, {})

public:
    explicit TableViewColumn(QObject* parent = nullptr);

    void classBegin() override {};
    void componentComplete() override {};

    virtual void viewModelCompleted(const TableViewModel& viewModel);
    virtual void updateRoles(const TableViewModel& viewModel);

    virtual QColor color(int row, const TableViewModel& viewModel);
    virtual QVariant data(int row, const TableViewModel& viewModel);
    virtual bool setData(int row, const QVariant &value, const TableViewModel& viewModel);

    const QString& getTitle() const;

    Q_INVOKABLE QVariant formatValue(const QVariant& value) const;
    Q_INVOKABLE QString formattedField(const QString& op=QString()) const;

public slots:
    bool setTitle(const QString& title);

signals:
    void invalidated();
    void titleChanged(const QString& title);

protected:
    void invalidate();

    QVariant sourceData(int row, const TableViewModel& viewModel);
    bool setSourceData(int row, const QVariant &value, const TableViewModel& viewModel);

    struct ChainedRoleNames {
        bool isValid = false;

        QString listRoleName;
        QStringList listSubRoleNames;
        int listRole=-1;
        QString indexRoleName;
        QStringList indexSubRoleNames;
        int indexRole=-1;
        QString objectRoleName;
        QStringList objectSubRoleNames;
    };
    static ChainedRoleNames parseChainedRoleName(const QString& roleName, const TableViewModel& viewModel, const QString& roleNameSeparator);
    static QVariant chainedValueData(const ChainedRoleNames& chainedRoleNames, const QVariantList& list, int row, const TableViewModel& viewModel);

private:
    QString m_title;

    ChainedRoleNames m_chainedRoleNames;
};

#endif // TABLEVIEWCOLUMN_H
