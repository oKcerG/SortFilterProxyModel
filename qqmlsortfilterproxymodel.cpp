#include "qqmlsortfilterproxymodel.h"
#include <QtQml>
#include <algorithm>
#include "filter.h"
#include "sorter.h"

namespace qqsfpm {

QQmlSortFilterProxyModel::QQmlSortFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    connect(this, &QAbstractProxyModel::sourceModelChanged, this, &QQmlSortFilterProxyModel::updateRoles);
    connect(this, &QAbstractItemModel::modelReset, this, &QQmlSortFilterProxyModel::updateRoles);
    connect(this, &QAbstractItemModel::rowsInserted, this, &QQmlSortFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &QQmlSortFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::modelReset, this, &QQmlSortFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::layoutChanged, this, &QQmlSortFilterProxyModel::countChanged);
    setDynamicSortFilter(true);
}

int QQmlSortFilterProxyModel::count() const
{
    return rowCount();
}

const QString& QQmlSortFilterProxyModel::filterRoleName() const
{
    return m_filterRoleName;
}

void QQmlSortFilterProxyModel::setFilterRoleName(const QString& filterRoleName)
{
    if (m_filterRoleName == filterRoleName)
        return;

    m_filterRoleName = filterRoleName;
    updateFilterRole();
    Q_EMIT filterRoleNameChanged();
}

QString QQmlSortFilterProxyModel::filterPattern() const
{
    return filterRegExp().pattern();
}

void QQmlSortFilterProxyModel::setFilterPattern(const QString& filterPattern)
{
    QRegExp regExp = filterRegExp();
    if (regExp.pattern() == filterPattern)
        return;

    regExp.setPattern(filterPattern);
    QSortFilterProxyModel::setFilterRegExp(regExp);
    Q_EMIT filterPatternChanged();
}

QQmlSortFilterProxyModel::PatternSyntax QQmlSortFilterProxyModel::filterPatternSyntax() const
{
    return static_cast<PatternSyntax>(filterRegExp().patternSyntax());
}

void QQmlSortFilterProxyModel::setFilterPatternSyntax(QQmlSortFilterProxyModel::PatternSyntax patternSyntax)
{
    QRegExp regExp = filterRegExp();
    QRegExp::PatternSyntax patternSyntaxTmp = static_cast<QRegExp::PatternSyntax>(patternSyntax);
    if (regExp.patternSyntax() == patternSyntaxTmp)
        return;

    regExp.setPatternSyntax(patternSyntaxTmp);
    QSortFilterProxyModel::setFilterRegExp(regExp);
    Q_EMIT filterPatternSyntaxChanged();
}

const QVariant& QQmlSortFilterProxyModel::filterValue() const
{
    return m_filterValue;
}

void QQmlSortFilterProxyModel::setFilterValue(const QVariant& filterValue)
{
    if (m_filterValue == filterValue)
        return;

    m_filterValue = filterValue;
    invalidateFilter();
    Q_EMIT filterValueChanged();
}

const QString& QQmlSortFilterProxyModel::sortRoleName() const
{
    return m_sortRoleName;
}

void QQmlSortFilterProxyModel::setSortRoleName(const QString& sortRoleName)
{
    if (m_sortRoleName == sortRoleName)
        return;

    m_sortRoleName = sortRoleName;
    updateSortRole();
    Q_EMIT sortRoleNameChanged();
}

bool QQmlSortFilterProxyModel::ascendingSortOrder() const
{
    return m_ascendingSortOrder;
}

void QQmlSortFilterProxyModel::setAscendingSortOrder(bool ascendingSortOrder)
{
    if (m_ascendingSortOrder == ascendingSortOrder)
        return;

    m_ascendingSortOrder = ascendingSortOrder;
    Q_EMIT ascendingSortOrderChanged();
    invalidate();
}

QQmlListProperty<Filter> QQmlSortFilterProxyModel::filters()
{
    return QQmlListProperty<Filter>(this, &m_filters,
                                    &QQmlSortFilterProxyModel::append_filter,
                                    &QQmlSortFilterProxyModel::count_filter,
                                    &QQmlSortFilterProxyModel::at_filter,
                                    &QQmlSortFilterProxyModel::clear_filters);
}

