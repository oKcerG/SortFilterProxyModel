#include "filter.h"
#include <QtQml>

namespace qqsfpm {

Filter::Filter(QObject *parent) : QObject(parent)
{
    connect(this, &Filter::filterChanged, this, &Filter::onFilterChanged);
}

bool Filter::enabled() const
{
    return m_enabled;
}

void Filter::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    Q_EMIT enabledChanged();
    Q_EMIT invalidate();
}

bool Filter::inverted() const
{
    return m_inverted;
}

void Filter::setInverted(bool inverted)
{
    if (m_inverted == inverted)
        return;

    m_inverted = inverted;
    Q_EMIT invertedChanged();
    Q_EMIT filterChanged();
}

bool Filter::filterAcceptsRow(const QModelIndex &sourceIndex) const
{
    return !m_enabled || filterRow(sourceIndex) ^ m_inverted;
}

QQmlSortFilterProxyModel* Filter::proxyModel() const
{
    return m_proxyModel;
}

void Filter::proxyModelCompleted()
{

}

void Filter::onFilterChanged()
{
    if (m_enabled)
        invalidate();
}

const QString& RoleFilter::roleName() const
{
    return m_roleName;
}

void RoleFilter::setRoleName(const QString& roleName)
{
    if (m_roleName == roleName)
        return;

    m_roleName = roleName;
    Q_EMIT roleNameChanged();
    Q_EMIT filterChanged();
}

QVariant RoleFilter::sourceData(const QModelIndex &sourceIndex) const
{
    return proxyModel()->sourceData(sourceIndex, m_roleName);
}

const QVariant &ValueFilter::value() const
{
    return m_value;
}

void ValueFilter::setValue(const QVariant& value)
{
    if (m_value == value)
        return;

    m_value = value;
    Q_EMIT valueChanged();
    Q_EMIT filterChanged();
}

bool ValueFilter::filterRow(const QModelIndex& sourceIndex) const
{
    return !m_value.isValid() || m_value == sourceData(sourceIndex);
}

const QVariant& IndexFilter::minimumIndex() const
{
    return m_minimumIndex;
}

void IndexFilter::setMinimumIndex(const QVariant& minimumIndex)
{
    if (m_minimumIndex == minimumIndex)
        return;

    m_minimumIndex = minimumIndex;
    Q_EMIT minimumIndexChanged();
    Q_EMIT filterChanged();
}

const QVariant& IndexFilter::maximumIndex() const
{
    return m_maximumIndex;
}

void IndexFilter::setMaximumIndex(const QVariant& maximumIndex)
{
    if (m_maximumIndex == maximumIndex)
        return;

    m_maximumIndex = maximumIndex;
    Q_EMIT maximumIndexChanged();
    Q_EMIT filterChanged();
}

bool IndexFilter::filterRow(const QModelIndex& sourceIndex) const
{
    int sourceRowCount = proxyModel()->sourceModel()->rowCount();
    int sourceRow = sourceIndex.row();

    bool minimumIsValid;
    int minimum = m_minimumIndex.toInt(&minimumIsValid);
    int actualMinimum = (sourceRowCount + minimum) % sourceRowCount;
    bool lowerThanMinimumIndex = minimumIsValid && sourceRow < actualMinimum;

    bool maximumIsValid;
    int maximum = m_maximumIndex.toInt(&maximumIsValid);
    int actualMaximum = (sourceRowCount + maximum) % sourceRowCount;
    bool greaterThanMaximumIndex = maximumIsValid && sourceRow >actualMaximum;

    return !lowerThanMinimumIndex && !greaterThanMaximumIndex;
}

QString RegExpFilter::pattern() const
{
    return m_pattern;
}

void RegExpFilter::setPattern(const QString& pattern)
{
    if (m_pattern == pattern)
        return;

    m_pattern = pattern;
    m_regExp.setPattern(pattern);
    Q_EMIT filterChanged();
    Q_EMIT patternChanged();
}

QQmlSortFilterProxyModel::PatternSyntax RegExpFilter::syntax() const
{
    return m_syntax;
}

void RegExpFilter::setSyntax(QQmlSortFilterProxyModel::PatternSyntax syntax)
{
    if (m_syntax == syntax)
        return;

    m_syntax = syntax;
    m_regExp.setPatternSyntax(static_cast<QRegExp::PatternSyntax>(syntax));
    Q_EMIT filterChanged();
    Q_EMIT syntaxChanged();
}

Qt::CaseSensitivity RegExpFilter::caseSensitivity() const
{
    return m_caseSensitivity;
}

void RegExpFilter::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
    if (m_caseSensitivity == caseSensitivity)
        return;

    m_caseSensitivity = caseSensitivity;
    m_regExp.setCaseSensitivity(caseSensitivity);
    Q_EMIT filterChanged();
    Q_EMIT caseSensitivityChanged();
}

bool RegExpFilter::filterRow(const QModelIndex& sourceIndex) const
{
    QString string = sourceData(sourceIndex).toString();
    return m_regExp.indexIn(string) != -1;
}

QVariant RangeFilter::minimumValue() const
{
    return m_minimumValue;
}

void RangeFilter::setMinimumValue(QVariant minimumValue)
{
    if (m_minimumValue == minimumValue)
        return;

    m_minimumValue = minimumValue;
    Q_EMIT minimumValueChanged();
    Q_EMIT filterChanged();
}

bool RangeFilter::minimumInclusive() const
{
    return m_minimumInclusive;
}

void RangeFilter::setMinimumInclusive(bool minimumInclusive)
{
    if (m_minimumInclusive == minimumInclusive)
        return;

    m_minimumInclusive = minimumInclusive;
    Q_EMIT minimumInclusiveChanged();
    Q_EMIT filterChanged();
}

