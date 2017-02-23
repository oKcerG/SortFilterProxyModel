#include "private/filter_p.h"
#include <QtQml>

namespace qqsfpm
{

void FilterPrivate::proxyModelCompleted()
{

}

Filter::Filter(FilterPrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&dd)
{
    Q_D(Filter);
    d->q_ptr = this;
    connect(this, &Filter::filterChanged, this, [&]
    {
        if (d->m_enabled)
            invalidate();
    });
}

bool Filter::enabled() const
{
    Q_D(const Filter);
    return d->m_enabled;
}

void Filter::setEnabled(bool enabled)
{
    Q_D(Filter);
    if (d->m_enabled == enabled)
        return;

    d->m_enabled = enabled;
    Q_EMIT enabledChanged();
    Q_EMIT invalidate();
}

bool Filter::inverted() const
{
    Q_D(const Filter);
    return d->m_inverted;
}

void Filter::setInverted(bool inverted)
{
    Q_D(Filter);
    if (d->m_inverted == inverted)
        return;

    d->m_inverted = inverted;
    Q_EMIT invertedChanged();
    Q_EMIT filterChanged();
}

QQmlSortFilterProxyModel *Filter::proxyModel() const
{
    Q_D(const Filter);
    return d->m_proxyModel;
}

void Filter::setProxyModel(QQmlSortFilterProxyModel *model, bool proxyModelCompleted)
{
    Q_D(Filter);
    d->m_proxyModel = model;

    if (!proxyModelCompleted)
        return;

    d->proxyModelCompleted();
}

bool Filter::filterAcceptsRow(const QModelIndex &sourceIndex) const
{
    Q_D(const Filter);
    return !d->m_enabled || d->filterRow(sourceIndex) ^ d->m_inverted;
}

/*************************************************************/

QVariant RoleFilterPrivate::sourceData(const QModelIndex &sourceIndex) const
{
    return m_proxyModel->sourceData(sourceIndex, m_roleName);
}

RoleFilter::RoleFilter(RoleFilterPrivate &dd, QObject *parent)
    : Filter(dd, parent)
{

}

const QString &RoleFilter::roleName() const
{
    Q_D(const RoleFilter);
    return d->m_roleName;
}

void RoleFilter::setRoleName(const QString &roleName)
{
    Q_D(RoleFilter);
    if (d->m_roleName == roleName)
        return;

    d->m_roleName = roleName;
    Q_EMIT roleNameChanged();
    Q_EMIT filterChanged();
}

/*************************************************************/

bool ValueFilterPrivate::filterRow(const QModelIndex &sourceIndex) const
{
    return !m_value.isValid() || m_value == sourceData(sourceIndex);
}

ValueFilter::ValueFilter(QObject *parent)
    : RoleFilter(*new ValueFilterPrivate(), parent)
{

}

ValueFilter::ValueFilter(ValueFilterPrivate &dd, QObject *parent)
    : RoleFilter(dd, parent)
{

}

const QVariant &ValueFilter::value() const
{
    Q_D(const ValueFilter);
    return d->m_value;
}

void ValueFilter::setValue(const QVariant &value)
{
    Q_D(ValueFilter);
    if (d->m_value == value)
        return;

    d->m_value = value;
    Q_EMIT valueChanged();
    Q_EMIT filterChanged();
}

/*************************************************************/

bool IndexFilterPrivate::filterRow(const QModelIndex &sourceIndex) const
{
    int sourceRowCount = m_proxyModel->sourceModel()->rowCount();
    int sourceRow = sourceIndex.row();

    bool minimumIsValid;
    int minimum = m_minimumIndex.toInt(&minimumIsValid);
    int actualMinimum = (sourceRowCount + minimum) % sourceRowCount;
    bool lowerThanMinimumIndex = minimumIsValid && sourceRow < actualMinimum;

    bool maximumIsValid;
    int maximum = m_maximumIndex.toInt(&maximumIsValid);
    int actualMaximum = (sourceRowCount + maximum) % sourceRowCount;
    bool greaterThanMaximumIndex = maximumIsValid && sourceRow > actualMaximum;

    return !lowerThanMinimumIndex && !greaterThanMaximumIndex;
}

IndexFilter::IndexFilter(QObject *parent)
    : Filter(*new IndexFilterPrivate(), parent)
{

}

IndexFilter::IndexFilter(IndexFilterPrivate &dd, QObject *parent)
    : Filter(dd, parent)
{

}

const QVariant &IndexFilter::minimumIndex() const
{
    Q_D(const IndexFilter);
    return d->m_minimumIndex;
}

void IndexFilter::setMinimumIndex(const QVariant &minimumIndex)
{
    Q_D(IndexFilter);
    if (d->m_minimumIndex == minimumIndex)
        return;

    d->m_minimumIndex = minimumIndex;
    Q_EMIT minimumIndexChanged();
    Q_EMIT filterChanged();
}

const QVariant &IndexFilter::maximumIndex() const
{
    Q_D(const IndexFilter);
    return d->m_maximumIndex;
}

void IndexFilter::setMaximumIndex(const QVariant &maximumIndex)
{
    Q_D(IndexFilter);
    if (d->m_maximumIndex == maximumIndex)
        return;

    d->m_maximumIndex = maximumIndex;
    Q_EMIT maximumIndexChanged();
    Q_EMIT filterChanged();
}

/*************************************************************/

bool RegExpFilterPrivate::filterRow(const QModelIndex &sourceIndex) const
{
    QString string = sourceData(sourceIndex).toString();
    return m_regExp.indexIn(string) != -1;
}

RegExpFilter::RegExpFilter(QObject *parent)
    : RoleFilter(*new RegExpFilterPrivate(), parent)
{

}

RegExpFilter::RegExpFilter(RegExpFilterPrivate &dd, QObject *parent)
    : RoleFilter(dd, parent)
{

}

QString RegExpFilter::pattern() const
{
    Q_D(const RegExpFilter);
    return d->m_pattern;
}

void RegExpFilter::setPattern(const QString &pattern)
{
    Q_D(RegExpFilter);
    if (d->m_pattern == pattern)
        return;

    d->m_pattern = pattern;
    d->m_regExp.setPattern(pattern);
    Q_EMIT filterChanged();
    Q_EMIT patternChanged();
}

QQmlSortFilterProxyModel::PatternSyntax RegExpFilter::syntax() const
{
    Q_D(const RegExpFilter);
    return d->m_syntax;
}

void RegExpFilter::setSyntax(QQmlSortFilterProxyModel::PatternSyntax syntax)
{
    Q_D(RegExpFilter);
    if (d->m_syntax == syntax)
        return;

    d->m_syntax = syntax;
    d->m_regExp.setPatternSyntax(static_cast<QRegExp::PatternSyntax>(syntax));
    Q_EMIT filterChanged();
    Q_EMIT syntaxChanged();
}

Qt::CaseSensitivity RegExpFilter::caseSensitivity() const
{
    Q_D(const RegExpFilter);
    return d->m_caseSensitivity;
}

void RegExpFilter::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
    Q_D(RegExpFilter);
    if (d->m_caseSensitivity == caseSensitivity)
        return;

