#include "expressionrole.h"
#include "qqmlsortfilterproxymodel.h"
#include <QtQml>

namespace qqsfpm {

/*!
    \qmltype ExpressionRole
    \inherits SingleRole
    \inqmlmodule SortFilterProxyModel
    \ingroup ProxyRoles
    \brief A custom role computed from a javascript expression.

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

bool ExpressionRole::isCached() const
{
    return m_cached;
}

void ExpressionRole::setCached(bool cached)
{
    if (m_cached == cached) {
        return;
    }

    m_cached = cached;
    emit cachedChanged(m_cached);
}

void ExpressionRole::invalidateCache()
{
    m_cache.clear();
}

QVariant ExpressionRole::data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel)
{
    if (isCached()) {
        const auto it = m_cache.constFind(sourceIndex);
        if (it != m_cache.constEnd()) {
            return *it;
        }
    }

    if (!m_scriptString.isEmpty()) {
        const QVariant modelMap = proxyModel.sourceData(sourceIndex);
        QQmlContext context(qmlContext(this));
        context.setContextProperty("index", sourceIndex.row());
        context.setContextProperty("modelIndex", sourceIndex);
        context.setContextProperty("model", modelMap);

        QQmlExpression expression(m_scriptString, &context);
        QVariant result = expression.evaluate();

        if (expression.hasError()) {
            qWarning() << expression.error();
            return true;
        }

        if (isCached()) {
            m_cache[sourceIndex] = result;
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

    const auto roleNames = proxyModel.roleNames().values();
    for (const QByteArray& roleName : roleNames) {
        modelMap[roleName] = QVariant();
    }

    m_context->setContextProperty("index", -1);
    m_context->setContextProperty("modelIndex", QModelIndex());
    m_context->setContextProperty("model", modelMap);
    updateExpression();
}

void ExpressionRole::updateExpression()
{
    if (!m_context)
        return;

    delete m_expression;
    m_expression = new QQmlExpression(m_scriptString, m_context, 0, this);
    connect(m_expression, &QQmlExpression::valueChanged, this, [=] {
        invalidateCache();
        invalidate();
    });
    m_expression->setNotifyOnValueChanged(true);
    m_expression->evaluate();
}

}
