#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QDefs>
#include <QFileSystemModel>

class FileSystemModel: public QFileSystemModel,
                       public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    Q_WRITABLE_REF_PROPERTY(QString, path, Path, {})
    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, true)
    Q_READONLY_VAR_PROPERTY(QModelIndex, rootIndex, RootIndex, {})

    Q_WRITABLE_REF_PROPERTY(QUrl, rootUrl, RootUrl, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, nameFilters, NameFilters, {})
    Q_WRITABLE_VAR_PROPERTY(bool, showFiles, ShowFiles, true)
    Q_WRITABLE_VAR_PROPERTY(bool, showDirs, ShowDirs, true)
    Q_WRITABLE_VAR_PROPERTY(bool, showDotAndDotDot, ShowDotAndDotDot, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showHidden, ShowHidden, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showOnlyReadable, ShowOnlyReadable, false)
    Q_WRITABLE_VAR_PROPERTY(bool, caseSensitive, CaseSensitive, true)

public:
    explicit FileSystemModel(QObject* parent=nullptr);

    void classBegin() override final;
    void componentComplete() override final;

    enum Roles  {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole = Qt::UserRole + 2,
        FileBaseNameRole = Qt::UserRole + 3,
        FileSuffixRole = Qt::UserRole + 4,
        FileSizeRole = Qt::UserRole + 5,
        FileLastModifiedRole = Qt::UserRole + 6,
        FileLastReadRole = Qt::UserRole +7,
        FileIsDirRole = Qt::UserRole + 8,
        FileUrlRole = Qt::UserRole + 9
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override final;
    QHash<int,QByteArray> roleNames() const override final;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override final;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

public slots:
    void queueInvalidate();
    void invalidate();

private slots:
    void queueInvalidateFilter();
    void invalidateFilter();

private:
    bool m_completed=false;
    bool m_invalidateQueued = false;
    bool m_filterInvalidateQueued = false;
    QHash<int, QByteArray> m_roleNames;
};

#endif // FILESYSTEMMODEL_H
