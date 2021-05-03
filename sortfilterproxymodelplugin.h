#ifndef QMLSORTFILTERPFOXYMODELPLUGIN_H
#define QMLSORTFILTERPFOXYMODELPLUGIN_H

#include <QQmlExtensionPlugin>

class QmlSortFilterProxyModelPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override;
};

#endif // QMLSORTFILTERPFOXYMODELPLUGIN_H

