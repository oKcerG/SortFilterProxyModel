#include "filter.h"
#include <QtQml>

namespace qqsfpm {

/*!
    \qmltype Filter
    \inqmlmodule SortFilterProxyModel
    \brief Base type for the \l SortFilterProxyModel filters

    The Filter type cannot be used directly in a QML file.
    It exists to provide a set of common properties and methods,
    available across all the other filter types that inherit from it.
    Attempting to use the Filter type directly will result in an error.
*/

Filter::Filter(QObject *parent) : QObject(parent)
{
}

/*!
    \qmlproperty bool Filter::enabled

    This property holds whether the filter is enabled.
    A disabled filter will accept every rows unconditionally (even if it's inverted).

    By default, filters are enabled.
*/
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
    Q_EMIT invalidated();
}

/*!
    \qmlproperty bool Filter::inverted

    This property holds whether the filter is inverted.
    When a filter is inverted, a row normally accepted would be rejected, and vice-versa.

    By default, filters are not inverted.
*/
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
    invalidate();
}

bool Filter::filterAcceptsRow(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    return !m_enabled || filterRow(sourceIndex, proxyModel) ^ m_inverted;
}

void Filter::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    Q_UNUSED(proxyModel)
}

void Filter::invalidate()
{
    if (m_enabled)
        Q_EMIT invalidated();
}

/*!
    \qmltype RoleFilter
    \qmlabstract
    \inherits Filter
    \inqmlmodule SortFilterProxyModel
    \brief Base type for filters based on a source model role

    The RoleFilter type cannot be used directly in a QML file.
    It exists to provide a set of common properties and methods,
    available across all the other filter types that inherit from it.
    Attempting to use the RoleFilter type directly will result in an error.
*/

/*!
    \qmlproperty string RoleFilter::roleName

    This property holds the role name that the filter is using to query the source model's data when filtering items.
*/
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
    invalidate();
}

QVariant RoleFilter::sourceData(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    return proxyModel.sourceData(sourceIndex, m_roleName);
}

/*!
    \qmltype ValueFilter
    \inherits RoleFilter
    \inqmlmodule SortFilterProxyModel
    \brief Filters rows matching exactly a value

    A ValueFilter is a simple \l RoleFilter that accepts rows matching exactly the filter's value

    In the following example, only rows with their \c favorite role set to \c true will be accepted when the checkbox is checked :
    \code
    CheckBox {
       id: showOnlyFavoriteCheckBox
    }

    SortFilterProxyModel {
       sourceModel: contactModel
       filters: ValueFilter {
           roleName: "favorite"
           value: true
           enabled: showOnlyFavoriteCheckBox.checked
       }
    }
    \endcode

*/

/*!
    \qmlproperty variant ValueFilter::value

    This property holds the value used to filter the contents of the source model.
*/
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
    invalidate();
}

bool ValueFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    return !m_value.isValid() || m_value == sourceData(sourceIndex, proxyModel);
}

/*!
    \qmltype IndexFilter
    \inherits Filter
    \inqmlmodule SortFilterProxyModel
    \brief Filters rows based on their source index

    An IndexFilter is a filter allowing contents to be filtered based on their source model index.

    In the following example, only the first row of the source model will be accepted:
    \code
    SortFilterProxyModel {
       sourceModel: contactModel
       filters: IndexFilter {
           maximumIndex: 0
       }
    }
    \endcode
*/

/*!
    \qmlproperty int IndexFilter::minimumIndex

    This property holds the minimumIndex of the filter.
    Rows with a source index lower than \c minimumIndex will be rejected.

    If \c minimumIndex is negative, it is counted from the end of the source model, meaning that :
    \code minimumIndex: -1\endcode
    is equivalent to :
    \code minimumIndex: sourceModel.count - 1\endcode
    By default, no value is set.
*/
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
    invalidate();
}

/*!
    \qmlproperty int IndexFilter::maximumIndex

    This property holds the maximumIndex of the filter.
    Rows with a source index higher than \c maximumIndex will be rejected.

    If \c maximumIndex is negative, it is counted from the end of the source model, meaning that:
    \code maximumIndex: -1\endcode
    is equivalent to :
    \code maximumIndex: sourceModel.count - 1\endcode
    By default, no value is set.
*/
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
    invalidate();
}

