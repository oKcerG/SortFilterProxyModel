#pragma once

#include <QObject>
#include <QScopedPointer>
#include <QQmlExpression>
#include "qqmlsortfilterproxymodel.h"

namespace qqsfpm
{

class FilterPrivate;
class Filter: public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Filter)
    Q_DISABLE_COPY(Filter)
    Filter() Q_DECL_EQ_DELETE;

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool inverted READ inverted WRITE setInverted NOTIFY invertedChanged)
    friend class QQmlSortFilterProxyModel;

public:
    ~Filter() Q_DECL_EQ_DEFAULT;

    bool enabled() const;
    void setEnabled(bool enabled);

    bool inverted() const;
    void setInverted(bool inverted);

    QQmlSortFilterProxyModel *proxyModel() const;
    void setProxyModel(QQmlSortFilterProxyModel *model, bool proxyModelCompleted);

    bool filterAcceptsRow(const QModelIndex &sourceIndex) const;

Q_SIGNALS:
    void enabledChanged();
    void invertedChanged();
    void filterChanged();
    void invalidate();

protected:
    Filter(FilterPrivate &dd, QObject *parent = Q_NULLPTR);
    const QScopedPointer<FilterPrivate> d_ptr;
};

class RoleFilterPrivate;
class RoleFilter: public Filter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RoleFilter)
    Q_DISABLE_COPY(RoleFilter)
    RoleFilter() Q_DECL_EQ_DELETE;

    Q_PROPERTY(QString roleName READ roleName WRITE setRoleName NOTIFY roleNameChanged)

public:
    ~RoleFilter() Q_DECL_EQ_DEFAULT;

    const QString &roleName() const;
    void setRoleName(const QString &roleName);

Q_SIGNALS:
    void roleNameChanged();

protected:
    RoleFilter(RoleFilterPrivate &dd, QObject *parent = Q_NULLPTR);
};

class ValueFilterPrivate;
class ValueFilter: public RoleFilter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ValueFilter)
    Q_DISABLE_COPY(ValueFilter)

    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit ValueFilter(QObject *parent = Q_NULLPTR);
    ~ValueFilter() Q_DECL_EQ_DEFAULT;

    const QVariant &value() const;
    void setValue(const QVariant &value);

protected:
    ValueFilter(ValueFilterPrivate &dd, QObject *parent = Q_NULLPTR);

Q_SIGNALS:
    void valueChanged();
};

class IndexFilterPrivate;
class IndexFilter: public Filter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(IndexFilter)
    Q_DISABLE_COPY(IndexFilter)

    Q_PROPERTY(QVariant minimumIndex READ minimumIndex WRITE setMinimumIndex NOTIFY minimumIndexChanged)
    Q_PROPERTY(QVariant maximumIndex READ maximumIndex WRITE setMaximumIndex NOTIFY maximumIndexChanged)

public:
    explicit IndexFilter(QObject *parent = Q_NULLPTR);
    ~IndexFilter() Q_DECL_EQ_DEFAULT;

    const QVariant &minimumIndex() const;
    void setMinimumIndex(const QVariant &minimumIndex);

    const QVariant &maximumIndex() const;
    void setMaximumIndex(const QVariant &maximumIndex);

protected:
    IndexFilter(IndexFilterPrivate &dd, QObject *parent = Q_NULLPTR);

Q_SIGNALS:
    void minimumIndexChanged();
    void maximumIndexChanged();
};

class RegExpFilterPrivate;
class RegExpFilter: public RoleFilter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RegExpFilter)
    Q_DISABLE_COPY(RegExpFilter)

    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
    Q_PROPERTY(QQmlSortFilterProxyModel::PatternSyntax syntax READ syntax WRITE setSyntax NOTIFY syntaxChanged)
    Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE setCaseSensitivity NOTIFY caseSensitivityChanged)

public:
    explicit RegExpFilter(QObject *parent = Q_NULLPTR);
    ~RegExpFilter() Q_DECL_EQ_DEFAULT;

    QString pattern() const;
    void setPattern(const QString &pattern);

    QQmlSortFilterProxyModel::PatternSyntax syntax() const;
    void setSyntax(QQmlSortFilterProxyModel::PatternSyntax syntax);

    Qt::CaseSensitivity caseSensitivity() const;
    void setCaseSensitivity(Qt::CaseSensitivity caseSensitivity);

