#ifndef SORTERSSORTERCONTAINER_H
#define SORTERSSORTERCONTAINER_H

#include <QList>
#include <QQmlListProperty>

namespace qqsfpm {

class Sorter;
class QQmlSortFilterProxyModel;

class SorterContainer {
public:
    virtual ~SorterContainer();

    QList<Sorter*> sorters() const;
    void appendSorter(Sorter* sorter);
    void removeSorter(Sorter* sorter);
    void clearSorters();

    QQmlListProperty<Sorter> sortersListProperty();

protected:
    QList<Sorter*> m_sorters;

private:
    virtual void onSorterAppended(Sorter* sorter) = 0;
    virtual void onSorterRemoved(Sorter* sorter) = 0;
    virtual void onSortersCleared() = 0;

    static void append_sorter(QQmlListProperty<Sorter>* list, Sorter* sorter);
    static int count_sorter(QQmlListProperty<Sorter>* list);
    static Sorter* at_sorter(QQmlListProperty<Sorter>* list, int index);
    static void clear_sorters(QQmlListProperty<Sorter>* list);
};

}

#define SorterContainer_iid "fr.grecko.SortFilterProxyModel.SorterContainer"
Q_DECLARE_INTERFACE(qqsfpm::SorterContainer, SorterContainer_iid)

#endif // SORTERSSORTERCONTAINER_H