bool IndexFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    int sourceRowCount = proxyModel.sourceModel()->rowCount();
    int sourceRow = sourceIndex.row();

    bool minimumIsValid;
    int minimum = m_minimumIndex.toInt(&minimumIsValid);
    if (minimumIsValid) {
        int actualMinimum = minimum < 0 ? sourceRowCount + minimum : minimum;
        if (sourceRow < actualMinimum)
            return false;
    }

    bool maximumIsValid;
    int maximum = m_maximumIndex.toInt(&maximumIsValid);
    if (maximumIsValid) {
        int actualMaximum = maximum < 0 ? sourceRowCount + maximum : maximum;
        if (sourceRow > actualMaximum)
            return false;
    }

    return true;
}

/*!
    \qmltype RegExpFilter
    \inherits RoleFilter
    \inqmlmodule SortFilterProxyModel
    \brief  Filters rows matching a regular expression

    A RegExpFilter is a \l RoleFilter that accepts rows matching a regular rexpression.

    In the following example, only rows with their \c lastName role beggining with the content of textfield the will be accepted:
    \code
    TextField {
       id: nameTextField
    }

    SortFilterProxyModel {
       sourceModel: contactModel
       filters: RegExpFilter {
           roleName: "lastName"
           pattern: "^" + nameTextField.displayText
       }
    }
    \endcode
*/

/*!
    \qmlproperty bool RegExpFilter::pattern

    The pattern used to filter the contents of the source model.

    \sa syntax
*/
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
    Q_EMIT patternChanged();
    invalidate();
}

/*!
    \qmlproperty enum RegExpFilter::syntax

    The pattern used to filter the contents of the source model.

    Only the source model's value having their \l roleName data matching this \l pattern with the specified \l syntax will be kept.

    \value RegExpFilter.RegExp A rich Perl-like pattern matching syntax. This is the default.
    \value RegExpFilter.Wildcard This provides a simple pattern matching syntax similar to that used by shells (command interpreters) for "file globbing".
    \value RegExpFilter.FixedString The pattern is a fixed string. This is equivalent to using the RegExp pattern on a string in which all metacharacters are escaped.
    \value RegExpFilter.RegExp2 Like RegExp, but with greedy quantifiers.
    \value RegExpFilter.WildcardUnix This is similar to Wildcard but with the behavior of a Unix shell. The wildcard characters can be escaped with the character "\".
    \value RegExpFilter.W3CXmlSchema11 The pattern is a regular expression as defined by the W3C XML Schema 1.1 specification.

    \sa pattern
*/
RegExpFilter::PatternSyntax RegExpFilter::syntax() const
{
    return m_syntax;
}

void RegExpFilter::setSyntax(RegExpFilter::PatternSyntax syntax)
{
    if (m_syntax == syntax)
        return;

    m_syntax = syntax;
    m_regExp.setPatternSyntax(static_cast<QRegExp::PatternSyntax>(syntax));
    Q_EMIT syntaxChanged();
    invalidate();
}

/*!
    \qmlproperty Qt::CaseSensitivity RegExpFilter::caseSensitivity

    This property holds the caseSensitivity of the filter.
*/
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
    Q_EMIT caseSensitivityChanged();
    invalidate();
}

bool RegExpFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    QString string = sourceData(sourceIndex, proxyModel).toString();
    return m_regExp.indexIn(string) != -1;
}

/*!
    \qmltype RangeFilter
    \inherits RoleFilter
    \inqmlmodule SortFilterProxyModel
    \brief Filters rows between boundary values

    A RangeFilter is a \l RoleFilter that accepts rows if their data is between the filter's minimum and maximum value.

    In the following example, only rows with their \c price role set to a value between the tow boundary of the slider will be accepted :
    \code
    RangeSlider {
       id: priceRangeSlider
    }

    SortFilterProxyModel {
       sourceModel: priceModel
       filters: RangeFilter {
           roleName: "price"
           minimumValue: priceRangeSlider.first.value
           maximumValue: priceRangeSlider.second.value
       }
    }
    \endcode
*/

/*!
    \qmlproperty int RangeFilter::minimumValue

    This property holds the minimumValue of the filter.
    Rows with a value lower than \c minimumValue will be rejected.

    By default, no value is set.

    \sa minimumInclusive
*/
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
    invalidate();
}

/*!
    \qmlproperty int RangeFilter::minimumInclusive

    This property holds whether the \l minimumValue is inclusive.

    By default, the \l minimumValue is inclusive.

    \sa minimumValue
*/
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
    invalidate();
}

/*!
    \qmlproperty int RangeFilter::maximumValue

    This property holds the maximumValue of the filter.
    Rows with a value higher than \c maximumValue will be rejected.

    By default, no value is set.

    \sa maximumInclusive
*/
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
    invalidate();
}

/*!
    \qmlproperty int RangeFilter::maximumInclusive

    This property holds whether the \l minimumValue is inclusive.

    By default, the \l minimumValue is inclusive.

    \sa minimumValue
*/
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
    invalidate();
}

