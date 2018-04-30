#ifndef PROXYROLE_H
#define PROXYROLE_H

#include <QObject>
#include <QMutex>

namespace qqsfpm {

class QQmlSortFilterProxyModel;

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

}

#endif // PROXYROLE_H
