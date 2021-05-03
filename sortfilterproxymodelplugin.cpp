#include "sortfilterproxymodelplugin.h"
#include "filters/filtersqmltypes.h"
#include "proxyroles/proxyrolesqmltypes.h"
#include "qqmlsortfilterproxymodel.h"
#include "sorters/sortersqmltypes.h"

#include <qqml.h>

using namespace qqsfpm;

void QmlSortFilterProxyModelPlugin::registerTypes(const char *uri)
{
    registerProxyRoleTypes(uri, QML_IMPORT_MAJOR_VERSION, QML_IMPORT_MINOR_VERSION);
    registerFiltersTypes(uri, QML_IMPORT_MAJOR_VERSION, QML_IMPORT_MINOR_VERSION);
    registerSorterTypes(uri, QML_IMPORT_MAJOR_VERSION, QML_IMPORT_MINOR_VERSION);

    qmlRegisterType<QQmlSortFilterProxyModel>(uri, QML_IMPORT_MAJOR_VERSION, QML_IMPORT_MINOR_VERSION, "SortFilterProxyModel");
}
