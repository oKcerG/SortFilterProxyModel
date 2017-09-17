#ifndef SORTER_H
#define SORTER_H

#include <QObject>
#include <QQmlExpression>
#include "qqmlsortfilterproxymodel.h"
#include "QCollator"

namespace qqsfpm {

class Sorter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool ascendingOrder READ ascendingOrder WRITE setAscendingOrder NOTIFY sortOrderChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)

public:
    Sorter(QObject* parent = nullptr);
    virtual ~Sorter() = 0;

    bool enabled() const;
    void setEnabled(bool enabled);

    bool ascendingOrder() const;
    void setAscendingOrder(bool ascendingOrder);

    Qt::SortOrder sortOrder() const;
    void setSortOrder(Qt::SortOrder sortOrder);

    int compareRows(const QModelIndex& source_left, const QModelIndex& source_right, const QQmlSortFilterProxyModel& proxyModel) const;

    virtual void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel);

Q_SIGNALS:
    void enabledChanged();
    void sortOrderChanged();

    void invalidated();

protected:
    virtual int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const;
    virtual bool lessThan(const QModelIndex& sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const;
    void invalidate();

private:
    bool m_enabled = true;
    Qt::SortOrder m_sortOrder = Qt::AscendingOrder;
};

class RoleSorter : public Sorter
{
    Q_OBJECT
    Q_PROPERTY(QString roleName READ roleName WRITE setRoleName NOTIFY roleNameChanged)

public:
    using Sorter::Sorter;

    const QString& roleName() const;
    void setRoleName(const QString& roleName);

Q_SIGNALS:
    void roleNameChanged();

protected:
    QPair<QVariant, QVariant> sourceData(const QModelIndex &sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const;
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const override;

private:
    QString m_roleName;
};

class StringSorter : public RoleSorter
{
    Q_OBJECT
    Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE setCaseSensitivity NOTIFY caseSensitivityChanged)
    Q_PROPERTY(bool ignorePunctation READ ignorePunctation WRITE setIgnorePunctation NOTIFY ignorePunctationChanged)
    Q_PROPERTY(QLocale locale READ locale WRITE setLocale NOTIFY localeChanged)
    Q_PROPERTY(bool numericMode READ numericMode WRITE setNumericMode NOTIFY numericModeChanged)

public:
    using RoleSorter::RoleSorter;

    Qt::CaseSensitivity caseSensitivity() const;
    void setCaseSensitivity(Qt::CaseSensitivity caseSensitivity);

    bool ignorePunctation() const;
    void setIgnorePunctation(bool ignorePunctation);

    QLocale locale() const;
    void setLocale(const QLocale& locale);

    bool numericMode() const;
    void setNumericMode(bool numericMode);

Q_SIGNALS:
    void caseSensitivityChanged();
    void ignorePunctationChanged();
    void localeChanged();
    void numericModeChanged();

protected:
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const override;

private:
    QCollator m_collator;
};

class ExpressionSorter : public Sorter
{
    Q_OBJECT
    Q_PROPERTY(QQmlScriptString expression READ expression WRITE setExpression NOTIFY expressionChanged)

public:
    using Sorter::Sorter;

    const QQmlScriptString& expression() const;
    void setExpression(const QQmlScriptString& scriptString);

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;

Q_SIGNALS:
    void expressionChanged();

protected:
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const override;

private:
    void updateContext(const QQmlSortFilterProxyModel& proxyModel);
    void updateExpression();

    QQmlScriptString m_scriptString;
    QQmlExpression* m_expression = nullptr;
    QQmlContext* m_context = nullptr;
};

}

#endif // SORTER_H
