#include "filter.h"
#include <QtQml>

namespace qqsfpm {

Filter::Filter(QObject *parent) : QObject(parent)
{
    connect(this, &Filter::filterChanged, this, &Filter::onFilterChanged);
}

bool Filter::enabled() const
{
    return m_enabled;
}

void Filter::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    emit enabledChanged();
    emit filterChanged();
}

bool Filter::inverted() const
{
    return m_inverted;
}

void Filter::setInverted(bool inverted)
{
    if (m_inverted == inverted)
        return;

    m_inverted = inverted;
    emit invertedChanged();
    emit filterChanged();
}

bool Filter::filterAcceptsRow(const QModelIndex &sourceIndex) const
{
    return !m_enabled || filterRow(sourceIndex) ^ m_inverted;
}

QQmlSortFilterProxyModel* Filter::proxyModel() const
{
    return m_proxyModel;
}

void Filter::proxyModelCompleted()
{

}

void Filter::onFilterChanged()
{
    if (m_enabled)
        invalidate();
}

const QString& RoleFilter::roleName() const
{
    return m_roleName;
}

void RoleFilter::setRoleName(const QString& roleName)
{
    if (m_roleName == roleName)
        return;

    m_roleName = roleName;
    emit roleNameChanged();
    emit filterChanged();
}

QVariant RoleFilter::sourceData(const QModelIndex &sourceIndex) const
{
    return proxyModel()->sourceData(sourceIndex, m_roleName);
}

const QVariant &ValueFilter::value() const
{
    return m_value;
}

void ValueFilter::setValue(const QVariant& value)
{
    if (m_value == value)
        return;

    m_value = value;
    emit valueChanged();
    emit filterChanged();
}

bool ValueFilter::filterRow(const QModelIndex& sourceIndex) const
{
    return !m_value.isValid() || m_value == sourceData(sourceIndex);
}

int IndexFilter::minimumIndex() const
{
    return m_minimumIndex;
}

void IndexFilter::setMinimumIndex(int minimumIndex)
{
    if (m_minimumIndex == minimumIndex)
        return;

    m_minimumIndex = minimumIndex;
    m_minimumIndexIsSet = true;
    emit minimumIndexChanged();
    emit filterChanged();
}

void IndexFilter::resetMinimumIndex()
{
    m_minimumIndexIsSet = false;
    emit filterChanged();
}

int IndexFilter::maximumIndex() const
{
    return m_maximumIndex;
}

void IndexFilter::setMaximumIndex(int maximumIndex)
{
    if (m_maximumIndex == maximumIndex)
        return;

    m_maximumIndex = maximumIndex;
    m_maximumIndexIsSet = true;
    emit maximumIndexChanged();
    emit filterChanged();
}

void IndexFilter::resetMaximumIndex()
{
    m_maximumIndexIsSet = false;
    emit filterChanged();
}

bool IndexFilter::filterRow(const QModelIndex &sourceIndex) const
{
    int sourceRow = sourceIndex.row();
    return (!m_minimumIndexIsSet || sourceRow >= m_minimumIndex) && (!m_maximumIndexIsSet || sourceRow <= m_maximumIndex);
}

void registerFilterTypes() {
    qmlRegisterUncreatableType<Filter>("SortFilterProxyModel", 0, 2, "Filter", "Filter is an abstract class");
    qmlRegisterType<ValueFilter>("SortFilterProxyModel", 0, 2, "ValueFilter");
    qmlRegisterType<IndexFilter>("SortFilterProxyModel", 0, 2, "IndexFilter");
}

Q_COREAPP_STARTUP_FUNCTION(registerFilterTypes)

}
