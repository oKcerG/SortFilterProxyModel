#include "proxyrolesqmltypes.h"
#include "proxyrole.h"
#include "joinrole.h"
#include "switchrole.h"
#include "expressionrole.h"
#include "regexprole.h"
#include "filterrole.h"
#include <QQmlEngine>
#include <QCoreApplication>

namespace qqsfpm {

void registerProxyRoleTypes(const char *uri, int major, int minor) {
    qmlRegisterUncreatableType<ProxyRole>(uri, major, minor, "ProxyRole", "ProxyRole is an abstract class");
    qmlRegisterType<JoinRole>(uri, major, minor, "JoinRole");
    qmlRegisterType<SwitchRole>(uri, major, minor, "SwitchRole");
    qmlRegisterType<ExpressionRole>(uri, major, minor, "ExpressionRole");
    qmlRegisterType<RegExpRole>(uri, major, minor, "RegExpRole");
    qmlRegisterType<FilterRole>(uri, major, minor, "FilterRole");
}

#ifndef QML_PLUGIN
void registerProxyRoleTypes() {
    registerProxyRoleTypes("SortFilterProxyModel", 0, 2);
}
Q_COREAPP_STARTUP_FUNCTION(registerProxyRoleTypes)
#endif

} // namespace qqsfpm
