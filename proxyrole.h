#ifndef PROXYROLE_H
#define PROXYROLE_H

#include <QObject>
#include <QMutex>
#include <QQmlScriptString>
#include <QQmlExpression>
#include <qqml.h>
#include "filter.h"

namespace qqsfpm {

class ProxyRole;
class QQmlSortFilterProxyModel;

class ProxyRoleContainer {
public:
    virtual ~ProxyRoleContainer();

    QList<ProxyRole*> proxyRoles() const;
    void appendProxyRole(ProxyRole* proxyRole);
    void removeProxyRole(ProxyRole* proxyRole);
    void clearProxyRoles();

    QQmlListProperty<ProxyRole> proxyRolesListProperty();

protected:
    QList<ProxyRole*> m_proxyRoles;

private:
    virtual void onProxyRoleAppended(ProxyRole* proxyRole) = 0;
    virtual void onProxyRoleRemoved(ProxyRole* proxyRole) = 0;
    virtual void onProxyRolesCleared() = 0;

    static void append_proxyRole(QQmlListProperty<ProxyRole>* list, ProxyRole* proxyRole);
    static int count_proxyRole(QQmlListProperty<ProxyRole>* list);
    static ProxyRole* at_proxyRole(QQmlListProperty<ProxyRole>* list, int index);
    static void clear_proxyRoles(QQmlListProperty<ProxyRole>* list);
};
}
#define ProxyRoleContainer_iid "fr.grecko.SortFilterProxyModel.ProxyRoleContainer"
Q_DECLARE_INTERFACE(qqsfpm::ProxyRoleContainer, ProxyRoleContainer_iid)

namespace qqsfpm {

class ProxyRole : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit ProxyRole(QObject *parent = nullptr);

    const QString& name() const;
    void setName(const QString& name);

    QVariant roleData(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel);
    virtual void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel);

protected:
    void invalidate();

Q_SIGNALS:
    void nameAboutToBeChanged();
    void nameChanged();
    void invalidated();

private:
    virtual QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) = 0;

    QString m_name;
    QMutex m_mutex;
};

class JoinRole : public ProxyRole
{
    Q_OBJECT
    Q_PROPERTY(QStringList roleNames READ roleNames WRITE setRoleNames NOTIFY roleNamesChanged)
    Q_PROPERTY(QString separator READ separator WRITE setSeparator NOTIFY separatorChanged)

public:
    using ProxyRole::ProxyRole;

    QStringList roleNames() const;
    void setRoleNames(const QStringList& roleNames);

    QString separator() const;
    void setSeparator(const QString& separator);

Q_SIGNALS:
    void roleNamesChanged();

    void separatorChanged();

private:
    QStringList m_roleNames;
    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;
    QString m_separator = " ";
};

class SwitchRoleAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
public:
    SwitchRoleAttached(QObject* parent);

    QVariant value() const;
    void setValue(QVariant value);

Q_SIGNALS:
    void valueChanged();

private:
    QVariant m_value;
};

class SwitchRole : public ProxyRole, public FilterContainer
{
    Q_OBJECT
    Q_INTERFACES(qqsfpm::FilterContainer)
    Q_PROPERTY(QString defaultRoleName READ defaultRoleName WRITE setDefaultRoleName NOTIFY defaultRoleNameChanged)
    Q_PROPERTY(QVariant defaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged)
    Q_PROPERTY(QQmlListProperty<qqsfpm::Filter> filters READ filtersListProperty)

public:
    using ProxyRole::ProxyRole;

    QString defaultRoleName() const;
    void setDefaultRoleName(const QString& defaultRoleName);

    QVariant defaultValue() const;
    void setDefaultValue(const QVariant& defaultValue);

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;

    static SwitchRoleAttached* qmlAttachedProperties(QObject* object);

Q_SIGNALS:
    void defaultRoleNameChanged();
    void defaultValueChanged();

private:
    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;

    void onFilterAppended(Filter *filter) override;
    void onFilterRemoved(Filter *filter) override;
    void onFiltersCleared() override;

    QString m_defaultRoleName;
    QVariant m_defaultValue;
};

class ExpressionRole : public ProxyRole
{
    Q_OBJECT
    Q_PROPERTY(QQmlScriptString expression READ expression WRITE setExpression NOTIFY expressionChanged)

public:
    using ProxyRole::ProxyRole;

    const QQmlScriptString& expression() const;
    void setExpression(const QQmlScriptString& scriptString);

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;

Q_SIGNALS:
    void expressionChanged();

private:
    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;
    void updateContext(const QQmlSortFilterProxyModel& proxyModel);
    void updateExpression();

    QQmlScriptString m_scriptString;
    QQmlExpression* m_expression = nullptr;
    QQmlContext* m_context = nullptr;
};

}

QML_DECLARE_TYPEINFO(qqsfpm::SwitchRole, QML_HAS_ATTACHED_PROPERTIES)

#endif // PROXYROLE_H
