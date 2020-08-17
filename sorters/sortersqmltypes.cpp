#include "sorter.h"
#include "rolesorter.h"
#include "stringsorter.h"
#include "filtersorter.h"
#include "expressionsorter.h"
#include "sortercontainer.h"
#include <QQmlEngine>
#include <QCoreApplication>

static bool wasRegistered = false;

namespace qqsfpm {

void registerSorterTypes() {
    if(!wasRegistered) {
        qmlRegisterUncreatableType<Sorter>("SortFilterProxyModel", 0, 2, "Sorter", "Sorter is an abstract class");
        qmlRegisterType<RoleSorter>("SortFilterProxyModel", 0, 2, "RoleSorter");
        qmlRegisterType<StringSorter>("SortFilterProxyModel", 0, 2, "StringSorter");
        qmlRegisterType<FilterSorter>("SortFilterProxyModel", 0, 2, "FilterSorter");
        qmlRegisterType<ExpressionSorter>("SortFilterProxyModel", 0, 2, "ExpressionSorter");
        qmlRegisterUncreatableType<SorterContainerAttached>("SortFilterProxyModel", 0, 2, "SorterContainer", "SorterContainer can only be used as an attaching type");
        wasRegistered = true;
    }
}

Q_COREAPP_STARTUP_FUNCTION(registerSorterTypes)

}
