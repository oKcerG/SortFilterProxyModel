#include "qqmlsortfilterproxymodel_plugin.h"

#include <qqml.h>

#include "qqmlsortfilterproxymodel.h"
#include "filter.h"
#include "sorter.h"

using namespace qqsfpm;

void QQmlSortFilterProxyModelPlugin::registerTypes(const char *uri)
{
    Q_UNUSED(uri)

    //Filter
    qmlRegisterUncreatableType<Filter>("SortFilterProxyModel", 0, 2, "Filter", "Filter is an abstract class");
    qmlRegisterType<ValueFilter>("SortFilterProxyModel", 0, 2, "ValueFilter");
    qmlRegisterType<IndexFilter>("SortFilterProxyModel", 0, 2, "IndexFilter");
    qmlRegisterType<RegExpFilter>("SortFilterProxyModel", 0, 2, "RegExpFilter");
    qmlRegisterType<RangeFilter>("SortFilterProxyModel", 0, 2, "RangeFilter");
    qmlRegisterType<ExpressionFilter>("SortFilterProxyModel", 0, 2, "ExpressionFilter");
    qmlRegisterType<AnyOfFilter>("SortFilterProxyModel", 0, 2, "AnyOf");
    qmlRegisterType<AllOfFilter>("SortFilterProxyModel", 0, 2, "AllOf");

    //Actual Model
    qmlRegisterType<QQmlSortFilterProxyModel>("SortFilterProxyModel", 0, 2, "SortFilterProxyModel");

    //Sorter
    qmlRegisterUncreatableType<Sorter>("SortFilterProxyModel", 0, 2, "Sorter", "Sorter is an abstract class");
    qmlRegisterType<RoleSorter>("SortFilterProxyModel", 0, 2, "RoleSorter");
    qmlRegisterType<NaturalSorter>("SortFilterProxyModel", 0, 2, "NaturalSorter");
    qmlRegisterType<ExpressionSorter>("SortFilterProxyModel", 0, 2, "ExpressionSorter");
}

