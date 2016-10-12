#include "qqmlsortfilterproxymodel.h"
#include <QtQml>

QQmlSortFilterProxyModel::QQmlSortFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    m_filterExpression(0),
    m_compareExpression(0)
{
    connect(this, &QAbstractProxyModel::sourceModelChanged, this, &QQmlSortFilterProxyModel::updateRoles);
    connect(this, &QAbstractItemModel::modelReset, this, &QQmlSortFilterProxyModel::updateRoles);
    connect(this, &QAbstractItemModel::rowsInserted, this, &QQmlSortFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &QQmlSortFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::modelReset, this, &QQmlSortFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::layoutChanged, this, &QQmlSortFilterProxyModel::countChanged);
    setDynamicSortFilter(true);
}

int QQmlSortFilterProxyModel::count() const
{
    return rowCount();
}

const QString& QQmlSortFilterProxyModel::filterRoleName() const
{
    return m_filterRoleName;
}

void QQmlSortFilterProxyModel::setFilterRoleName(const QString& filterRoleName)
{
    if (m_filterRoleName == filterRoleName)
        return;

    m_filterRoleName = filterRoleName;
    updateFilterRole();
    emit filterRoleNameChanged();
}

QString QQmlSortFilterProxyModel::filterPattern() const
{
    return filterRegExp().pattern();
}

void QQmlSortFilterProxyModel::setFilterPattern(const QString& filterPattern)
{
    QRegExp regExp = filterRegExp();
    if (regExp.pattern() == filterPattern)
        return;

    regExp.setPattern(filterPattern);
    QSortFilterProxyModel::setFilterRegExp(regExp);
    emit filterPatternChanged();
}

QQmlSortFilterProxyModel::PatternSyntax QQmlSortFilterProxyModel::filterPatternSyntax() const
{
    return static_cast<PatternSyntax>(filterRegExp().patternSyntax());
}

void QQmlSortFilterProxyModel::setFilterPatternSyntax(QQmlSortFilterProxyModel::PatternSyntax patternSyntax)
{
    QRegExp regExp = filterRegExp();
    QRegExp::PatternSyntax patternSyntaxTmp = static_cast<QRegExp::PatternSyntax>(patternSyntax);
    if (regExp.patternSyntax() == patternSyntaxTmp)
        return;

    regExp.setPatternSyntax(patternSyntaxTmp);
    QSortFilterProxyModel::setFilterRegExp(regExp);
    emit filterPatternSyntaxChanged();
}

const QVariant& QQmlSortFilterProxyModel::filterValue() const
{
    return m_filterValue;
}

void QQmlSortFilterProxyModel::setFilterValue(const QVariant& filterValue)
{
    if (m_filterValue == filterValue)
        return;

    m_filterValue = filterValue;
    invalidateFilter();
    emit filterValueChanged();
}

const QQmlScriptString& QQmlSortFilterProxyModel::filterExpression() const
{
    return m_filterScriptString;
}

void QQmlSortFilterProxyModel::setFilterExpression(const QQmlScriptString& filterScriptString)
{
    if (m_filterScriptString == filterScriptString)
        return;

    m_filterScriptString = filterScriptString;
    QQmlContext* context = new QQmlContext(qmlContext(this));

    QVariantMap map;
    Q_FOREACH (const QByteArray& roleName, roleNames().values())
        map.insert(roleName, QVariant());

    context->setContextProperty("model", map);
    context->setContextProperty("index", -1);

    delete(m_filterExpression);
    m_filterExpression = new QQmlExpression(m_filterScriptString, context, 0, this);
    connect(m_filterExpression, &QQmlExpression::valueChanged, this, &QQmlSortFilterProxyModel::invalidateFilter);
    m_filterExpression->setNotifyOnValueChanged(true);
    m_filterExpression->evaluate();

    emit filterExpressionChanged();
}

const QString& QQmlSortFilterProxyModel::sortRoleName() const
{
    return m_sortRoleName;
}

void QQmlSortFilterProxyModel::setSortRoleName(const QString& sortRoleName)
{
    if (m_sortRoleName == sortRoleName)
        return;

    m_sortRoleName = sortRoleName;
    updateSortRole();
    emit sortRoleNameChanged();
}

