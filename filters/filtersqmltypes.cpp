#include "filtersqmltypes.h"
#include "filter.h"
#include "valuefilter.h"
#include "indexfilter.h"
#include "regexpfilter.h"
#include "rangefilter.h"
#include "expressionfilter.h"
#include "anyoffilter.h"
#include "alloffilter.h"
#include <QQmlEngine>
#include <QCoreApplication>

namespace qqsfpm {

void registerFiltersTypes(const char *uri, int major, int minor) {
    qmlRegisterUncreatableType<Filter>(uri, major, minor, "Filter", "Filter is an abstract class");
    qmlRegisterType<ValueFilter>(uri, major, minor, "ValueFilter");
    qmlRegisterType<IndexFilter>(uri, major, minor, "IndexFilter");
    qmlRegisterType<RegExpFilter>(uri, major, minor, "RegExpFilter");
    qmlRegisterType<RangeFilter>(uri, major, minor, "RangeFilter");
    qmlRegisterType<ExpressionFilter>(uri, major, minor, "ExpressionFilter");
    qmlRegisterType<AnyOfFilter>(uri, major, minor, "AnyOf");
    qmlRegisterType<AllOfFilter>(uri, major, minor, "AllOf");
    qmlRegisterUncreatableType<FilterContainerAttached>(uri, major, minor, "FilterContainer", "FilterContainer can only be used as an attaching type");
}

#ifndef QML_PLUGIN
void registerFiltersTypes() {
    registerFiltersTypes("SortFilterProxyModel", 0, 2);
}
Q_COREAPP_STARTUP_FUNCTION(registerFiltersTypes)
#endif

} // namespace qqsfpm
