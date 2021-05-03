#include "sortersqmltypes.h"
#include "sorter.h"
#include "rolesorter.h"
#include "stringsorter.h"
#include "filtersorter.h"
#include "expressionsorter.h"
#include "sortercontainer.h"
#include <QQmlEngine>
#include <QCoreApplication>

namespace qqsfpm {

void registerSorterTypes(const char *uri, int major, int minor) {
    qmlRegisterUncreatableType<Sorter>(uri, major, minor, "Sorter", "Sorter is an abstract class");
    qmlRegisterType<RoleSorter>(uri, major, minor, "RoleSorter");
    qmlRegisterType<StringSorter>(uri, major, minor, "StringSorter");
    qmlRegisterType<FilterSorter>(uri, major, minor, "FilterSorter");
    qmlRegisterType<ExpressionSorter>(uri, major, minor, "ExpressionSorter");
    qmlRegisterUncreatableType<SorterContainerAttached>(uri, major, minor, "SorterContainer", "SorterContainer can only be used as an attaching type");
}

void registerSorterTypes() {
    registerSorterTypes("SortFilterProxyModel", 0, 2);
}

#ifndef QML_PLUGIN
Q_COREAPP_STARTUP_FUNCTION(registerSorterTypes)
#endif

} // namespace qqsfpm
