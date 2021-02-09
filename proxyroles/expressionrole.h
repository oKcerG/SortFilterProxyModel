#ifndef EXPRESSIONROLE_H
#define EXPRESSIONROLE_H

#include "singlerole.h"
#include <QMap>
#include <QPersistentModelIndex>
#include <QQmlScriptString>

class QQmlExpression;

namespace qqsfpm {

class ExpressionRole : public SingleRole
{
    Q_OBJECT
    Q_PROPERTY(QQmlScriptString expression READ expression WRITE setExpression NOTIFY expressionChanged)
    Q_PROPERTY(bool cached READ isCached WRITE setCached NOTIFY cachedChanged)

public:
    using SingleRole::SingleRole;

    const QQmlScriptString& expression() const;
    void setExpression(const QQmlScriptString& scriptString);
    bool isCached() const;

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;

public slots:
    void setCached(bool cached);
    void invalidateCache();

Q_SIGNALS:
    void expressionChanged();
    void cachedChanged(bool cached);

private:
    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;
    void updateContext(const QQmlSortFilterProxyModel& proxyModel);
    void updateExpression();

    QQmlScriptString m_scriptString;
    QQmlExpression* m_expression = nullptr;
    QQmlContext* m_context = nullptr;

    QMap<QPersistentModelIndex, QVariant> m_cache;
    bool m_cached = false;
};

}

#endif // EXPRESSIONROLE_H
