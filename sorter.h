#ifndef SORTER_H
#define SORTER_H

#include <QObject>
#include "qqmlsortfilterproxymodel.h"

namespace qqsfpm {

class Sorter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool ascendingOrder READ ascendingOrder WRITE setAscendingOrder NOTIFY ascendingOrderChanged)
    friend class QQmlSortFilterProxyModel;

public:
    Sorter(QObject* parent = nullptr);
    virtual ~Sorter() = 0;

    bool enabled() const;
    void setEnabled(bool enabled);

    bool ascendingOrder() const;
    void setAscendingOrder(bool ascendingOrder);

    int compareRows(const QModelIndex& source_left, const QModelIndex& source_right) const;

signals:
    void enabledChanged();
    void ascendingOrderChanged();

    void sorterChanged();
    void invalidate();

protected:
    QQmlSortFilterProxyModel* proxyModel() const;

    virtual int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const;
    virtual bool lessThan(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const;
    virtual void proxyModelCompleted();

private slots:
    void onSorterChanged();

private:
    bool m_enabled = true;
    bool m_ascendingOrder = true;
    QQmlSortFilterProxyModel* m_proxyModel = nullptr;
};

class RoleSorter : public Sorter
{
    Q_OBJECT
    Q_PROPERTY(QString roleName READ roleName WRITE setRoleName NOTIFY roleNameChanged)

public:
    using Sorter::Sorter;

    const QString& roleName() const;
    void setRoleName(const QString& roleName);

signals:
    void roleNameChanged();

protected:
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const override;

private:
    QString m_roleName;
};

}

#endif // SORTER_H