protected:
    RegExpFilter(RegExpFilterPrivate &dd, QObject *parent = Q_NULLPTR);

Q_SIGNALS:
    void patternChanged();
    void syntaxChanged();
    void caseSensitivityChanged();
};

class RangeFilterPrivate;
class RangeFilter: public RoleFilter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RangeFilter)
    Q_DISABLE_COPY(RangeFilter)

    Q_PROPERTY(QVariant minimumValue READ minimumValue WRITE setMinimumValue NOTIFY minimumValueChanged)
    Q_PROPERTY(bool minimumInclusive READ minimumInclusive WRITE setMinimumInclusive NOTIFY minimumInclusiveChanged)
    Q_PROPERTY(QVariant maximumValue READ maximumValue WRITE setMaximumValue NOTIFY maximumValueChanged)
    Q_PROPERTY(bool maximumInclusive READ maximumInclusive WRITE setMaximumInclusive NOTIFY maximumInclusiveChanged)

public:
    explicit RangeFilter(QObject *parent = Q_NULLPTR);
    ~RangeFilter() Q_DECL_EQ_DEFAULT;

    QVariant minimumValue() const;
    void setMinimumValue(QVariant minimumValue);
    bool minimumInclusive() const;
    void setMinimumInclusive(bool minimumInclusive);

    QVariant maximumValue() const;
    void setMaximumValue(QVariant maximumValue);
    bool maximumInclusive() const;
    void setMaximumInclusive(bool maximumInclusive);

protected:
    RangeFilter(RangeFilterPrivate &dd, QObject *parent = Q_NULLPTR);

Q_SIGNALS:
    void minimumValueChanged();
    void minimumInclusiveChanged();
    void maximumValueChanged();
    void maximumInclusiveChanged();
};

class ExpressionFilterPrivate;
class ExpressionFilter: public Filter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ExpressionFilter)
    Q_DISABLE_COPY(ExpressionFilter)

    Q_PROPERTY(QQmlScriptString expression READ expression WRITE setExpression NOTIFY expressionChanged)

public:
    explicit ExpressionFilter(QObject *parent = Q_NULLPTR);
    ~ExpressionFilter() Q_DECL_EQ_DEFAULT;

    const QQmlScriptString &expression() const;
    void setExpression(const QQmlScriptString &scriptString);

protected:
    ExpressionFilter(ExpressionFilterPrivate &dd, QObject *parent = Q_NULLPTR);

Q_SIGNALS:
    void expressionChanged();
};

class FilterContainerPrivate;
class FilterContainer: public Filter
{
Q_OBJECT
    Q_DECLARE_PRIVATE(FilterContainer)
    Q_DISABLE_COPY(FilterContainer)
    FilterContainer() Q_DECL_EQ_DELETE;

    Q_PROPERTY(QQmlListProperty<qqsfpm::Filter> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    ~FilterContainer() Q_DECL_EQ_DEFAULT;

    QQmlListProperty<Filter> filters();

    static void append_filter(QQmlListProperty<Filter> *list, Filter *filter);
    static int count_filter(QQmlListProperty<Filter> *list);
    static Filter *at_filter(QQmlListProperty<Filter> *list, int index);
    static void clear_filters(QQmlListProperty<Filter> *list);

protected:
    FilterContainer(FilterContainerPrivate &dd, QObject *parent = Q_NULLPTR);
};

class AnyOfFilterPrivate;
class AnyOfFilter: public FilterContainer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AnyOfFilter)
    Q_DISABLE_COPY(AnyOfFilter)

public:
    explicit AnyOfFilter(QObject *parent = Q_NULLPTR);
    ~AnyOfFilter() Q_DECL_EQ_DEFAULT;
};

class AllOfFilterPrivate;
class AllOfFilter: public FilterContainer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AllOfFilter)
    Q_DISABLE_COPY(AllOfFilter)

public:
    explicit AllOfFilter(QObject *parent = Q_NULLPTR);
    ~AllOfFilter() Q_DECL_EQ_DEFAULT;
};

}
