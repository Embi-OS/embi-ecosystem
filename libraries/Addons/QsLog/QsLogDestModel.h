#ifndef QSLOGDESTMODEL_H
#define QSLOGDESTMODEL_H

#include <QAbstractListModel>
#include <QReadWriteLock>
#include <QString>
#include <QQmlEngine>

#include "QsLogMessage.h"
#include "QsLogDest.h"

namespace QsLogging
{

class ModelDestination : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ size NOTIFY countChanged FINAL)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_PROPERTY(bool delayed READ delayed WRITE setDelayed NOTIFY delayedChanged FINAL)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged FINAL)

    Q_PROPERTY(QsLogging::ModelDestination::Level logLevel READ getLogLevel WRITE setLogLevel NOTIFY logLevelChanged FINAL)
    Q_PROPERTY(int limit READ getLimit WRITE setLimit NOTIFY limitChanged FINAL)
    Q_PROPERTY(QsLogging::Message lastMessage READ getLastMessage NOTIFY lastMessageChanged FINAL)

public:
    explicit ModelDestination(QObject *parent = nullptr);
    virtual ~ModelDestination();

    enum {
        MessageRole = Qt::UserRole,
        TimeRole,
        LevelRole,
        FileRole,
        FunctionRole,
        CategoryRole,
        LineRole,
        ColorRole,
    };

    enum Level {
        NoticeLevel = QsLogging::Level::NoticeLevel,
        TraceLevel = QsLogging::Level::TraceLevel,
        DebugLevel = QsLogging::Level::DebugLevel,
        InfoLevel = QsLogging::Level::InfoLevel,
        WarnLevel = QsLogging::Level::WarnLevel,
        CritLevel = QsLogging::Level::CritLevel,
        FatalLevel = QsLogging::Level::FatalLevel,
        OffLevel = QsLogging::Level::OffLevel,
    };
    Q_ENUM (Level)

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return rowCount(); };
    int size() const { return rowCount(); };
    int length() const { return rowCount(); };
    bool isEmpty() const { return rowCount() == 0; };

    Q_INVOKABLE QsLogging::Message at(int index) const;

    bool delayed() const;
    void setDelayed(bool delayed);

    bool enabled() const;
    void setEnabled(bool enabled);

    Level getLogLevel() const;
    void setLogLevel(Level logLevel);

    int getLimit() const;
    void setLimit(int limit);

    const QsLogging::Message& getLastMessage() const;
    void setLastMessage(const QsLogging::Message& lastMessage);

public slots:
    void addEntry(const QsLogging::Message& message);
    void clear();

    virtual void queueInvalidate();
    virtual void invalidate();

signals:
    void countChanged(int count);
    void emptyChanged(bool empty);
    void delayedChanged(bool delayed);
    void enabledChanged(bool enabled);

    void limitChanged(int limit);
    void logLevelChanged(QsLogging::ModelDestination::Level logLevel);
    void lastMessageChanged(const QsLogging::Message &lastMessage);

private slots:
    void countInvalidate();

private:
    int m_count=0;
    bool m_delayed = false;
    bool m_enabled = true;
    bool m_invalidateQueued = false;

    QHash<int, QByteArray> m_roleNames;
    QList<Message> m_messages;

    Level m_logLevel;
    int m_limit;
    QsLogging::Message m_lastMessage;

    DestinationPtr m_functorDest=nullptr;
};

}

#endif // QSLOGDESTMODEL_H
