#include "sorter.h"
#include "private/sorter_p.h"
#include <QtQml>

namespace qqsfpm
{

int SorterPrivate::compare(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    if (lessThan(sourceLeft, sourceRight))
        return -1;
    if (lessThan(sourceRight, sourceLeft))
        return 1;
    return 0;
}

bool SorterPrivate::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    Q_UNUSED(sourceLeft)
    Q_UNUSED(sourceRight)
    return false;
}

void SorterPrivate::proxyModelCompleted()
{

}

Sorter::Sorter(SorterPrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&dd)
{
    Q_D(Sorter);
    d->q_ptr = this;
    connect(this, &Sorter::sorterChanged, this, [&]
    {
        if (d->m_enabled)
            invalidate();
    });
}

bool Sorter::enabled() const
{
    Q_D(const Sorter);
    return d->m_enabled;
}

void Sorter::setEnabled(bool enabled)
{
    Q_D(Sorter);
    if (d->m_enabled == enabled)
        return;

    d->m_enabled = enabled;
    Q_EMIT enabledChanged();
    Q_EMIT invalidate();
}

bool Sorter::ascendingOrder() const
{
    Q_D(const Sorter);
    return d->m_ascendingOrder;
}

void Sorter::setAscendingOrder(bool ascendingOrder)
{
    Q_D(Sorter);
    if (d->m_ascendingOrder == ascendingOrder)
        return;

    d->m_ascendingOrder = ascendingOrder;
    Q_EMIT ascendingOrderChanged();
    Q_EMIT sorterChanged();
}

int Sorter::compareRows(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    Q_D(const Sorter);
    int comparison = d->compare(source_left, source_right);
    return d->m_ascendingOrder ? comparison : -comparison;
}

QQmlSortFilterProxyModel *Sorter::proxyModel() const
{
    Q_D(const Sorter);
    return d->m_proxyModel;
}

void Sorter::setProxyModel(QQmlSortFilterProxyModel *model, bool proxyModelCompleted)
{
    Q_D(Sorter);
    d->m_proxyModel = model;

    if (!proxyModelCompleted)
        return;

    d->proxyModelCompleted();
}

/*************************************************************/

int RoleSorterPrivate::compare(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    QVariant leftValue = m_proxyModel->sourceData(sourceLeft, m_roleName);
    QVariant rightValue = m_proxyModel->sourceData(sourceRight, m_roleName);
    if (leftValue < rightValue)
        return -1;
    if (leftValue > rightValue)
        return 1;
    return 0;
}

RoleSorter::RoleSorter(QObject *parent)
    : Sorter(*new RoleSorterPrivate(), parent)
{

}

const QString &RoleSorter::roleName() const
{
    Q_D(const RoleSorter);
    return d->m_roleName;
}

void RoleSorter::setRoleName(const QString &roleName)
{
    Q_D(RoleSorter);
    if (d->m_roleName == roleName)
        return;

    d->m_roleName = roleName;
    Q_EMIT roleNameChanged();
    Q_EMIT sorterChanged();
}

/*************************************************************/

int ExpressionSorterPrivate::compare(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    Q_Q(const ExpressionSorter);

    if (!m_scriptString.isEmpty()) {
        QVariantMap modelLeftMap, modelRightMap;
        QHash<int, QByteArray> roles = m_proxyModel->roleNames();

        QQmlContext context(qmlContext(q));

        for (auto it = roles.cbegin(); it != roles.cend(); ++it) {
            modelLeftMap.insert(it.value(), m_proxyModel->sourceData(sourceLeft, it.key()));
            modelRightMap.insert(it.value(), m_proxyModel->sourceData(sourceRight, it.key()));
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

void ExpressionSorterPrivate::proxyModelCompleted()
{
    updateContext();
}

bool ExpressionSorterPrivate::evaluateBoolExpression(QQmlExpression &expression) const
{
    QVariant variantResult = expression.evaluate();
    if (expression.hasError()) {
        qWarning() << expression.error();
        return false;
    }
    if (variantResult.canConvert<bool>()) {
        return variantResult.toBool();
    }
    else {
        qWarning("%s:%i:%i : Can't convert result to bool",
                 expression.sourceFile().toUtf8().data(),
                 expression.lineNumber(),
                 expression.columnNumber());
        return false;
    }
}

void ExpressionSorterPrivate::updateContext()
{
    Q_Q(ExpressionSorter);
    if (!m_proxyModel)
        return;

    delete m_context;
    m_context = new QQmlContext(qmlContext(q), q);

    QVariantMap modelLeftMap, modelRightMap;
    // what about roles changes ?

    for (const QByteArray &roleName : m_proxyModel->roleNames().values()) {
        modelLeftMap.insert(roleName, QVariant());
        modelRightMap.insert(roleName, QVariant());
    }
    modelLeftMap.insert("index", -1);
    modelRightMap.insert("index", -1);

    m_context->setContextProperty("modelLeft", modelLeftMap);
    m_context->setContextProperty("modelRight", modelRightMap);

    updateExpression();
}

void ExpressionSorterPrivate::updateExpression()
{
    Q_Q(ExpressionSorter);
    if (!m_context)
        return;

    delete m_expression;
    m_expression = new QQmlExpression(m_scriptString, m_context, 0, q);
    QObject::connect(m_expression, &QQmlExpression::valueChanged, q, &Sorter::sorterChanged);
    m_expression->setNotifyOnValueChanged(true);
    m_expression->evaluate();
}

ExpressionSorter::ExpressionSorter(QObject *parent)
    : Sorter(*new ExpressionSorterPrivate(), parent)
{

}

const QQmlScriptString &ExpressionSorter::expression() const
{
    Q_D(const ExpressionSorter);
    return d->m_scriptString;
}

void ExpressionSorter::setExpression(const QQmlScriptString &scriptString)
{
    Q_D(ExpressionSorter);
    if (d->m_scriptString == scriptString)
        return;

    d->m_scriptString = scriptString;
    d->updateExpression();

    Q_EMIT expressionChanged();
    Q_EMIT sorterChanged();
}

/*************************************************************/

int IndexSorterPrivate::compare(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    return sourceLeft.row() - sourceRight.row();
}

IndexSorter::IndexSorter(QObject *parent)
    : Sorter(*new IndexSorterPrivate(), parent)
{

}

/*************************************************************/

int ReverseIndexSorterPrivate::compare(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    return sourceRight.row() - sourceLeft.row();
}

ReverseIndexSorter::ReverseIndexSorter(QObject *parent)
    : Sorter(*new ReverseIndexSorterPrivate(), parent)
{

}

/*************************************************************/

void registerSorterTypes()
{
    qmlRegisterUncreatableType<Sorter>("SortFilterProxyModel", 0, 2, "Sorter", "Sorter is an abstract class");
    qmlRegisterType<RoleSorter>("SortFilterProxyModel", 0, 2, "RoleSorter");
    qmlRegisterType<ExpressionSorter>("SortFilterProxyModel", 0, 2, "ExpressionSorter");
    qmlRegisterType<IndexSorter>("SortFilterProxyModel", 0, 2, "IndexSorter");
    qmlRegisterType<ReverseIndexSorter>("SortFilterProxyModel", 0, 2, "ReverseIndexSorter");
}

Q_COREAPP_STARTUP_FUNCTION(registerSorterTypes)

}
