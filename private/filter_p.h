#pragma once
#include "filter.h"
#include <QString>
#include <QVariant>

namespace qqsfpm {

class FilterPrivate
{
    Q_DECLARE_PUBLIC(Filter)
    friend class FilterContainerPrivate;

public:
    FilterPrivate() {}
    virtual ~FilterPrivate() Q_DECL_EQ_DEFAULT;

    bool m_enabled = true;
    bool m_inverted = false;
    QQmlSortFilterProxyModel* m_proxyModel = Q_NULLPTR;
    virtual void proxyModelCompleted();

protected:
    Filter *q_ptr = Q_NULLPTR;

    virtual bool filterRow(const QModelIndex &sourceIndex) const = 0;
};

/*************************************************************/

class RoleFilterPrivate: public FilterPrivate
{
    Q_DECLARE_PUBLIC(RoleFilter)

public:
    RoleFilterPrivate() {}
    ~RoleFilterPrivate() Q_DECL_EQ_DEFAULT;

    QString m_roleName;

protected:
    QVariant sourceData(const QModelIndex &sourceIndex) const;
};

/*************************************************************/

class ValueFilterPrivate: public RoleFilterPrivate
{
    Q_DECLARE_PUBLIC(ValueFilter)

public:
    ValueFilterPrivate() {}
    ~ValueFilterPrivate() Q_DECL_EQ_DEFAULT;

    QVariant m_value;

protected:
    bool filterRow(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;
};

/*************************************************************/

class IndexFilterPrivate: public FilterPrivate
{
    Q_DECLARE_PUBLIC(IndexFilter)

public:
    IndexFilterPrivate() {}
    ~IndexFilterPrivate() Q_DECL_EQ_DEFAULT;

    QVariant m_minimumIndex;
    QVariant m_maximumIndex;

protected:
    bool filterRow(const QModelIndex& sourceIndex) const Q_DECL_OVERRIDE;
};

/*************************************************************/

class RegExpFilterPrivate: public RoleFilterPrivate
{
    Q_DECLARE_PUBLIC(RegExpFilter)

public:
    RegExpFilterPrivate() {}
    ~RegExpFilterPrivate() Q_DECL_EQ_DEFAULT;

    QRegExp m_regExp;
    Qt::CaseSensitivity m_caseSensitivity = m_regExp.caseSensitivity();
    QQmlSortFilterProxyModel::PatternSyntax m_syntax = static_cast<QQmlSortFilterProxyModel::PatternSyntax>(m_regExp.patternSyntax());
    QString m_pattern = m_regExp.pattern();

protected:
    bool filterRow(const QModelIndex& sourceIndex) const Q_DECL_OVERRIDE;
};

/*************************************************************/

class RangeFilterPrivate: public RoleFilterPrivate
{
    Q_DECLARE_PUBLIC(RangeFilter)

public:
    RangeFilterPrivate() {}
    ~RangeFilterPrivate() Q_DECL_EQ_DEFAULT;

    QVariant m_minimumValue;
    bool m_minimumInclusive = true;
    QVariant m_maximumValue;
    bool m_maximumInclusive = true;

protected:
    bool filterRow(const QModelIndex& sourceIndex) const Q_DECL_OVERRIDE;
};

/*************************************************************/

class ExpressionFilterPrivate: public FilterPrivate
{
    Q_DECLARE_PUBLIC(ExpressionFilter)

public:
    ExpressionFilterPrivate() {}
    ~ExpressionFilterPrivate() Q_DECL_EQ_DEFAULT;

    void proxyModelCompleted() Q_DECL_OVERRIDE;

    QQmlScriptString m_scriptString;
    QQmlExpression* m_expression = Q_NULLPTR;
    QQmlContext* m_context = Q_NULLPTR;

protected:
    bool filterRow(const QModelIndex& sourceIndex) const Q_DECL_OVERRIDE;

    void updateContext();
    void updateExpression();
};

/*************************************************************/

class FilterContainerPrivate: public FilterPrivate
{
    Q_DECLARE_PUBLIC(FilterContainer)

public:
    FilterContainerPrivate() {}
    ~FilterContainerPrivate() Q_DECL_EQ_DEFAULT;

    void proxyModelCompleted() override;

    QList<Filter*> m_filters;    
};

/*************************************************************/

class AnyOfFilterPrivate: public FilterContainerPrivate
{
    Q_DECLARE_PUBLIC(AnyOfFilter)

public:
    AnyOfFilterPrivate() {}
    ~AnyOfFilterPrivate() Q_DECL_EQ_DEFAULT;

protected:
    bool filterRow(const QModelIndex& sourceIndex) const Q_DECL_OVERRIDE;
};

/*************************************************************/

class AllOfFilterPrivate: public FilterContainerPrivate
{
    Q_DECLARE_PUBLIC(AllOfFilter)

public:
    AllOfFilterPrivate() {}
    ~AllOfFilterPrivate() Q_DECL_EQ_DEFAULT;

protected:
    bool filterRow(const QModelIndex& sourceIndex) const Q_DECL_OVERRIDE;
};

}
