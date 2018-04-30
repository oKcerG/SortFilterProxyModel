#include "proxyrole.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlExpression>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlInfo>
#include "filters/filter.h"
#include "qqmlsortfilterproxymodel.h"

namespace qqsfpm {

/*!
    \qmltype ProxyRole
    \inqmlmodule SortFilterProxyModel
    \brief Base type for the \l SortFilterProxyModel proxy roles

    The ProxyRole type cannot be used directly in a QML file.
    It exists to provide a set of common properties and methods,
    available across all the other proxy role types that inherit from it.
    Attempting to use the ProxyRole type directly will result in an error.
*/
ProxyRole::ProxyRole(QObject *parent) : QObject(parent)
{
}

/*!
    \qmlproperty string ProxyRole::name

    This property holds the role name of the proxy role.
*/
const QString& ProxyRole::name() const
{
    return m_name;
}

void ProxyRole::setName(const QString& name)
{
    if (m_name == name)
        return;

    Q_EMIT nameAboutToBeChanged();
    m_name = name;
    Q_EMIT nameChanged();
}

QVariant ProxyRole::roleData(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel)
{
    if (m_mutex.tryLock()) {
        QVariant result = data(sourceIndex, proxyModel);
        m_mutex.unlock();
        return result;
    } else {
        return QVariant{};
    }
}

void ProxyRole::proxyModelCompleted(const QQmlSortFilterProxyModel &proxyModel)
{
    Q_UNUSED(proxyModel)
}

void ProxyRole::invalidate()
{
    Q_EMIT invalidated();
}








}