QQmlListProperty<Sorter> QQmlSortFilterProxyModel::sorters()
{
    return QQmlListProperty<Sorter>(this, &m_sorters,
                                    &QQmlSortFilterProxyModel::append_sorter,
                                    &QQmlSortFilterProxyModel::count_sorter,
                                    &QQmlSortFilterProxyModel::at_sorter,
                                    &QQmlSortFilterProxyModel::clear_sorters);
}

void QQmlSortFilterProxyModel::classBegin()
{

}

void QQmlSortFilterProxyModel::componentComplete()
{
    m_completed = true;
    for (const auto& filter : m_filters)
        filter->proxyModelCompleted();
    invalidate();
    sort(0);
}

QVariant QQmlSortFilterProxyModel::sourceData(const QModelIndex& sourceIndex, const QString& roleName) const
{
    int role = sourceModel()->roleNames().key(roleName.toUtf8());
    return sourceData(sourceIndex, role);
}

QVariant QQmlSortFilterProxyModel::sourceData(const QModelIndex &sourceIndex, int role) const
{
    return sourceModel()->data(sourceIndex, role);
}

int QQmlSortFilterProxyModel::roleForName(const QString& roleName) const
{
    return roleNames().key(roleName.toUtf8(), -1);
}

QVariantMap QQmlSortFilterProxyModel::get(int row) const
{
    QVariantMap map;
    QModelIndex modelIndex = index(row, 0);
    QHash<int, QByteArray> roles = roleNames();
    for (QHash<int, QByteArray>::const_iterator it = roles.begin(); it != roles.end(); ++it)
        map.insert(it.value(), data(modelIndex, it.key()));
    return map;
}

QVariant QQmlSortFilterProxyModel::get(int row, const QString& roleName) const
{
    return data(index(row, 0), roleForName(roleName));
}

QModelIndex QQmlSortFilterProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
    return QSortFilterProxyModel::mapToSource(proxyIndex);
}

int QQmlSortFilterProxyModel::mapToSource(int proxyRow) const
{
    QModelIndex proxyIndex = index(proxyRow, 0);
    QModelIndex sourceIndex = mapToSource(proxyIndex);
    return sourceIndex.isValid() ? sourceIndex.row() : -1;
}

QModelIndex QQmlSortFilterProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    return QSortFilterProxyModel::mapFromSource(sourceIndex);
}

int QQmlSortFilterProxyModel::mapFromSource(int sourceRow) const
{
    QModelIndex proxyIndex;
    if (QAbstractItemModel* source = sourceModel()) {
        QModelIndex sourceIndex = source->index(sourceRow, 0);
        proxyIndex = mapFromSource(sourceIndex);
    }
    return proxyIndex.isValid() ? proxyIndex.row() : -1;
}

bool QQmlSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    if (!m_completed)
        return true;
    QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
    bool valueAccepted = !m_filterValue.isValid() || ( m_filterValue == sourceModel()->data(sourceIndex, filterRole()) );
    bool baseAcceptsRow = valueAccepted && QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    baseAcceptsRow = baseAcceptsRow && std::all_of(m_filters.begin(), m_filters.end(),
        [=, &source_parent] (Filter* filter) {
            return filter->filterAcceptsRow(sourceIndex);
        }
    );
    return baseAcceptsRow;
}

bool QQmlSortFilterProxyModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
    if (m_completed) {
        if (!m_sortRoleName.isEmpty()) {
            if (QSortFilterProxyModel::lessThan(source_left, source_right))
                return m_ascendingSortOrder;
            if (QSortFilterProxyModel::lessThan(source_right, source_left))
                return !m_ascendingSortOrder;
        }
        for(auto sorter : m_sorters) {
            if (sorter->enabled()) {
                int comparison = sorter->compareRows(source_left, source_right);
                if (comparison != 0)
                    return comparison < 0;
            }
        }
    }
    return source_left.row() < source_right.row();
}

void QQmlSortFilterProxyModel::resetInternalData()
{
    QSortFilterProxyModel::resetInternalData();
    if (roleNames().isEmpty()) // workaround for when a model has no roles and roles are added when the model is populated (ListModel)
        connect(this, &QAbstractItemModel::rowsAboutToBeInserted, this, &QQmlSortFilterProxyModel::initRoles);
}

