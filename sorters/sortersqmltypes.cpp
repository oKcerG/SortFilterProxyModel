#include "sorter.h"
#include "rolesorter.h"
#include "stringsorter.h"
#include "filtersorter.h"
#include "expressionsorter.h"
#include <QQmlEngine>
#include <QCoreApplication>

namespace qqsfpm {

void registerSorterTypes() {
    qmlRegisterUncreatableType<Sorter>("SortFilterProxyModel", 0, 2, "Sorter", "Sorter is an abstract class");
    qmlRegisterType<RoleSorter>("SortFilterProxyModel", 0, 2, "RoleSorter");
    qmlRegisterType<StringSorter>("SortFilterProxyModel", 0, 2, "StringSorter");
    qmlRegisterType<FilterSorter>("SortFilterProxyModel", 0, 2, "FilterSorter");
    qmlRegisterType<ExpressionSorter>("SortFilterProxyModel", 0, 2, "ExpressionSorter");
}

Q_COREAPP_STARTUP_FUNCTION(registerSorterTypes)

}
