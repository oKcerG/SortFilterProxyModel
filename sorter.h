#ifndef SORTER_H
#define SORTER_H

#include <QObject>
#include <QCollator>
#include <QQmlExpression>
#include "qqmlsortfilterproxymodel.h"
#include "qqmlsortfilterproxymodelexport.h"

namespace qqsfpm {

class QQMLSORTFILTERPROXYMODEL_EXPORT Sorter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool ascendingOrder READ ascendingOrder WRITE setAscendingOrder NOTIFY ascendingOrderChanged)
    friend class QQmlSortFilterProxyModel;

public:
    Sorter(QObject* parent = nullptr);
    virtual ~Sorter() = 0;

    bool enabled() const;
    void setEnabled(bool enabled);

    bool ascendingOrder() const;
    void setAscendingOrder(bool ascendingOrder);

    int compareRows(const QModelIndex& source_left, const QModelIndex& source_right) const;

Q_SIGNALS:
    void enabledChanged();
    void ascendingOrderChanged();

    void invalidate();

protected:
    QQmlSortFilterProxyModel* proxyModel() const;

    virtual int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const;
    virtual bool lessThan(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const;
    virtual void proxyModelCompleted();
    void sorterChanged();

private:
    bool m_enabled = true;
    bool m_ascendingOrder = true;
    QQmlSortFilterProxyModel* m_proxyModel = nullptr;
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
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const override;

private:
    QString m_roleName;
};

class NaturalSorter: public RoleSorter
{
    Q_OBJECT
    Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE setCaseSensitivity NOTIFY caseSensitivityChanged)
    Q_PROPERTY(bool numericMode READ numericMode WRITE setNumericMode NOTIFY numericModeChanged)
    Q_PROPERTY(bool ignorePunctuation READ ignorePunctuation WRITE setIgnorePunctuation NOTIFY ignorePunctuationChanged)

public:
    Qt::CaseSensitivity caseSensitivity() const;
    void setCaseSensitivity(Qt::CaseSensitivity value);

    bool numericMode() const;
    void setNumericMode(bool value);

    bool ignorePunctuation() const;
    void setIgnorePunctuation(bool value);

Q_SIGNALS:
    void caseSensitivityChanged();
    void numericModeChanged();
    void ignorePunctuationChanged();

protected:
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const override;

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

Q_SIGNALS:
    void expressionChanged();

protected:
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const override;
    void proxyModelCompleted() override;

private:
    void updateContext();
    void updateExpression();

    QQmlScriptString m_scriptString;
    QQmlExpression* m_expression = nullptr;
    QQmlContext* m_context = nullptr;
};

}

#endif // SORTER_H
