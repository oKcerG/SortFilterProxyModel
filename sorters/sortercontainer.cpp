#include "sortercontainer.h"

namespace qqsfpm {

SorterContainer::~SorterContainer()
{

}

QList<Sorter*> SorterContainer::sorters() const
{
    return m_sorters;
}

void SorterContainer::appendSorter(Sorter* sorter)
{
    m_sorters.append(sorter);
    onSorterAppended(sorter);
}

void SorterContainer::removeSorter(Sorter *sorter)
{
    m_sorters.removeOne(sorter);
    onSorterRemoved(sorter);
}

void SorterContainer::clearSorters()
{
    m_sorters.clear();
    onSortersCleared();
}

QQmlListProperty<Sorter> SorterContainer::sortersListProperty()
{
    return QQmlListProperty<Sorter>(reinterpret_cast<QObject*>(this), &m_sorters,
                                    &SorterContainer::append_sorter,
                                    &SorterContainer::count_sorter,
                                    &SorterContainer::at_sorter,
                                    &SorterContainer::clear_sorters);
}

void SorterContainer::append_sorter(QQmlListProperty<Sorter>* list, Sorter* sorter)
{
    if (!sorter)
        return;

    SorterContainer* that = reinterpret_cast<SorterContainer*>(list->object);
    that->appendSorter(sorter);
}

int SorterContainer::count_sorter(QQmlListProperty<Sorter>* list)
{
    QList<Sorter*>* sorters = static_cast<QList<Sorter*>*>(list->data);
    return sorters->count();
}

Sorter* SorterContainer::at_sorter(QQmlListProperty<Sorter>* list, int index)
{
    QList<Sorter*>* sorters = static_cast<QList<Sorter*>*>(list->data);
    return sorters->at(index);
}

void SorterContainer::clear_sorters(QQmlListProperty<Sorter> *list)
{
    SorterContainer* that = reinterpret_cast<SorterContainer*>(list->object);
    that->clearSorters();
}

}
