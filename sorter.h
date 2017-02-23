#pragma once

#include <QObject>
#include <QQmlExpression>
#include "qqmlsortfilterproxymodel.h"

namespace qqsfpm
{

class SorterPrivate;
class Sorter: public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Sorter)
    Q_DISABLE_COPY(Sorter)
    Sorter() Q_DECL_EQ_DELETE;

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool ascendingOrder READ ascendingOrder WRITE setAscendingOrder NOTIFY ascendingOrderChanged)
    friend class QQmlSortFilterProxyModel;

public:
    virtual ~Sorter() Q_DECL_EQ_DEFAULT;

    bool enabled() const;
    void setEnabled(bool enabled);

    bool ascendingOrder() const;
    void setAscendingOrder(bool ascendingOrder);

    QQmlSortFilterProxyModel *proxyModel() const;
    void setProxyModel(QQmlSortFilterProxyModel *model, bool proxyModelCompleted);

    int compareRows(const QModelIndex &source_left, const QModelIndex &source_right) const;

Q_SIGNALS:
    void enabledChanged();
    void ascendingOrderChanged();

    void sorterChanged();
    void invalidate();

protected:
    Sorter(SorterPrivate &dd, QObject *parent = Q_NULLPTR);
    const QScopedPointer<SorterPrivate> d_ptr;
};

class RoleSorterPrivate;
class RoleSorter: public Sorter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RoleSorter)
    Q_DISABLE_COPY(RoleSorter)

    Q_PROPERTY(QString roleName READ roleName WRITE setRoleName NOTIFY roleNameChanged)

public:
    explicit RoleSorter(QObject *parent = Q_NULLPTR);
    ~RoleSorter() Q_DECL_EQ_DEFAULT;

    const QString &roleName() const;
    void setRoleName(const QString &roleName);

Q_SIGNALS:
    void roleNameChanged();
};

class ExpressionSorterPrivate;
class ExpressionSorter: public Sorter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ExpressionSorter)
    Q_DISABLE_COPY(ExpressionSorter)

    Q_PROPERTY(QQmlScriptString expression READ expression WRITE setExpression NOTIFY expressionChanged)

public:
    explicit ExpressionSorter(QObject *parent = Q_NULLPTR);
    ~ExpressionSorter() Q_DECL_EQ_DEFAULT;

    const QQmlScriptString &expression() const;
    void setExpression(const QQmlScriptString &scriptString);

Q_SIGNALS:
    void expressionChanged();
};

class IndexSorterPrivate;
class IndexSorter: public Sorter
{
public:
    explicit IndexSorter(QObject *parent = Q_NULLPTR);
    ~IndexSorter() Q_DECL_EQ_DEFAULT;
};

class ReverseIndexSorterPrivate;
class ReverseIndexSorter: public Sorter
{
public:
    explicit ReverseIndexSorter(QObject *parent = Q_NULLPTR);
    ~ReverseIndexSorter() Q_DECL_EQ_DEFAULT;
};

}
