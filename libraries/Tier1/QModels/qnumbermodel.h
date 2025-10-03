#ifndef QNUMBERMODEL_H
#define QNUMBERMODEL_H

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <QLocale>
#include <QDefs>

class QNumberModel: public QAbstractListModel,
                    public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(NumberModel)
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)

    Q_WRITABLE_FUZ_PROPERTY(double, from, From, 0.0)
    Q_WRITABLE_FUZ_PROPERTY(double, to, To, 0.0)
    Q_WRITABLE_FUZ_PROPERTY(double, stepSize, StepSize, 1.0)
    Q_WRITABLE_VAR_PROPERTY(QLocale::NumberOptions, formattingOptions, FormattingOptions, QLocale::DefaultNumberOptions)

public:
    explicit QNumberModel(QObject *parent = nullptr);

    enum Roles {
        DisplayRole = Qt::DisplayRole,
        ValueRole = Qt::UserRole,
    };

    void classBegin() override;
    void componentComplete() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    double determinateValue(const QModelIndex &index) const;
    int determinateCount() const;

    int count() const { return rowCount(); };
    int size() const { return count(); };
    int length() const { return count(); };
    bool isEmpty() const { return count() == 0; };

public slots:
    void queueInvalidate();
    void invalidate();

signals:
    void countChanged(int count);
    void emptyChanged(bool empty);

protected:
    void countInvalidate();

private:
    int m_count=0;
    QHash<int, QByteArray> m_roleNames;
    int m_rowCount=0;

    bool m_completed = false;
    bool m_invalidateQueued = false;
    double m_minimumValue = 0.0;
    double m_maximumValue = 0.0;
    double m_stepValue = 1.0;
};

#endif // QNUMBERMODEL_H
