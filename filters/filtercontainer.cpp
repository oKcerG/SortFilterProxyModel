#include "filtercontainer.h"

namespace qqsfpm {

FilterContainer::~FilterContainer()
{

}

QList<Filter*> FilterContainer::filters() const
{
    return m_filters;
}

void FilterContainer::appendFilter(Filter* filter)
{
    m_filters.append(filter);
    onFilterAppended(filter);
}

void FilterContainer::removeFilter(Filter* filter)
{
    m_filters.removeOne(filter);
    onFilterRemoved(filter);
}

void FilterContainer::clearFilters()
{
    m_filters.clear();
    onFiltersCleared();
}

QQmlListProperty<Filter> FilterContainer::filtersListProperty()
{
    return QQmlListProperty<Filter>(reinterpret_cast<QObject*>(this), &m_filters,
                                    &FilterContainer::append_filter,
                                    &FilterContainer::count_filter,
                                    &FilterContainer::at_filter,
                                    &FilterContainer::clear_filters);
}

void FilterContainer::append_filter(QQmlListProperty<Filter>* list, Filter* filter)
{
    if (!filter)
        return;

    FilterContainer* that = reinterpret_cast<FilterContainer*>(list->object);
    that->appendFilter(filter);
}

int FilterContainer::count_filter(QQmlListProperty<Filter>* list)
{
    QList<Filter*>* filters = static_cast<QList<Filter*>*>(list->data);
    return filters->count();
}

Filter* FilterContainer::at_filter(QQmlListProperty<Filter>* list, int index)
{
    QList<Filter*>* filters = static_cast<QList<Filter*>*>(list->data);
    return filters->at(index);
}

void FilterContainer::clear_filters(QQmlListProperty<Filter> *list)
{
    FilterContainer* that = reinterpret_cast<FilterContainer*>(list->object);
    that->clearFilters();
}

}