bool RangeFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    QVariant value = sourceData(sourceIndex, proxyModel);
    bool lessThanMin = m_minimumValue.isValid() &&
            m_minimumInclusive ? value < m_minimumValue : value <= m_minimumValue;
    bool moreThanMax = m_maximumValue.isValid() &&
            m_maximumInclusive ? value > m_maximumValue : value >= m_maximumValue;
    return !(lessThanMin || moreThanMax);
}

/*!
    \qmltype ExpressionFilter
    \inherits Filter
    \inqmlmodule SortFilterProxyModel
    \brief Filters row with a custom filtering

    An ExpressionFilter is a \l Filter allowing to implement custom filtering based on a javascript expression.
*/

/*!
    \qmlproperty expression ExpressionFilter::expression

    An expression to implement custom filtering, it must evaluate to a boolean.
    It has the same syntax has a \l {http://doc.qt.io/qt-5/qtqml-syntax-propertybinding.html} {Property Binding} except it will be evaluated for each of the source model's rows.
    Rows that have their expression evaluating to \c true will be accepted by the model.
    Data for each row is exposed like for a delegate of a QML View.

    This expression is reevaluated for a row every time its model data changes.
    When an external property (not \c index or in \c model) the expression depends on changes, the expression is reevaluated for every row of the source model.
    To capture the properties the expression depends on, the expression is first executed with invalid data and each property access is detected by the QML engine.
    This means that if a property is not accessed because of a conditional, it won't be captured and the expression won't be reevaluted when this property changes.

    A workaround to this problem is to access all the properties the expressions depends unconditionally at the beggining of the expression.
*/
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
    invalidate();
}

void ExpressionFilter::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    updateContext(proxyModel);
}

bool ExpressionFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
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

void ExpressionFilter::updateContext(const QQmlSortFilterProxyModel& proxyModel)
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

void ExpressionFilter::updateExpression()
{
    if (!m_context)
        return;

    delete m_expression;
    m_expression = new QQmlExpression(m_scriptString, m_context, 0, this);
    connect(m_expression, &QQmlExpression::valueChanged, this, &ExpressionFilter::invalidate);
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

void FilterContainer::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    for (Filter* filter : m_filters)
        filter->proxyModelCompleted(proxyModel);
}

void FilterContainer::append_filter(QQmlListProperty<Filter>* list, Filter* filter)
{
    if (!filter)
        return;

    FilterContainer* that = static_cast<FilterContainer*>(list->object);
    that->m_filters.append(filter);
    connect(filter, &Filter::invalidated, that, &FilterContainer::invalidate);
    that->invalidate();
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
    that->invalidate();
}

/*!
    \qmltype AnyOf
    \inherits Filter
    \inqmlmodule SortFilterProxyModel
    \brief Filter container accepting rows accepted by at least one of its child filters

    The AnyOf type is a \l Filter container that accepts rows if any of its contained (and enabled) filters accept them.

    In the following example, only the rows where the \c firstName role or the \c lastName role match the text entered in the \c nameTextField will be accepted :
    \code
    TextField {
      id: nameTextField
    }

    SortFilterProxyModel {
      sourceModel: contactModel
      filters: AnyOf {
          RegExpFilter {
              roleName: "lastName"
              pattern: nameTextField.text
              caseSensitivity: Qt.CaseInsensitive
          }
          RegExpFilter {
              roleName: "firstName"
              pattern: nameTextField.text
              caseSensitivity: Qt.CaseInsensitive
          }
      }
    }
    \endcode
*/
bool AnyOfFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    //return true if any of the enabled filters return true
    return std::any_of(m_filters.begin(), m_filters.end(),
        [&sourceIndex, &proxyModel] (Filter* filter) {
            return filter->enabled() && filter->filterAcceptsRow(sourceIndex, proxyModel);
        }
    );
}

/*!
    \qmltype AllOf
    \inherits Filter
    \inqmlmodule SortFilterProxyModel
    \brief Filter container accepting rows accepted by all its child filters

    The AllOf type is a \l Filter container that accepts rows if all of its contained (and enabled) filters accept them, or if it has no filter.

    Using it as a top level filter has the same effect as putting all its child filters as top level filters. It can however be usefull to use an AllOf filter when nested in an AnyOf filter.
*/
bool AllOfFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    //return true if all filters return false, or if there is no filter.
    return std::all_of(m_filters.begin(), m_filters.end(),
        [&sourceIndex, &proxyModel] (Filter* filter) {
            return filter->filterAcceptsRow(sourceIndex, proxyModel);
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
