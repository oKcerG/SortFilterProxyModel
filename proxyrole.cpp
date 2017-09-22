#include "proxyrole.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlExpression>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlInfo>
#include "filter.h"

namespace qqsfpm {

ProxyRole::ProxyRole(QObject *parent) : QObject(parent)
{
}

const QString& ProxyRole::name() const
{
    return m_name;
}

void ProxyRole::setName(const QString& name)
{
    if (m_name == name)
        return;

    Q_EMIT nameAboutToBeChanged();
    m_name = name;
    Q_EMIT nameChanged();
}

QVariant ProxyRole::roleData(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel)
{
    if (m_mutex.tryLock()) {
        QVariant result = data(sourceIndex, proxyModel);
        m_mutex.unlock();
        return result;
    } else {
        return QVariant{};
    }
}

void ProxyRole::proxyModelCompleted(const QQmlSortFilterProxyModel &proxyModel)
{
    Q_UNUSED(proxyModel)
}

void ProxyRole::invalidate()
{
    Q_EMIT invalidated();
}

QStringList JoinRole::roleNames() const
{
    return m_roleNames;
}

void JoinRole::setRoleNames(const QStringList& roleNames)
{
    if (m_roleNames == roleNames)
        return;

    m_roleNames = roleNames;
    Q_EMIT roleNamesChanged();
    invalidate();
}

QString JoinRole::separator() const
{
    return m_separator;
}

void JoinRole::setSeparator(const QString& separator)
{
    if (m_separator == separator)
        return;

    m_separator = separator;
    Q_EMIT separatorChanged();
    invalidate();
}

QVariant JoinRole::data(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel)
{
    QString result;

    for (const QString& roleName : m_roleNames)
        result += proxyModel.sourceData(sourceIndex, roleName).toString() + m_separator;

    if (!m_roleNames.isEmpty())
        result.chop(m_separator.length());

    return result;
}

SwitchRoleAttached::SwitchRoleAttached(QObject* parent) : QObject (parent)
{
    if (!qobject_cast<Filter*>(parent))
        qmlInfo(parent) << "SwitchRole must be attached to a Filter";
}

QVariant SwitchRoleAttached::value() const
{
    return m_value;
}

void SwitchRoleAttached::setValue(QVariant value)
{
    if (m_value == value)
        return;

    m_value = value;
    Q_EMIT valueChanged();
}

QString SwitchRole::defaultRoleName() const
{
    return m_defaultRoleName;
}

void SwitchRole::setDefaultRoleName(const QString& defaultRoleName)
{
    if (m_defaultRoleName == defaultRoleName)
        return;

    m_defaultRoleName = defaultRoleName;
    Q_EMIT defaultRoleNameChanged();
    invalidate();
}

QVariant SwitchRole::defaultValue() const
{
    return m_defaultValue;
}

void SwitchRole::setDefaultValue(const QVariant& defaultValue)
{
    if (m_defaultValue == defaultValue)
        return;

    m_defaultValue = defaultValue;
    Q_EMIT defaultValueChanged();
    invalidate();
}

QQmlListProperty<Filter> SwitchRole::filters()
{
    return QQmlListProperty<Filter>(this, &m_filters,
                                    &SwitchRole::append_filter,
                                    &SwitchRole::count_filter,
                                    &SwitchRole::at_filter,
                                    &SwitchRole::clear_filters);
}

void SwitchRole::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    for (Filter* filter : m_filters)
        filter->proxyModelCompleted(proxyModel);
}

SwitchRoleAttached* SwitchRole::qmlAttachedProperties(QObject* object)
{
    return new SwitchRoleAttached(object);
}

QVariant SwitchRole::data(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel &proxyModel)
{
    for (auto filter: m_filters) {
        if (!filter->enabled())
            continue;
        if (filter->filterAcceptsRow(sourceIndex, proxyModel)) {
            auto attached = static_cast<SwitchRoleAttached*>(qmlAttachedPropertiesObject<SwitchRole>(filter, false));
            if (!attached) {
                qWarning() << "No SwitchRole.value provided for this filter" << filter;
                continue;
            }
            QVariant value = attached->value();
            if (!value.isValid()) {
                qWarning() << "No SwitchRole.value provided for this filter" << filter;
                continue;
            }
            return value;
        }
    }
    if (!m_defaultRoleName.isEmpty())
        return proxyModel.sourceData(sourceIndex, m_defaultRoleName);
    return m_defaultValue;
}