void QQmlSortFilterProxyModel::setSortOrder(Qt::SortOrder sortOrder)
{
    sort(0, sortOrder);
}

const QQmlScriptString& QQmlSortFilterProxyModel::sortExpression() const
{
    return m_compareScriptString;
}

void QQmlSortFilterProxyModel::setSortExpression(const QQmlScriptString& compareScriptString)
{
    if (m_compareScriptString == compareScriptString)
        return;

    m_compareScriptString = compareScriptString;
    QQmlContext* context = new QQmlContext(qmlContext(this));

    QVariantMap map;
    Q_FOREACH (const QByteArray& roleName, roleNames().values())
        map.insert(roleName, QVariant());

    context->setContextProperty("modelLeft", map);
    context->setContextProperty("indexLeft", -1);
    context->setContextProperty("modelRight", map);
    context->setContextProperty("indexRight", -1);

    delete(m_compareExpression);
    m_compareExpression = new QQmlExpression(m_compareScriptString, context, 0, this);
    connect(m_compareExpression, &QQmlExpression::valueChanged, this, &QQmlSortFilterProxyModel::invalidate);
    m_compareExpression->setNotifyOnValueChanged(true);
    m_compareExpression->evaluate();

    emit sortExpressionChanged();
}

bool QQmlSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    QModelIndex modelIndex = sourceModel()->index(source_row, 0, source_parent);
    bool valueAccepted = !m_filterValue.isValid() || ( m_filterValue == sourceModel()->data(modelIndex, filterRole()) );
    bool baseAcceptsRow = valueAccepted && QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    if (baseAcceptsRow && !m_filterScriptString.isEmpty())
    {
        QVariantMap map = modelDataMap(modelIndex);

        QQmlContext context(qmlContext(this));
        context.setContextProperty("model", map);
        context.setContextProperty("index", source_row);
        QQmlExpression expression(m_filterScriptString, &context, 0);
        QVariant result = expression.evaluate();

        if (!expression.hasError())
            return result.toBool();
        else
            qWarning() << expression.error();
    }
    return baseAcceptsRow;
}

bool QQmlSortFilterProxyModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
    if (!m_compareScriptString.isEmpty())
    {
        QQmlContext context(qmlContext(this));
        context.setContextProperty("modelLeft", modelDataMap(source_left));
        context.setContextProperty("indexLeft", source_left.row());
        context.setContextProperty("modelRight", modelDataMap(source_right));
        context.setContextProperty("indexRight", source_right.row());

        QQmlExpression expression(m_compareScriptString, &context, 0);
        QVariant result = expression.evaluate();

        if (!expression.hasError())
            return result.toBool();
        else
            qWarning() << expression.error();
    }
    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

void QQmlSortFilterProxyModel::invalidateFilter()
{
    QSortFilterProxyModel::invalidateFilter();
}

void QQmlSortFilterProxyModel::updateFilterRole()
{
    QList<int> filterRoles = roleNames().keys(m_filterRoleName.toUtf8());
    if (!filterRoles.empty())
    {
        setFilterRole(filterRoles.first());
    }
}

void QQmlSortFilterProxyModel::updateSortRole()
{
    QList<int> sortRoles = roleNames().keys(m_sortRoleName.toUtf8());
    if (!sortRoles.empty())
    {
        setSortRole(sortRoles.first());
        sort(0, sortOrder());
    }
}

void QQmlSortFilterProxyModel::updateRoles()
{
    updateFilterRole();
    updateSortRole();
}

QVariantMap QQmlSortFilterProxyModel::modelDataMap(const QModelIndex& modelIndex) const
{
    QVariantMap map;
    QHash<int, QByteArray> roles = roleNames();
    for (QHash<int, QByteArray>::const_iterator it = roles.begin(); it != roles.end(); ++it)
        map.insert(it.value(), sourceModel()->data(modelIndex, it.key()));
    return map;
}

void registerQQmlSortFilterProxyModelTypes() {
    qmlRegisterType<QAbstractItemModel>();
    qmlRegisterType<QQmlSortFilterProxyModel>("SortFilterProxyModel", 0, 1, "SortFilterProxyModel");
}

Q_COREAPP_STARTUP_FUNCTION(registerQQmlSortFilterProxyModelTypes)
