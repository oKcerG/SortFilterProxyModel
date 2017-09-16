#include "proxyrole.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlExpression>
#include <QCoreApplication>
#include <QDebug>

namespace qqsfpm {

ProxyRole::ProxyRole(QObject *parent) : QObject(parent)
{
}

const QString& ProxyRole::name() const
{
    return m_name;
}

void ProxyRole::setName(const QString& name)
{
    if (m_name == name)
        return;

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

void ProxyRole::invalidate()
{
    Q_EMIT invalidated();
}


void registerProxyRoleTypes() {
    qmlRegisterUncreatableType<ProxyRole>("SortFilterProxyModel", 0, 2, "ProxyRole", "ProxyRole is an abstract class");
}

Q_COREAPP_STARTUP_FUNCTION(registerProxyRoleTypes)

}
