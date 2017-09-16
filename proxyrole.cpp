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

QStringList JoinRole::roleNames() const
{
    return m_roleNames;
}

void JoinRole::setRoleNames(const QStringList& roleNames)
{
    if (m_roleNames == roleNames)
        return;

    m_roleNames = roleNames;
    Q_EMIT roleNamesChanged();
    invalidate();
}

QString JoinRole::separator() const
{
    return m_separator;
}

void JoinRole::setSeparator(const QString& separator)
{
    if (m_separator == separator)
        return;

    m_separator = separator;
    Q_EMIT separatorChanged();
    invalidate();
}

QVariant JoinRole::data(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel &proxyModel)
{
    QString result;

    for (const QString& roleName : m_roleNames)
        result += proxyModel.sourceData(sourceIndex, roleName).toString() + m_separator;

    if (!m_roleNames.isEmpty())
        result.chop(m_separator.length());

    return result;
}

void registerProxyRoleTypes() {
    qmlRegisterUncreatableType<ProxyRole>("SortFilterProxyModel", 0, 2, "ProxyRole", "ProxyRole is an abstract class");
    qmlRegisterType<JoinRole>("SortFilterProxyModel", 0, 2, "JoinRole");
}

Q_COREAPP_STARTUP_FUNCTION(registerProxyRoleTypes)

}