QVariant RangeFilter::maximumValue() const
{
    return m_maximumValue;
}

void RangeFilter::setMaximumValue(QVariant maximumValue)
{
    if (m_maximumValue == maximumValue)
        return;

    m_maximumValue = maximumValue;
    Q_EMIT maximumValueChanged();
    Q_EMIT filterChanged();
}

bool RangeFilter::maximumInclusive() const
{
    return m_maximumInclusive;
}

void RangeFilter::setMaximumInclusive(bool maximumInclusive)
{
    if (m_maximumInclusive == maximumInclusive)
        return;

    m_maximumInclusive = maximumInclusive;
    Q_EMIT maximumInclusiveChanged();
    Q_EMIT filterChanged();
}

bool RangeFilter::filterRow(const QModelIndex& sourceIndex) const
{
    QVariant value = sourceData(sourceIndex);
    bool lessThanMin = m_minimumValue.isValid() &&
            m_minimumInclusive ? value < m_minimumValue : value <= m_minimumValue;
    bool moreThanMax = m_maximumValue.isValid() &&
            m_maximumInclusive ? value > m_maximumValue : value >= m_maximumValue;
    return !(lessThanMin || moreThanMax);
}

const QQmlScriptString& ExpressionFilter::expression() const
{
    return m_scriptString;
}

void ExpressionFilter::setExpression(const QQmlScriptString& scriptString)
{
    if (m_scriptString == scriptString)
        return;

    m_scriptString = scriptString;
    updateExpression();

    Q_EMIT expressionChanged();
    Q_EMIT filterChanged();
}

bool ExpressionFilter::filterRow(const QModelIndex& sourceIndex) const
{
    if (!m_scriptString.isEmpty()) {
        QVariantMap modelMap;
        QHash<int, QByteArray> roles = proxyModel()->roleNames();

        QQmlContext context(qmlContext(this));
        auto addToContext = [&] (const QString &name, const QVariant& value) {
            context.setContextProperty(name, value);
            modelMap.insert(name, value);
        };

        for (auto it = roles.cbegin(); it != roles.cend(); ++it)
            addToContext(it.value(), proxyModel()->sourceData(sourceIndex, it.key()));
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
        } else {
            qWarning("%s:%i:%i : Can't convert result to bool",
                     expression.sourceFile().toUtf8().data(),
                     expression.lineNumber(),
                     expression.columnNumber());
            return true;
        }
    }
    return true;
}

void ExpressionFilter::proxyModelCompleted()
{
    updateContext();
}

void ExpressionFilter::updateContext()
{
    if (!proxyModel())
        return;

    delete m_context;
    m_context = new QQmlContext(qmlContext(this), this);
    // what about roles changes ?
    QVariantMap modelMap;

    auto addToContext = [&] (const QString &name, const QVariant& value) {
        m_context->setContextProperty(name, value);
        modelMap.insert(name, value);
    };

    for (const QByteArray& roleName : proxyModel()->roleNames().values())
        addToContext(roleName, QVariant());

    addToContext("index", -1);

    m_context->setContextProperty("model", modelMap);
    updateExpression();
}

void ExpressionFilter::updateExpression()
{
    if (!m_context)
        return;

    delete m_expression;
    m_expression = new QQmlExpression(m_scriptString, m_context, 0, this);
    connect(m_expression, &QQmlExpression::valueChanged, this, &Filter::filterChanged);
    m_expression->setNotifyOnValueChanged(true);
    m_expression->evaluate();
}

QQmlListProperty<Filter> FilterContainer::filters()
{
    return QQmlListProperty<Filter>(this, &m_filters,
                                    &FilterContainer::append_filter,
                                    &FilterContainer::count_filter,
                                    &FilterContainer::at_filter,
                                    &FilterContainer::clear_filters);
}

void FilterContainer::append_filter(QQmlListProperty<Filter>* list, Filter* filter)
{
    if (!filter)
        return;

    FilterContainer* that = static_cast<FilterContainer*>(list->object);
    that->m_filters.append(filter);
    connect(filter, &Filter::invalidate, that, &Filter::filterChanged);
    that->filterChanged();
}

int FilterContainer::count_filter(QQmlListProperty<Filter>* list)
{
    QList<Filter*>* filters = static_cast<QList<Filter*>*>(list->data);
    return filters->count();
}

Filter* FilterContainer::at_filter(QQmlListProperty<Filter>* list, int index)
{
    QList<Filter*>* filters = static_cast<QList<Filter*>*>(list->data);
    return filters->at(index);
}

void FilterContainer::clear_filters(QQmlListProperty<Filter> *list)
{
    FilterContainer* that = static_cast<FilterContainer*>(list->object);
    that->m_filters.clear();
    that->filterChanged();
}

void FilterContainer::proxyModelCompleted()
{
    for (Filter* filter : m_filters) {
        filter->m_proxyModel = proxyModel();
        filter->proxyModelCompleted();
    }
}

bool AnyOfFilter::filterRow(const QModelIndex& sourceIndex) const
{
    //return true if any of the enabled filters return true
    return std::any_of(m_filters.begin(), m_filters.end(),
        [&sourceIndex] (Filter* filter) {
            return filter->enabled() && filter->filterAcceptsRow(sourceIndex);
        }
    );
}

bool AllOfFilter::filterRow(const QModelIndex& sourceIndex) const
{
    //return true if all filters return false, or if there is no filter.
    return std::all_of(m_filters.begin(), m_filters.end(),
        [&sourceIndex] (Filter* filter) {
            return filter->filterAcceptsRow(sourceIndex);
        }
    );
}

void registerFilterTypes() {
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
