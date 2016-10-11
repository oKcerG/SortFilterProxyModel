#include "sorter.h"
#include <QtQml>

namespace qqsfpm {

Sorter::Sorter(QObject *parent) : QObject(parent)
{
    connect(this, &Sorter::sorterChanged, this, &Sorter::onSorterChanged);
}

Sorter::~Sorter() = default;

bool Sorter::enabled() const
{
    return m_enabled;
}

void Sorter::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    emit enabledChanged();
    emit sorterChanged();
}

bool Sorter::ascendingOrder() const
{
    return m_ascendingOrder;
}

void Sorter::setAscendingOrder(bool ascendingOrder)
{
    if (m_ascendingOrder == ascendingOrder)
        return;

    m_ascendingOrder = ascendingOrder;
    emit ascendingOrderChanged();
    emit sorterChanged();
}

int Sorter::compareRows(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    int comparison = compare(source_left, source_right);
    return m_ascendingOrder ? comparison : -comparison;
}

QQmlSortFilterProxyModel* Sorter::proxyModel() const
{
    return m_proxyModel;
}

int Sorter::compare(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    if (lessThan(sourceLeft, sourceRight))
        return -1;
    if (lessThan(sourceRight, sourceLeft))
        return 1;
    return 0;
}

bool Sorter::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    Q_UNUSED(sourceLeft)
    Q_UNUSED(sourceRight)
    return false;
}

void Sorter::proxyModelCompleted()
{

}

void Sorter::onSorterChanged()
{
    if (m_enabled)
        emit invalidate();
}

const QString& RoleSorter::roleName() const
{
    return m_roleName;
}

void RoleSorter::setRoleName(const QString& roleName)
{
    if (m_roleName == roleName)
        return;

    m_roleName = roleName;
    emit roleNameChanged();
    emit sorterChanged();
}

int RoleSorter::compare(const QModelIndex &sourceLeft, const QModelIndex& sourceRight) const
{
    QVariant leftValue = proxyModel()->sourceData(sourceLeft, m_roleName);
    QVariant rightValue = proxyModel()->sourceData(sourceRight, m_roleName);
    if (leftValue < rightValue)
        return -1;
    if (leftValue > rightValue)
        return 1;
    return 0;
}

}