void SwitchRole::append_filter(QQmlListProperty<Filter>* list, Filter* filter)
{
    if (!filter)
        return;

    SwitchRole* that = static_cast<SwitchRole*>(list->object);
    that->m_filters.append(filter);
    connect(filter, &Filter::invalidated, that, &SwitchRole::invalidate);
    auto attached = static_cast<SwitchRoleAttached*>(qmlAttachedPropertiesObject<SwitchRole>(filter, true));
    connect(attached, &SwitchRoleAttached::valueChanged, that, &SwitchRole::invalidate);
    that->invalidate();
}

int SwitchRole::count_filter(QQmlListProperty<Filter>* list)
{
    QList<Filter*>* filters = static_cast<QList<Filter*>*>(list->data);
    return filters->count();
}

Filter* SwitchRole::at_filter(QQmlListProperty<Filter>* list, int index)
{
    QList<Filter*>* filters = static_cast<QList<Filter*>*>(list->data);
    return filters->at(index);
}

void SwitchRole::clear_filters(QQmlListProperty<Filter> *list)
{
    SwitchRole* that = static_cast<SwitchRole*>(list->object);
    that->m_filters.clear();
    that->invalidate();
}

const QQmlScriptString& ExpressionRole::expression() const
{
    return m_scriptString;
}

void ExpressionRole::setExpression(const QQmlScriptString& scriptString)
{
    if (m_scriptString == scriptString)
        return;

    m_scriptString = scriptString;
    updateExpression();

    Q_EMIT expressionChanged();
    invalidate();
}

void ExpressionRole::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    updateContext(proxyModel);
}

QVariant ExpressionRole::data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel)
{
    if (!m_scriptString.isEmpty()) {
        QVariantMap modelMap;
        QHash<int, QByteArray> roles = proxyModel.roleNames();

        QQmlContext context(qmlContext(this));
        auto addToContext = [&] (const QString &name, const QVariant& value) {
            context.setContextProperty(name, value);
            modelMap.insert(name, value);
        };

        for (auto it = roles.cbegin(); it != roles.cend(); ++it)
            addToContext(it.value(), proxyModel.sourceData(sourceIndex, it.key()));
        addToContext("index", sourceIndex.row());

        context.setContextProperty("model", modelMap);

        QQmlExpression expression(m_scriptString, &context);
        QVariant result = expression.evaluate();

        if (expression.hasError()) {
            qWarning() << expression.error();
            return true;
        }
        return result;
    }
    return QVariant();
}

void ExpressionRole::updateContext(const QQmlSortFilterProxyModel& proxyModel)
{
    delete m_context;
    m_context = new QQmlContext(qmlContext(this), this);
    // what about roles changes ?
    QVariantMap modelMap;

    auto addToContext = [&] (const QString &name, const QVariant& value) {
        m_context->setContextProperty(name, value);
        modelMap.insert(name, value);
    };

    for (const QByteArray& roleName : proxyModel.roleNames().values())
        addToContext(roleName, QVariant());

    addToContext("index", -1);

    m_context->setContextProperty("model", modelMap);
    updateExpression();
}

void ExpressionRole::updateExpression()
{
    if (!m_context)
        return;

    delete m_expression;
    m_expression = new QQmlExpression(m_scriptString, m_context, 0, this);
    connect(m_expression, &QQmlExpression::valueChanged, this, &ExpressionRole::invalidate);
    m_expression->setNotifyOnValueChanged(true);
    m_expression->evaluate();
}

void registerProxyRoleTypes() {
    qmlRegisterUncreatableType<ProxyRole>("SortFilterProxyModel", 0, 2, "ProxyRole", "ProxyRole is an abstract class");
    qmlRegisterType<JoinRole>("SortFilterProxyModel", 0, 2, "JoinRole");
    qmlRegisterType<SwitchRole>("SortFilterProxyModel", 0, 2, "SwitchRole");
    qmlRegisterType<ExpressionRole>("SortFilterProxyModel", 0, 2, "ExpressionRole");
}

Q_COREAPP_STARTUP_FUNCTION(registerProxyRoleTypes)

}
