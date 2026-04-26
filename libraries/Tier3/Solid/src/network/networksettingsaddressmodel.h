#ifndef NETWORKSETTINGSADDRESSMODEL_H
#define NETWORKSETTINGSADDRESSMODEL_H

#include <QDefs>
#include <QStringListModel>

class NetworkSettingsAddressModel : public QStringListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(bool automatic READ automatic NOTIFY automaticChanged FINAL)

public:
    explicit NetworkSettingsAddressModel(QObject *parent = nullptr);
    explicit NetworkSettingsAddressModel(const QStringList &strings, QObject *parent = nullptr);
    Q_INVOKABLE void append(const QString& address);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void resetChanges();
    Q_INVOKABLE void clear();
    Q_INVOKABLE void changeStringList(const QStringList &addresses);
    void setStringList(const QStringList &addresses);
    int count() const;

    bool automatic() const;
    void setAutomatic(bool automatic);

Q_SIGNALS:
    void countChanged();
    void automaticChanged();
private:
    QStringList m_addresses;
    bool m_automatic;
};

#endif // NETWORKSETTINGSADDRESSMODEL_H
