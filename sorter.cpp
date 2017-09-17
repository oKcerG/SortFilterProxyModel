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
    Q_EMIT invalidated();
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
    invalidate();
}

int Sorter::compareRows(const QModelIndex &source_left, const QModelIndex &source_right, const QQmlSortFilterProxyModel& proxyModel) const
{
    int comparison = compare(source_left, source_right, proxyModel);
    return (m_sortOrder == Qt::AscendingOrder) ? comparison : -comparison;
}

int Sorter::compare(const QModelIndex &sourceLeft, const QModelIndex &sourceRight, const QQmlSortFilterProxyModel& proxyModel) const
{
    if (lessThan(sourceLeft, sourceRight, proxyModel))
        return -1;
    if (lessThan(sourceRight, sourceLeft, proxyModel))
        return 1;
    return 0;
}

void Sorter::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    Q_UNUSED(proxyModel)
}

bool Sorter::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight, const QQmlSortFilterProxyModel& proxyModel) const
{
    Q_UNUSED(sourceLeft)
    Q_UNUSED(sourceRight)
    Q_UNUSED(proxyModel)
    return false;
}

void Sorter::invalidate()
{
    if (m_enabled)
        Q_EMIT invalidated();
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
    invalidate();
}

QPair<QVariant, QVariant> RoleSorter::sourceData(const QModelIndex &sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const
{
    QPair<QVariant, QVariant> pair;
    int role = proxyModel.roleForName(m_roleName);
    pair.first = proxyModel.sourceData(sourceLeft, role);
    pair.second = proxyModel.sourceData(sourceRight, role);
    return pair;
}

int RoleSorter::compare(const QModelIndex &sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const
{
    QPair<QVariant, QVariant> pair = sourceData(sourceLeft, sourceRight, proxyModel);
    QVariant leftValue = pair.first;
    QVariant rightValue = pair.second;
    if (leftValue < rightValue)
        return -1;
    if (leftValue > rightValue)
        return 1;
    return 0;
}

/*!
    \qmltype StringSorter
    \inherits RoleSorter
    \inqmlmodule SortFilterProxyModel
    \brief Sorts rows based on a source model string role

    \l StringSorter is a specialized \l RoleSorter that sorts rows based on a source model string role.
    \l StringSorter compares strings according to a localized collation algorithm.

    In the following example, rows with be sorted by their \c lastName role :
    \code
    SortFilterProxyModel {
       sourceModel: contactModel
       sorters: StringSorter { roleName: "lastName" }
    }
    \endcode
*/

/*!
    \qmlproperty Qt.CaseSensitivity StringSorter::caseSensitivity

    This property holds the case sensitivity of the sorter.
*/
Qt::CaseSensitivity StringSorter::caseSensitivity() const
{
    return m_collator.caseSensitivity();
}

void StringSorter::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
    if (m_collator.caseSensitivity() == caseSensitivity)
        return;

    m_collator.setCaseSensitivity(caseSensitivity);
    Q_EMIT caseSensitivityChanged();
    invalidate();
}

/*!
    \qmlproperty bool StringSorter::ignorePunctation

    This property holds whether the sorter ignores punctation.
    if \c ignorePunctuation is \c true, punctuation characters and symbols are ignored when determining sort order.

    \note This property is not currently supported on Apple platforms or if Qt is configured to not use ICU on Linux.
*/
bool StringSorter::ignorePunctation() const
{
    return m_collator.ignorePunctuation();
}

void StringSorter::setIgnorePunctation(bool ignorePunctation)
{
    if (m_collator.ignorePunctuation() == ignorePunctation)
        return;

    m_collator.setIgnorePunctuation(ignorePunctation);
    Q_EMIT ignorePunctationChanged();
    invalidate();
}

/*!
    \qmlproperty Locale StringSorter::locale

    This property holds the locale of the sorter.
*/
QLocale StringSorter::locale() const
{
    return m_collator.locale();
}

void StringSorter::setLocale(const QLocale &locale)
{
    if (m_collator.locale() == locale)
        return;

    m_collator.setLocale(locale);
    Q_EMIT localeChanged();
    invalidate();
}

/*!
    \qmlproperty bool StringSorter::numericMode

    This property holds whether the numeric mode of the sorter is enabled.
    This will enable proper sorting of numeric digits, so that e.g. 100 sorts after 99.
    By default this mode is off.
*/
bool StringSorter::numericMode() const
{
    return m_collator.numericMode();
}

void StringSorter::setNumericMode(bool numericMode)
{
    if (m_collator.numericMode() == numericMode)
        return;

    m_collator.setNumericMode(numericMode);
    Q_EMIT numericModeChanged();
    invalidate();
}

int StringSorter::compare(const QModelIndex &sourceLeft, const QModelIndex &sourceRight, const QQmlSortFilterProxyModel& proxyModel) const
{
    QPair<QVariant, QVariant> pair = sourceData(sourceLeft, sourceRight, proxyModel);
    QString leftValue = pair.first.toString();
    QString rightValue = pair.second.toString();
    return m_collator.compare(leftValue, rightValue);
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
    invalidate();
}

void ExpressionSorter::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    updateContext(proxyModel);
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

int ExpressionSorter::compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const
{
    if (!m_scriptString.isEmpty()) {
        QVariantMap modelLeftMap, modelRightMap;
        QHash<int, QByteArray> roles = proxyModel.roleNames();

        QQmlContext context(qmlContext(this));

        for (auto it = roles.cbegin(); it != roles.cend(); ++it) {
            modelLeftMap.insert(it.value(), proxyModel.sourceData(sourceLeft, it.key()));
            modelRightMap.insert(it.value(), proxyModel.sourceData(sourceRight, it.key()));
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

void ExpressionSorter::updateContext(const QQmlSortFilterProxyModel& proxyModel)
{
    delete m_context;
    m_context = new QQmlContext(qmlContext(this), this);

    QVariantMap modelLeftMap, modelRightMap;
    // what about roles changes ?

    for (const QByteArray& roleName : proxyModel.roleNames().values()) {
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
    connect(m_expression, &QQmlExpression::valueChanged, this, &ExpressionSorter::invalidate);
    m_expression->setNotifyOnValueChanged(true);
    m_expression->evaluate();
}

void registerSorterTypes() {
    qmlRegisterUncreatableType<Sorter>("SortFilterProxyModel", 0, 2, "Sorter", "Sorter is an abstract class");
    qmlRegisterType<RoleSorter>("SortFilterProxyModel", 0, 2, "RoleSorter");
    qmlRegisterType<StringSorter>("SortFilterProxyModel", 0, 2, "StringSorter");
    qmlRegisterType<ExpressionSorter>("SortFilterProxyModel", 0, 2, "ExpressionSorter");
}

Q_COREAPP_STARTUP_FUNCTION(registerSorterTypes)

}