    d->m_caseSensitivity = caseSensitivity;
    d->m_regExp.setCaseSensitivity(caseSensitivity);
    Q_EMIT filterChanged();
    Q_EMIT caseSensitivityChanged();
}

/*************************************************************/

bool RangeFilterPrivate::filterRow(const QModelIndex &sourceIndex) const
{
    QVariant value = sourceData(sourceIndex);
    bool lessThanMin = m_minimumValue.isValid() &&
        m_minimumInclusive ? value < m_minimumValue : value <= m_minimumValue;
    bool moreThanMax = m_maximumValue.isValid() &&
        m_maximumInclusive ? value > m_maximumValue : value >= m_maximumValue;
    return !(lessThanMin || moreThanMax);
}

RangeFilter::RangeFilter(QObject *parent)
    : RoleFilter(*new RangeFilterPrivate(), parent)
{

}

RangeFilter::RangeFilter(RangeFilterPrivate &dd, QObject *parent)
    : RoleFilter(dd, parent)
{

}

QVariant RangeFilter::minimumValue() const
{
    Q_D(const RangeFilter);
    return d->m_minimumValue;
}

void RangeFilter::setMinimumValue(QVariant minimumValue)
{
    Q_D(RangeFilter);
    if (d->m_minimumValue == minimumValue)
        return;

    d->m_minimumValue = minimumValue;
    Q_EMIT minimumValueChanged();
    Q_EMIT filterChanged();
}

