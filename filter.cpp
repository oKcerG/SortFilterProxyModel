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
    emit enabledChanged();
    emit filterChanged();
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
    emit invertedChanged();
    emit filterChanged();
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
    emit roleNameChanged();
    emit filterChanged();
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
    emit valueChanged();
    emit filterChanged();
}

bool ValueFilter::filterRow(const QModelIndex& sourceIndex) const
{
    return !m_value.isValid() || m_value == sourceData(sourceIndex);
}

int IndexFilter::minimumIndex() const
{
    return m_minimumIndex;
}

void IndexFilter::setMinimumIndex(int minimumIndex)
{
    if (m_minimumIndex == minimumIndex)
        return;

    m_minimumIndex = minimumIndex;
    m_minimumIndexIsSet = true;
    emit minimumIndexChanged();
    emit filterChanged();
}

void IndexFilter::resetMinimumIndex()
{
    m_minimumIndexIsSet = false;
    emit filterChanged();
}

int IndexFilter::maximumIndex() const
{
    return m_maximumIndex;
}

void IndexFilter::setMaximumIndex(int maximumIndex)
{
    if (m_maximumIndex == maximumIndex)
        return;

    m_maximumIndex = maximumIndex;
    m_maximumIndexIsSet = true;
    emit maximumIndexChanged();
    emit filterChanged();
}

void IndexFilter::resetMaximumIndex()
{
    m_maximumIndexIsSet = false;
    emit filterChanged();
}

bool IndexFilter::filterRow(const QModelIndex &sourceIndex) const
{
    int sourceRow = sourceIndex.row();
    return (!m_minimumIndexIsSet || sourceRow >= m_minimumIndex) && (!m_maximumIndexIsSet || sourceRow <= m_maximumIndex);
}

QString RegexpFilter::pattern() const
{
    return m_pattern;
}

void RegexpFilter::setPattern(const QString& pattern)
{
    if (m_pattern == pattern)
        return;

    m_pattern = pattern;
    m_regExp.setPattern(pattern);
    emit filterChanged();
    emit patternChanged();
}

QQmlSortFilterProxyModel::PatternSyntax RegexpFilter::syntax() const
{
    return m_syntax;
}

void RegexpFilter::setSyntax(QQmlSortFilterProxyModel::PatternSyntax syntax)
{
    if (m_syntax == syntax)
        return;

    m_syntax = syntax;
    m_regExp.setPatternSyntax(static_cast<QRegExp::PatternSyntax>(syntax));
    emit filterChanged();
    emit syntaxChanged();
}

Qt::CaseSensitivity RegexpFilter::caseSensitivity() const
{
    return m_caseSensitivity;
}

void RegexpFilter::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
    if (m_caseSensitivity == caseSensitivity)
        return;

    m_caseSensitivity = caseSensitivity;
    m_regExp.setCaseSensitivity(caseSensitivity);
    emit filterChanged();
    emit caseSensitivityChanged();
}

bool RegexpFilter::filterRow(const QModelIndex& sourceIndex) const
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
    emit minimumValueChanged();
    emit filterChanged();
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
    emit maximumValueChanged();
    emit filterChanged();
}

bool RangeFilter::filterRow(const QModelIndex& sourceIndex) const
{
    QVariant value = sourceData(sourceIndex);
    bool lessThanMin = m_minimumValue.isValid() && value < m_minimumValue;
    bool moreThanMax = m_maximumValue.isValid() && value > m_maximumValue;
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

    emit expressionChanged();
    emit filterChanged();
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
    qmlRegisterType<RegexpFilter>("SortFilterProxyModel", 0, 2, "RegexpFilter");
    qmlRegisterType<RangeFilter>("SortFilterProxyModel", 0, 2, "RangeFilter");
    qmlRegisterType<ExpressionFilter>("SortFilterProxyModel", 0, 2, "ExpressionFilter");
    qmlRegisterType<AnyOfFilter>("SortFilterProxyModel", 0, 2, "AnyOf");
    qmlRegisterType<AllOfFilter>("SortFilterProxyModel", 0, 2, "AllOf");
}

Q_COREAPP_STARTUP_FUNCTION(registerFilterTypes)

}
