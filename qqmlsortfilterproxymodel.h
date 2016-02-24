#ifndef QQMLSORTFILTERPROXYMODEL_H
#define QQMLSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QQmlExpression>

class QQmlSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterRoleName READ filterRoleName WRITE setFilterRoleName NOTIFY filterRoleNameChanged)
    Q_PROPERTY(QString filterPattern READ filterPattern WRITE setFilterPattern NOTIFY filterPatternChanged)
    Q_PROPERTY(PatternSyntax filterPatternSyntax READ filterPatternSyntax WRITE setFilterPatternSyntax NOTIFY filterPatternSyntaxChanged)
    Q_PROPERTY(QVariant filterValue READ filterValue WRITE setFilterValue NOTIFY filterValueChanged)
    Q_PROPERTY(QQmlScriptString filterExpression READ filterExpression WRITE setFilterExpression NOTIFY filterExpressionChanged)

    Q_PROPERTY(QString sortRoleName READ sortRoleName WRITE setSortRoleName NOTIFY sortRoleNameChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder)
    Q_PROPERTY(QQmlScriptString sortExpression READ sortExpression WRITE setSortExpression NOTIFY sortExpressionChanged)

public:
    enum PatternSyntax {
        RegExp = QRegExp::RegExp,
        Wildcard = QRegExp::Wildcard,
        FixedString = QRegExp::FixedString,
        RegExp2 = QRegExp::RegExp2,
        WildcardUnix = QRegExp::WildcardUnix,
        W3CXmlSchema11 = QRegExp::W3CXmlSchema11 };
    Q_ENUMS(PatternSyntax)

    QQmlSortFilterProxyModel(QObject* parent = 0);

    QString filterRoleName() const;
    QString filterPattern() const;
    PatternSyntax filterPatternSyntax() const;
    QVariant filterValue() const;
    QQmlScriptString filterExpression() const;

    QString sortRoleName() const;
    QQmlScriptString sortExpression() const;

public slots:
    void setFilterRoleName(QString filterRoleName);
    void setFilterPattern(QString filterPattern);
    void setFilterPatternSyntax(PatternSyntax patternSyntax);
    void setFilterValue(QVariant filterValue);
    void setFilterExpression(QQmlScriptString filterScriptString);

    void setSortRoleName(QString sortRoleName);
    void setSortOrder(Qt::SortOrder sortOrder);
    void setSortExpression(QQmlScriptString compareScriptString);

signals:
    void filterRoleNameChanged();
    void filterPatternSyntaxChanged();
    void filterPatternChanged();
    void filterValueChanged();
    void filterExpressionChanged();

    void sortRoleNameChanged();
    void sortExpressionChanged();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const;

private slots:
    void invalidateFilter();
    void updateRoles();

private:
    QVariantMap modelDataMap(const QModelIndex& modelIndex) const;

    QString m_filterRoleName;
    QString m_sortRoleName;

    QQmlScriptString m_filterScriptString;
    QQmlExpression* m_filterExpression;

    QQmlScriptString m_compareScriptString;
    QQmlExpression* m_compareExpression;
    QVariant m_filterValue;
};

#endif // QQMLSORTFILTERPROXYMODEL_H

