#include "sortfilterproxymodel.h"

// forwards
namespace qqsfpm {

void registerFiltersTypes();
void registerProxyRoleTypes();
void registerQQmlSortFilterProxyModelTypes();
void registerSorterTypes();

}

void SortFilterProxyModel::registerQml()
{
    qqsfpm::registerFiltersTypes();
    qqsfpm::registerProxyRoleTypes();
    qqsfpm::registerQQmlSortFilterProxyModelTypes();
    qqsfpm::registerSorterTypes();
}
