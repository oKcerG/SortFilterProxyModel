#include "sorter.h"
#include <QtQml>

namespace qqsfpm {

/*!
    \qmltype Sorter
    \inqmlmodule SortFilterProxyModel
    \brief Base type for the \l SortFilterProxyModel sorters

    The Sorter type cannot be used directly in a QML file.
    It exists to provide a set of common properties and methods,
    available across all the other sorters types that inherit from it.
    Attempting to use the Sorter type directly will result in an error.
*/

Sorter::Sorter(QObject *parent) : QObject(parent)
{
}

Sorter::~Sorter() = default;

/*!
    \qmlproperty bool Sorter::enabled

    This property holds whether the sorter is enabled.
    A disabled sorter will not change the order of the rows.

    By default, sorters are enabled.
*/
bool Sorter::enabled() const
{
    return m_enabled;
}

void Sorter::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    Q_EMIT enabledChanged();
    Q_EMIT invalidate();
}

bool Sorter::ascendingOrder() const
{
    return sortOrder() == Qt::AscendingOrder;
}

void Sorter::setAscendingOrder(bool ascendingOrder)
{
    setSortOrder(ascendingOrder ? Qt::AscendingOrder : Qt::DescendingOrder);
}


/*!
    \qmlproperty Qt::SortOrder Sorter::sortOrder

    This property holds the sort order of this sorter.

    \value Qt.AscendingOrder The items are sorted ascending e.g. starts with 'AAA' ends with 'ZZZ' in Latin-1 locales
    \value Qt.DescendingOrder The items are sorted descending e.g. starts with 'ZZZ' ends with 'AAA' in Latin-1 locales

    By default, sorting is in ascending order.
*/
Qt::SortOrder Sorter::sortOrder() const
{
    return m_sortOrder;
}

void Sorter::setSortOrder(Qt::SortOrder sortOrder)
{
    if (m_sortOrder == sortOrder)
        return;

    m_sortOrder = sortOrder;
    Q_EMIT sortOrderChanged();
    sorterChanged();
}

int Sorter::compareRows(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    int comparison = compare(source_left, source_right);
    return (m_sortOrder == Qt::AscendingOrder) ? comparison : -comparison;
}

QQmlSortFilterProxyModel* Sorter::proxyModel() const
{
    return m_proxyModel;
}

int Sorter::compare(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    if (lessThan(sourceLeft, sourceRight))
        return -1;
    if (lessThan(sourceRight, sourceLeft))
        return 1;
    return 0;
}

bool Sorter::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    Q_UNUSED(sourceLeft)
    Q_UNUSED(sourceRight)
    return false;
}

void Sorter::proxyModelCompleted()
{

}

void Sorter::sorterChanged()
{
    if (m_enabled)
        Q_EMIT invalidate();
}

const QString& RoleSorter::roleName() const
{
    return m_roleName;
}

/*!
    \qmltype RoleSorter
    \inherits Sorter
    \inqmlmodule SortFilterProxyModel
    \brief Sorts rows based on a source model role

    A RoleSorter is a simple \l Sorter that sorts rows based on a source model role.

    In the following example, rows with be sorted by their \c lastName role :
    \code
    SortFilterProxyModel {
       sourceModel: contactModel
       sorters: RoleSorter { roleName: "lastName" }
    }
    \endcode
*/

/*!
    \qmlproperty string RoleSorter::roleName

    This property holds the role name that the sorter is using to query the source model's data when sorting items.
*/
void RoleSorter::setRoleName(const QString& roleName)
{
    if (m_roleName == roleName)
        return;

    m_roleName = roleName;
    Q_EMIT roleNameChanged();
    sorterChanged();
}

int RoleSorter::compare(const QModelIndex &sourceLeft, const QModelIndex& sourceRight) const
{
    QVariant leftValue = proxyModel()->sourceData(sourceLeft, m_roleName);
    QVariant rightValue = proxyModel()->sourceData(sourceRight, m_roleName);
    if (leftValue < rightValue)
        return -1;
    if (leftValue > rightValue)
        return 1;
    return 0;
}

/*!
    \qmltype ExpressionSorter
    \inherits Sorter
    \inqmlmodule SortFilterProxyModel
    \brief Sorts row with a custom sorting

    An ExpressionSorter is a \l Sorter allowing to implement custom sorting based on a javascript expression.
*/