bool RangeFilter::minimumInclusive() const
{
    Q_D(const RangeFilter);
    return d->m_minimumInclusive;
}

void RangeFilter::setMinimumInclusive(bool minimumInclusive)
{
    Q_D(RangeFilter);
    if (d->m_minimumInclusive == minimumInclusive)
        return;

    d->m_minimumInclusive = minimumInclusive;
    Q_EMIT minimumInclusiveChanged();
    Q_EMIT filterChanged();
}

QVariant RangeFilter::maximumValue() const
{
    Q_D(const RangeFilter);
    return d->m_maximumValue;
}

void RangeFilter::setMaximumValue(QVariant maximumValue)
{
    Q_D(RangeFilter);
    if (d->m_maximumValue == maximumValue)
        return;

    d->m_maximumValue = maximumValue;
    Q_EMIT maximumValueChanged();
    Q_EMIT filterChanged();
}

bool RangeFilter::maximumInclusive() const
{
    Q_D(const RangeFilter);
    return d->m_maximumInclusive;
}

void RangeFilter::setMaximumInclusive(bool maximumInclusive)
{
    Q_D(RangeFilter);
    if (d->m_maximumInclusive == maximumInclusive)
        return;

    d->m_maximumInclusive = maximumInclusive;
    Q_EMIT maximumInclusiveChanged();
    Q_EMIT filterChanged();
}

/*************************************************************/

bool ExpressionFilterPrivate::filterRow(const QModelIndex &sourceIndex) const
{
    Q_Q(const ExpressionFilter);
    if (!m_scriptString.isEmpty()) {
        QVariantMap modelMap;
        QHash<int, QByteArray> roles = m_proxyModel->roleNames();

        QQmlContext context(qmlContext(q));
        auto addToContext = [&](const QString &name, const QVariant &value)
        {
            context.setContextProperty(name, value);
            modelMap.insert(name, value);
        };

        for (auto it = roles.cbegin(); it != roles.cend(); ++it)
            addToContext(it.value(), m_proxyModel->sourceData(sourceIndex, it.key()));
        addToContext("index", sourceIndex.row());

        context.setContextProperty("model", modelMap);

        QQmlExpression expression(m_scriptString, &context);
        QVariant variantResult = expression.evaluate();

        if (expression.hasError()) {
            qWarning() << expression.error();
            return true;
        }
        if (variantResult.canConvert<bool>()) {
            return variantResult.toBool();
        }
        else {
            qWarning("%s:%i:%i : Can't convert result to bool",
                     expression.sourceFile().toUtf8().data(),
                     expression.lineNumber(),
                     expression.columnNumber());
            return true;
        }
    }
    return true;
}
void ExpressionFilterPrivate::proxyModelCompleted()
{
    updateContext();
}

void ExpressionFilterPrivate::updateContext()
{
    Q_Q(ExpressionFilter);
    if (!m_proxyModel)
        return;

    delete m_context;
    m_context = new QQmlContext(qmlContext(q), q);
    // what about roles changes ?
    QVariantMap modelMap;

    auto addToContext = [&](const QString &name, const QVariant &value)
    {
        m_context->setContextProperty(name, value);
        modelMap.insert(name, value);
    };

    for (const QByteArray &roleName : m_proxyModel->roleNames().values())
        addToContext(roleName, QVariant());

    addToContext("index", -1);

    m_context->setContextProperty("model", modelMap);
    updateExpression();
}

void ExpressionFilterPrivate::updateExpression()
{
    Q_Q(ExpressionFilter);
    if (!m_context)
        return;

    delete m_expression;
    m_expression = new QQmlExpression(m_scriptString, m_context, 0, q);
    QObject::connect(m_expression, &QQmlExpression::valueChanged, q, &Filter::filterChanged);
    m_expression->setNotifyOnValueChanged(true);
    m_expression->evaluate();
}

ExpressionFilter::ExpressionFilter(QObject *parent)
    : Filter(*new ExpressionFilterPrivate(), parent)
{

}

ExpressionFilter::ExpressionFilter(ExpressionFilterPrivate &dd, QObject *parent)
    : Filter(dd, parent)
{

}

const QQmlScriptString &ExpressionFilter::expression() const
{
    Q_D(const ExpressionFilter);
    return d->m_scriptString;
}

