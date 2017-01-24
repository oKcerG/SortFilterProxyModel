#include "sorter.h"
#include <QtQml>

namespace qqsfpm {

Sorter::Sorter(QObject *parent) : QObject(parent)
{
    connect(this, &Sorter::sorterChanged, this, &Sorter::onSorterChanged);
}

Sorter::~Sorter() = default;

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
    return m_ascendingOrder;
}

void Sorter::setAscendingOrder(bool ascendingOrder)
{
    if (m_ascendingOrder == ascendingOrder)
        return;

    m_ascendingOrder = ascendingOrder;
    Q_EMIT ascendingOrderChanged();
    Q_EMIT sorterChanged();
}

int Sorter::compareRows(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    int comparison = compare(source_left, source_right);
    return m_ascendingOrder ? comparison : -comparison;
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

void Sorter::onSorterChanged()
{
    if (m_enabled)
        Q_EMIT invalidate();
}

const QString& RoleSorter::roleName() const
{
    return m_roleName;
}

void RoleSorter::setRoleName(const QString& roleName)
{
    if (m_roleName == roleName)
        return;

    m_roleName = roleName;
    Q_EMIT roleNameChanged();
    Q_EMIT sorterChanged();
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
    Q_EMIT sorterChanged();
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
    connect(m_expression, &QQmlExpression::valueChanged, this, &Sorter::sorterChanged);
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