/*!
    \qmlproperty expression ExpressionSorter::expression

    An expression to implement custom sorting, it must evaluate to a bool.
    It has the same syntax has a \l {http://doc.qt.io/qt-5/qtqml-syntax-propertybinding.html} {Property Binding} except it will be evaluated for each of the source model's rows.
    Model data is accessible for both row with the \c indexLeft, \c modelLeft, \c indexRight and \c modelRight properties.
    The expression should return \c true if the value of the left item is less than the value of the right item, otherwise returns false.

    This expression is reevaluated for a row every time its model data changes.
    When an external property (not \c index* or in \c model*) the expression depends on changes, the expression is reevaluated for every row of the source model.
    To capture the properties the expression depends on, the expression is first executed with invalid data and each property access is detected by the QML engine.
    This means that if a property is not accessed because of a conditional, it won't be captured and the expression won't be reevaluted when this property changes.

    A workaround to this problem is to access all the properties the expressions depends unconditionally at the beggining of the expression.
*/
const QQmlScriptString& ExpressionSorter::expression() const
{
    return m_scriptString;
}

void ExpressionSorter::setExpression(const QQmlScriptString& scriptString)
{
    if (m_scriptString == scriptString)
        return;

    m_scriptString = scriptString;
    updateExpression();

    Q_EMIT expressionChanged();
    sorterChanged();
}

bool evaluateBoolExpression(QQmlExpression& expression)
{
    QVariant variantResult = expression.evaluate();
    if (expression.hasError()) {
        qWarning() << expression.error();
        return false;
    }
    if (variantResult.canConvert<bool>()) {
        return variantResult.toBool();
    } else {
        qWarning("%s:%i:%i : Can't convert result to bool",
                 expression.sourceFile().toUtf8().data(),
                 expression.lineNumber(),
                 expression.columnNumber());
        return false;
    }
}

int ExpressionSorter::compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const
{
    if (!m_scriptString.isEmpty()) {
        QVariantMap modelLeftMap, modelRightMap;
        QHash<int, QByteArray> roles = proxyModel()->roleNames();

        QQmlContext context(qmlContext(this));

        for (auto it = roles.cbegin(); it != roles.cend(); ++it) {
            modelLeftMap.insert(it.value(), proxyModel()->sourceData(sourceLeft, it.key()));
            modelRightMap.insert(it.value(), proxyModel()->sourceData(sourceRight, it.key()));
        }
        modelLeftMap.insert("index", sourceLeft.row());
        modelRightMap.insert("index", sourceRight.row());

        QQmlExpression expression(m_scriptString, &context);

        context.setContextProperty("modelLeft", modelLeftMap);
        context.setContextProperty("modelRight", modelRightMap);
        if (evaluateBoolExpression(expression))
                return -1;

        context.setContextProperty("modelLeft", modelRightMap);
        context.setContextProperty("modelRight", modelLeftMap);
        if (evaluateBoolExpression(expression))
                return 1;
    }
    return 0;
}

void ExpressionSorter::proxyModelCompleted()
{
    updateContext();
}

void ExpressionSorter::updateContext()
{
    if (!proxyModel())
        return;

    delete m_context;
    m_context = new QQmlContext(qmlContext(this), this);

    QVariantMap modelLeftMap, modelRightMap;
    // what about roles changes ?

    for (const QByteArray& roleName : proxyModel()->roleNames().values()) {
        modelLeftMap.insert(roleName, QVariant());
        modelRightMap.insert(roleName, QVariant());
    }
    modelLeftMap.insert("index", -1);
    modelRightMap.insert("index", -1);

    m_context->setContextProperty("modelLeft", modelLeftMap);
    m_context->setContextProperty("modelRight", modelRightMap);

    updateExpression();
}

void ExpressionSorter::updateExpression()
{
    if (!m_context)
        return;

    delete m_expression;
    m_expression = new QQmlExpression(m_scriptString, m_context, 0, this);
    connect(m_expression, &QQmlExpression::valueChanged, this, &ExpressionSorter::sorterChanged);
    m_expression->setNotifyOnValueChanged(true);
    m_expression->evaluate();
}

void registerSorterTypes() {
    qmlRegisterUncreatableType<Sorter>("SortFilterProxyModel", 0, 2, "Sorter", "Sorter is an abstract class");
    qmlRegisterType<RoleSorter>("SortFilterProxyModel", 0, 2, "RoleSorter");
    qmlRegisterType<ExpressionSorter>("SortFilterProxyModel", 0, 2, "ExpressionSorter");
}

Q_COREAPP_STARTUP_FUNCTION(registerSorterTypes)

}
