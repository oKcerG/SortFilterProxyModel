#include "proxyrole.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlExpression>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlInfo>
#include "filter.h"

namespace qqsfpm {

/*!
    \qmltype ProxyRole
    \inqmlmodule SortFilterProxyModel
    \brief Base type for the \l SortFilterProxyModel proxy roles

    The ProxyRole type cannot be used directly in a QML file.
    It exists to provide a set of common properties and methods,
    available across all the other proxy role types that inherit from it.
    Attempting to use the ProxyRole type directly will result in an error.
*/
ProxyRole::ProxyRole(QObject *parent) : QObject(parent)
{
}

/*!
    \qmlproperty string ProxyRole::name

    This property holds the role name of the proxy role.
*/
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

/*!
    \qmltype JoinRole
    \inherits ProxyRole
    \inqmlmodule SortFilterProxyModel
    \brief a role made from concatenating other roles

    A JoinRole is a simple \l ProxyRole that concatenates other roles.

    In the following example, the \c fullName role is computed by the concatenation of the \c firstName role and the \c lastName role separated by a space :
    \code
    SortFilterProxyModel {
       sourceModel: contactModel
       proxyRoles: JoinRole {
           name: "fullName"
           roleNames: ["firstName", "lastName"]
      }
    }
    \endcode

*/

/*!
    \qmlproperty list<string> JoinRole::roleNames

    This property holds the role names that are joined by this role.
*/
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

/*!
    \qmlproperty string JoinRole::separator

    This property holds the separator that is used to join the roles specified in \l roleNames.

    By default, it's a space.
*/
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

/*!
    \qmltype SwitchRole
    \inherits ProxyRole
    \inqmlmodule SortFilterProxyModel
    \brief a role using \l Filter to conditionnaly compute its data

    A SwitchRole is a \l ProxyRole that computes its data with the help of \l Filter.
    Each top level filters specified in the \l SwitchRole is evaluated on the rows of the model, if a \l Filter evaluates to true, the data of the \l SwitchRole for this row will be the one of the attached \l {value} {SwitchRole.value} property.
    If no top level filters evaluate to true, the data will default to the one of the \l defaultRoleName (or the \l defaultValue if no \l defaultRoleName is specified).

    In the following example, the \c favoriteOrFirstNameSection role is equal to \c * if the \c favorite role of a row is true, otherwise it's the same as the \c firstName role :
    \code
    SortFilterProxyModel {
       sourceModel: contactModel
       proxyRoles: SwitchRole {
           name: "favoriteOrFirstNameSection"
           filters: ValueFilter {
               roleName: "favorite"
               value: true
               SwitchRole.value: "*"
           }
           defaultRoleName: "firstName"
        }
    }
    \endcode

*/
SwitchRoleAttached::SwitchRoleAttached(QObject* parent) : QObject (parent)
{
    if (!qobject_cast<Filter*>(parent))
        qmlInfo(parent) << "SwitchRole must be attached to a Filter";
}

/*!
    \qmlattachedproperty var SwitchRole::value

    This property attaches a value to a \l Filter.
*/
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

/*!
    \qmlproperty string SwitchRole::defaultRoleName

    This property holds the default role name of the role.
    If no filter match a row, the data of this role will be the data of the role whose name is \c defaultRoleName.
*/
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

/*!
    \qmlproperty var SwitchRole::defaultValue

    This property holds the default value of the role.
    If no filter match a row, and no \l defaultRoleName is set, the data of this role will be \c defaultValue.
*/
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

/*!
    \qmlproperty list<Filter> SwitchRole::filters

    This property holds the list of filters for this proxy role.
    The data of this role will be equal to the attached \l {value} {SwitchRole.value} property of the first filter that matches the model row.

    \sa Filter
*/
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

/*!
    \qmltype ExpressionRole
    \inherits ProxyRole
    \inqmlmodule SortFilterProxyModel
    \brief A custom role computed from a javascrip expression

    An ExpressionRole is a \l ProxyRole allowing to implement a custom role based on a javascript expression.

    In the following example, the \c c role is computed by adding the \c a role and \c b role of the model :
    \code
    SortFilterProxyModel {
       sourceModel: numberModel
       proxyRoles: ExpressionRole {
           name: "c"
           expression: model.a + model.b
      }
    }
    \endcode
*/

/*!
    \qmlproperty expression ExpressionRole::expression

    An expression to implement a custom role.
    It has the same syntax has a \l {http://doc.qt.io/qt-5/qtqml-syntax-propertybinding.html} {Property Binding} except it will be evaluated for each of the source model's rows.
    The data for this role will be the retuned valued of the expression.
    Data for each row is exposed like for a delegate of a QML View.

    This expression is reevaluated for a row every time its model data changes.
    When an external property (not \c index or in \c model) the expression depends on changes, the expression is reevaluated for every row of the source model.
    To capture the properties the expression depends on, the expression is first executed with invalid data and each property access is detected by the QML engine.
    This means that if a property is not accessed because of a conditional, it won't be captured and the expression won't be reevaluted when this property changes.

    A workaround to this problem is to access all the properties the expressions depends unconditionally at the beggining of the expression.
*/
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