void QQmlSortFilterProxyModel::invalidateFilter()
{
    if (m_completed)
        QSortFilterProxyModel::invalidateFilter();
}

void QQmlSortFilterProxyModel::invalidate()
{
    if (m_completed)
        QSortFilterProxyModel::invalidate();
}

void QQmlSortFilterProxyModel::updateFilterRole()
{
    QList<int> filterRoles = roleNames().keys(m_filterRoleName.toUtf8());
    if (!filterRoles.empty())
    {
        setFilterRole(filterRoles.first());
    }
}

void QQmlSortFilterProxyModel::updateSortRole()
{
    QList<int> sortRoles = roleNames().keys(m_sortRoleName.toUtf8());
    if (!sortRoles.empty())
    {
        setSortRole(sortRoles.first());
        invalidate();
    }
}

void QQmlSortFilterProxyModel::updateRoles()
{
    updateFilterRole();
    updateSortRole();
}

void QQmlSortFilterProxyModel::initRoles()
{
    disconnect(this, &QAbstractItemModel::rowsAboutToBeInserted, this , &QQmlSortFilterProxyModel::initRoles);
    resetInternalData();
}

QVariantMap QQmlSortFilterProxyModel::modelDataMap(const QModelIndex& modelIndex) const
{
    QVariantMap map;
    QHash<int, QByteArray> roles = roleNames();
    for (QHash<int, QByteArray>::const_iterator it = roles.begin(); it != roles.end(); ++it)
        map.insert(it.value(), sourceModel()->data(modelIndex, it.key()));
    return map;
}

void QQmlSortFilterProxyModel::append_filter(QQmlListProperty<Filter>* list, Filter* filter)
{
    if (!filter)
        return;

    QQmlSortFilterProxyModel* that = static_cast<QQmlSortFilterProxyModel*>(list->object);
    that->m_filters.append(filter);
    connect(filter, &Filter::invalidate, that, &QQmlSortFilterProxyModel::invalidateFilter);
    filter->m_proxyModel = that;
    that->invalidateFilter();
}

int QQmlSortFilterProxyModel::count_filter(QQmlListProperty<Filter>* list)
{
    QList<Filter*>* filters = static_cast<QList<Filter*>*>(list->data);
    return filters->count();
}

Filter* QQmlSortFilterProxyModel::at_filter(QQmlListProperty<Filter>* list, int index)
{
    QList<Filter*>* filters = static_cast<QList<Filter*>*>(list->data);
    return filters->at(index);
}

void QQmlSortFilterProxyModel::clear_filters(QQmlListProperty<Filter> *list)
{
    QQmlSortFilterProxyModel* that = static_cast<QQmlSortFilterProxyModel*>(list->object);
    that->m_filters.clear();
    that->invalidateFilter();
}

void QQmlSortFilterProxyModel::append_sorter(QQmlListProperty<Sorter>* list, Sorter* sorter)
{
    if (!sorter)
        return;

    auto that = static_cast<QQmlSortFilterProxyModel*>(list->object);
    that->m_sorters.append(sorter);
    connect(sorter, &Sorter::invalidate, that, &QQmlSortFilterProxyModel::invalidate);
    sorter->m_proxyModel = that;
    that->invalidate();
}

int QQmlSortFilterProxyModel::count_sorter(QQmlListProperty<Sorter>* list)
{
    auto sorters = static_cast<QList<Sorter*>*>(list->data);
    return sorters->count();
}

Sorter* QQmlSortFilterProxyModel::at_sorter(QQmlListProperty<Sorter>* list, int index)
{
    auto sorters = static_cast<QList<Sorter*>*>(list->data);
    return sorters->at(index);
}

void QQmlSortFilterProxyModel::clear_sorters(QQmlListProperty<Sorter>* list)
{
    auto that = static_cast<QQmlSortFilterProxyModel*>(list->object);
    that->m_sorters.clear();
    that->invalidate();
}

void registerQQmlSortFilterProxyModelTypes() {
    qmlRegisterType<QQmlSortFilterProxyModel>("SortFilterProxyModel", 0, 2, "SortFilterProxyModel");
}

Q_COREAPP_STARTUP_FUNCTION(registerQQmlSortFilterProxyModelTypes)

}
