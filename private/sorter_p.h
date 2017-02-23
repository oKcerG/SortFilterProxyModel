#pragma once
#include "sorter.h"
#include <QString>
#include <QVariant>

namespace qqsfpm {

class SorterPrivate
{
    Q_DECLARE_PUBLIC(Sorter)

public:
    SorterPrivate() {}
    virtual ~SorterPrivate() Q_DECL_EQ_DEFAULT;

    virtual void proxyModelCompleted();

    bool m_enabled = true;
    bool m_ascendingOrder = true;
    QQmlSortFilterProxyModel* m_proxyModel = nullptr;

protected:
    Sorter *q_ptr = Q_NULLPTR;

    virtual int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const;
    virtual bool lessThan(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const;
};

/*************************************************************/

class RoleSorterPrivate: public SorterPrivate
{
    Q_DECLARE_PUBLIC(RoleSorter)

public:
    RoleSorterPrivate() {}
    ~RoleSorterPrivate() Q_DECL_EQ_DEFAULT;

    QString m_roleName;

protected:
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const override;
};

/*************************************************************/

class ExpressionSorterPrivate: public SorterPrivate
{
    Q_DECLARE_PUBLIC(ExpressionSorter)

public:
    ExpressionSorterPrivate() {}
    ~ExpressionSorterPrivate() Q_DECL_EQ_DEFAULT;

    QQmlScriptString m_scriptString;
    QQmlExpression* m_expression = nullptr;
    QQmlContext* m_context = nullptr;

protected:
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const override;
    void proxyModelCompleted() override;

private:
    bool evaluateBoolExpression(QQmlExpression& expression) const;
    void updateContext();
    void updateExpression();
};

/*************************************************************/

class IndexSorterPrivate: public SorterPrivate
{
    Q_DECLARE_PUBLIC(IndexSorter)

public:
    IndexSorterPrivate() {}
    ~IndexSorterPrivate() Q_DECL_EQ_DEFAULT;

protected:
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const override;
};

/*************************************************************/

class ReverseIndexSorterPrivate: public SorterPrivate
{
    Q_DECLARE_PUBLIC(ReverseIndexSorter)

public:
    ReverseIndexSorterPrivate() {}
    ~ReverseIndexSorterPrivate() Q_DECL_EQ_DEFAULT;

protected:
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const override;
};

}
