#include "qqmlsortfilterproxymodeltypes.h"

namespace qqsfpm {
extern void registerQQmlSortFilterProxyModelTypes();
extern void registerFiltersTypes();
extern void registerSorterTypes();
extern void registerProxyRoleTypes();
} // namespace qqsfpm

void qqsfpm::registerTypes() {
  registerQQmlSortFilterProxyModelTypes();
  registerFiltersTypes();
  registerSorterTypes();
  registerProxyRoleTypes();
}
