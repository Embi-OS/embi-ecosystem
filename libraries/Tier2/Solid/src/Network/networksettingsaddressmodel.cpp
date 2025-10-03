#include "networksettingsaddressmodel.h"

/*!
    \class NetworkSettingsAddressModel
    \inmodule QtNetworkSettings

    \brief The NetworkSettingsAddressModel class represents a network
    interface address.

    An address model contains a lists of available network interface addresses.

    \sa NetworkSettingsManager
*/

/*!
    \property NetworkSettingsAddressModel::count
    \brief Holds the number of rows in the model.
*/

/*!
    \fn NetworkSettingsAddressModel::countChanged()
    This signal is emitted when the number of rows in the model changes.
*/

/*!
    Creates an address model with the parent \a parent.
*/
NetworkSettingsAddressModel::NetworkSettingsAddressModel(QObject *parent)
    :QStringListModel(parent)
{
}

/*!
    Creates an address model with the addresses specified by
    \a strings and parent \a parent.
*/
NetworkSettingsAddressModel::NetworkSettingsAddressModel(const QStringList &strings, QObject *parent)
    :QStringListModel(parent)
{
    setStringList(strings);
}

/*!
    Adds the addresses specified by \a addresses to the address model.
*/
void NetworkSettingsAddressModel::setStringList(const QStringList &addresses)
{
    m_addresses = addresses;
    QStringListModel::setStringList(m_addresses);
    emit countChanged();
}

/*!
    Adds the address specified by \a address to the address model.
*/
void NetworkSettingsAddressModel::append(const QString& address)
{
    int row = rowCount();

    bool succeed = insertRows(row, 1);
    if (succeed)
        succeed = setData(index(row), QVariant::fromValue(address));

    Q_ASSERT(succeed == true);

    emit countChanged();
}

/*!
    Removes the address at the position specified by \a index from the address
    model.
*/
void NetworkSettingsAddressModel::remove(int index)
{
    removeRows(index, 1);
    emit countChanged();
}

/*!
    Returns the number of rows in the address model.
*/
int NetworkSettingsAddressModel::count() const
{
    return rowCount();
}

/*!
    Resets the changes made to the addresses in the address model.
*/
void NetworkSettingsAddressModel::resetChanges()
{
    QStringListModel::setStringList(m_addresses);
}

/*!
    Clear all the addresses in the address model.
*/
void NetworkSettingsAddressModel::clear()
{
    QStringListModel::setStringList(QStringList());
    emit countChanged();
}

/*!
    Set the addresses to \a addresses in the address model.
*/
void NetworkSettingsAddressModel::changeStringList(const QStringList &addresses)
{
    QStringListModel::setStringList(addresses);
    emit countChanged();
}


bool NetworkSettingsAddressModel::automatic() const
{
    return m_automatic;
}
void NetworkSettingsAddressModel::setAutomatic(bool automatic)
{
    if(m_automatic==automatic)
        return;
    m_automatic = automatic;
    emit automaticChanged();
}