void ExpressionFilter::setExpression(const QQmlScriptString &scriptString)
{
    Q_D(ExpressionFilter);
    if (d->m_scriptString == scriptString)
        return;

    d->m_scriptString = scriptString;
    d->updateExpression();

    Q_EMIT expressionChanged();
    Q_EMIT filterChanged();
}

/*************************************************************/

void FilterContainerPrivate::proxyModelCompleted()
{
    for (Filter *filter : m_filters)
        filter->setProxyModel(m_proxyModel, true);
}

FilterContainer::FilterContainer(FilterContainerPrivate &dd, QObject *parent)
    : Filter(dd, parent)
{

}

QQmlListProperty<Filter> FilterContainer::filters()
{
    Q_D(FilterContainer);
    return QQmlListProperty<Filter>(this, &d->m_filters,
                                    &FilterContainer::append_filter,
                                    &FilterContainer::count_filter,
                                    &FilterContainer::at_filter,
                                    &FilterContainer::clear_filters);
}

void FilterContainer::append_filter(QQmlListProperty<Filter> *list, Filter *filter)
{
    if (!filter)
        return;

    FilterContainer *that = static_cast<FilterContainer *>(list->object);
    FilterContainerPrivate *thatPrivate = reinterpret_cast<FilterContainerPrivate *>(that->d_ptr.data());

    thatPrivate->m_filters.append(filter);
    connect(filter, &Filter::invalidate, that, &Filter::filterChanged);
    that->filterChanged();
}

int FilterContainer::count_filter(QQmlListProperty<Filter> *list)
{
    QList<Filter *> *filters = static_cast<QList<Filter *> *>(list->data);
    return filters->count();
}

Filter *FilterContainer::at_filter(QQmlListProperty<Filter> *list, int index)
{
    QList<Filter *> *filters = static_cast<QList<Filter *> *>(list->data);
    return filters->at(index);
}

void FilterContainer::clear_filters(QQmlListProperty<Filter> *list)
{
    FilterContainer *that = static_cast<FilterContainer *>(list->object);
    FilterContainerPrivate *thatPrivate = reinterpret_cast<FilterContainerPrivate *>(that->d_ptr.data());

    thatPrivate->m_filters.clear();
    that->filterChanged();
}

/*************************************************************/

bool AnyOfFilterPrivate::filterRow(const QModelIndex &sourceIndex) const
{
    //return true if any of the enabled filters return true
    return std::any_of(m_filters.begin(), m_filters.end(),
                       [&sourceIndex](Filter *filter)
                       {
                           return filter->enabled() && filter->filterAcceptsRow(sourceIndex);
                       }
    );
}

AnyOfFilter::AnyOfFilter(QObject *parent)
    : FilterContainer(*new AnyOfFilterPrivate(), parent)
{

}

/*************************************************************/

bool AllOfFilterPrivate::filterRow(const QModelIndex &sourceIndex) const
{
    //return true if all filters return false, or if there is no filter.
    return std::all_of(m_filters.begin(), m_filters.end(),
                       [&sourceIndex](Filter *filter)
                       {
                           return filter->filterAcceptsRow(sourceIndex);
                       }
    );
}

AllOfFilter::AllOfFilter(QObject *parent)
    : FilterContainer(*new AnyOfFilterPrivate(), parent)
{

}

/*************************************************************/

void registerFilterTypes()
{
    qmlRegisterUncreatableType<Filter>("SortFilterProxyModel", 0, 2, "Filter", "Filter is an abstract class");
    qmlRegisterType<ValueFilter>("SortFilterProxyModel", 0, 2, "ValueFilter");
    qmlRegisterType<IndexFilter>("SortFilterProxyModel", 0, 2, "IndexFilter");
    qmlRegisterType<RegExpFilter>("SortFilterProxyModel", 0, 2, "RegExpFilter");
    qmlRegisterType<RangeFilter>("SortFilterProxyModel", 0, 2, "RangeFilter");
    qmlRegisterType<ExpressionFilter>("SortFilterProxyModel", 0, 2, "ExpressionFilter");
    qmlRegisterType<AnyOfFilter>("SortFilterProxyModel", 0, 2, "AnyOf");
    qmlRegisterType<AllOfFilter>("SortFilterProxyModel", 0, 2, "AllOf");
}

Q_COREAPP_STARTUP_FUNCTION(registerFilterTypes)

}
