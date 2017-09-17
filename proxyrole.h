#ifndef PROXYROLE_H
#define PROXYROLE_H

#include <QObject>
#include <QMutex>
#include "qqmlsortfilterproxymodel.h"

namespace qqsfpm {

class ProxyRole : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit ProxyRole(QObject *parent = nullptr);

    const QString& name() const;
    void setName(const QString& name);

    QVariant roleData(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel);
    virtual void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel);

protected:
    void invalidate();

Q_SIGNALS:
    void nameAboutToBeChanged();
    void nameChanged();
    void invalidated();

private:
    virtual QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) = 0;

    QString m_name;
    QMutex m_mutex;
};

class JoinRole : public ProxyRole
{
    Q_OBJECT
    Q_PROPERTY(QStringList roleNames READ roleNames WRITE setRoleNames NOTIFY roleNamesChanged)
    Q_PROPERTY(QString separator READ separator WRITE setSeparator NOTIFY separatorChanged)

public:
    using ProxyRole::ProxyRole;

    QStringList roleNames() const;
    void setRoleNames(const QStringList& roleNames);

    QString separator() const;
    void setSeparator(const QString& separator);

Q_SIGNALS:
    void roleNamesChanged();

    void separatorChanged();

private:
    QStringList m_roleNames;
    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;
    QString m_separator = " ";
};

}

#endif // PROXYROLE_H
