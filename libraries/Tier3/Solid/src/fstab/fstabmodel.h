#ifndef FSTABMODEL_H
#define FSTABMODEL_H

#include <QDefs>
#include <QUtils>
#include <QModels>

#include "fstab.h"

class FstabModel: public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_PROPERTY(QList<FstabEntry> entries READ entries NOTIFY updated FINAL)

    Q_READONLY_VAR_PROPERTY(bool, processRunning, ProcessRunning, false)
    Q_READONLY_REF_PROPERTY(QString, currentProcess, CurrentProcess, {})

public:
    explicit FstabModel(QObject* parent=nullptr);

    enum {
        EntryRole = Qt::UserRole,
        FsSpecRole,
        MountPointRole,
        TypeRole,
        OptionsRole,
        DumpRole,
        PassRole
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return rowCount(); };
    int size() const { return rowCount(); };
    int length() const { return rowCount(); };
    bool isEmpty() const { return rowCount() == 0; };

    const QList<FstabEntry>& entries() const;
    FstabEntry entry(int index) const;

    Q_INVOKABLE QVariantMap smbParams(const QString& mountPoint) const;

signals:
    void updated();
    void countChanged(int count);
    void emptyChanged(bool empty);

    void mounted(const QString& hostName, bool result);
    void umounted(const QString& hostName, bool result);
    void smbShareTested(const QString& hostName, bool result);

    void error(const QString& error);

public slots:
    void refresh();

    bool addSmbShare(const QVariantMap& mountParams);
    bool updateSmbShare(const QString& mountPoint, const QVariantMap& mountParams);
    bool removeSmbShare(const QString& mountPoint);
    void testSmbShare(const QVariantMap& mountParams);

    void mount();
    void umount(const QString& mountPoint);

private slots:
    void countInvalidate();

private:
    int m_count=0;
    QHash<int, QByteArray> m_roleNames;

    QList<FstabEntry> m_entries;
};

#endif // FSTABMODEL_H
