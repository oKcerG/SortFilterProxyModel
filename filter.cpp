#include "filter.h"

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
