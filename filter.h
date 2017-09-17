#ifndef FILTER_H
#define FILTER_H

#include <QObject>
#include <QQmlExpression>
#include "qqmlsortfilterproxymodel.h"

namespace qqsfpm {

class Filter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool inverted READ inverted WRITE setInverted NOTIFY invertedChanged)

public:
    explicit Filter(QObject *parent = nullptr);
    virtual ~Filter() = default;

    bool enabled() const;
    void setEnabled(bool enabled);

    bool inverted() const;
    void setInverted(bool inverted);

    bool filterAcceptsRow(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const;

    virtual void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel);

Q_SIGNALS:
    void enabledChanged();
    void invertedChanged();
    void invalidated();

protected:
    virtual bool filterRow(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const = 0;
    void invalidate();

private:
    bool m_enabled = true;
    bool m_inverted = false;
};

class RoleFilter : public Filter
{
    Q_OBJECT
    Q_PROPERTY(QString roleName READ roleName WRITE setRoleName NOTIFY roleNameChanged)

public:
    using Filter::Filter;

    const QString& roleName() const;
    void setRoleName(const QString& roleName);

Q_SIGNALS:
    void roleNameChanged();

protected:
    QVariant sourceData(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const;

private:
    QString m_roleName;
};

class ValueFilter : public RoleFilter {
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    using RoleFilter::RoleFilter;

    const QVariant& value() const;
    void setValue(const QVariant& value);

protected:
    bool filterRow(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;

Q_SIGNALS:
    void valueChanged();

private:
    QVariant m_value;
};

class IndexFilter: public Filter {
    Q_OBJECT
    Q_PROPERTY(QVariant minimumIndex READ minimumIndex WRITE setMinimumIndex NOTIFY minimumIndexChanged)
    Q_PROPERTY(QVariant maximumIndex READ maximumIndex WRITE setMaximumIndex NOTIFY maximumIndexChanged)

public:
    using Filter::Filter;

    const QVariant& minimumIndex() const;
    void setMinimumIndex(const QVariant& minimumIndex);

    const QVariant& maximumIndex() const;
    void setMaximumIndex(const QVariant& maximumIndex);

protected:
    bool filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;

Q_SIGNALS:
    void minimumIndexChanged();
    void maximumIndexChanged();

private:
    QVariant m_minimumIndex;
    QVariant m_maximumIndex;
};

class RegExpFilter : public RoleFilter {
    Q_OBJECT
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
    Q_PROPERTY(PatternSyntax syntax READ syntax WRITE setSyntax NOTIFY syntaxChanged)
    Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE setCaseSensitivity NOTIFY caseSensitivityChanged)

public:
    enum PatternSyntax {
        RegExp = QRegExp::RegExp,
        Wildcard = QRegExp::Wildcard,
        FixedString = QRegExp::FixedString,
        RegExp2 = QRegExp::RegExp2,
        WildcardUnix = QRegExp::WildcardUnix,
        W3CXmlSchema11 = QRegExp::W3CXmlSchema11 };
    Q_ENUMS(PatternSyntax)

    using RoleFilter::RoleFilter;

    QString pattern() const;
    void setPattern(const QString& pattern);

    PatternSyntax syntax() const;
    void setSyntax(PatternSyntax syntax);

    Qt::CaseSensitivity caseSensitivity() const;
    void setCaseSensitivity(Qt::CaseSensitivity caseSensitivity);

protected:
    bool filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;

Q_SIGNALS:
    void patternChanged();
    void syntaxChanged();
    void caseSensitivityChanged();

private:
    QRegExp m_regExp;
    Qt::CaseSensitivity m_caseSensitivity = m_regExp.caseSensitivity();
    PatternSyntax m_syntax = static_cast<PatternSyntax>(m_regExp.patternSyntax());
    QString m_pattern = m_regExp.pattern();
};

class RangeFilter : public RoleFilter
{
    Q_OBJECT
    Q_PROPERTY(QVariant minimumValue READ minimumValue WRITE setMinimumValue NOTIFY minimumValueChanged)
    Q_PROPERTY(bool minimumInclusive READ minimumInclusive WRITE setMinimumInclusive NOTIFY minimumInclusiveChanged)
    Q_PROPERTY(QVariant maximumValue READ maximumValue WRITE setMaximumValue NOTIFY maximumValueChanged)
    Q_PROPERTY(bool maximumInclusive READ maximumInclusive WRITE setMaximumInclusive NOTIFY maximumInclusiveChanged)

public:
    using RoleFilter::RoleFilter;

    QVariant minimumValue() const;
    void setMinimumValue(QVariant minimumValue);
    bool minimumInclusive() const;
    void setMinimumInclusive(bool minimumInclusive);

    QVariant maximumValue() const;
    void setMaximumValue(QVariant maximumValue);
    bool maximumInclusive() const;
    void setMaximumInclusive(bool maximumInclusive);

protected:
    bool filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;

Q_SIGNALS:
    void minimumValueChanged();
    void minimumInclusiveChanged();
    void maximumValueChanged();
    void maximumInclusiveChanged();

private:
    QVariant m_minimumValue;
    bool m_minimumInclusive = true;
    QVariant m_maximumValue;
    bool m_maximumInclusive = true;
};

class ExpressionFilter : public Filter
{
    Q_OBJECT
    Q_PROPERTY(QQmlScriptString expression READ expression WRITE setExpression NOTIFY expressionChanged)

public:
    using Filter::Filter;

    const QQmlScriptString& expression() const;
    void setExpression(const QQmlScriptString& scriptString);

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;

protected:
    bool filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;

Q_SIGNALS:
    void expressionChanged();

private:
    void updateContext(const QQmlSortFilterProxyModel& proxyModel);
    void updateExpression();

    QQmlScriptString m_scriptString;
    QQmlExpression* m_expression = nullptr;
    QQmlContext* m_context = nullptr;
};

class FilterContainer : public Filter {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<qqsfpm::Filter> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    using Filter::Filter;

    QQmlListProperty<Filter> filters();

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;

protected:
    QList<Filter*> m_filters;

private:
    static void append_filter(QQmlListProperty<Filter>* list, Filter* filter);
    static int count_filter(QQmlListProperty<Filter>* list);
    static Filter* at_filter(QQmlListProperty<Filter>* list, int index);
    static void clear_filters(QQmlListProperty<Filter>* list);
};

class AnyOfFilter : public FilterContainer {
    Q_OBJECT

public:
    using FilterContainer::FilterContainer;

protected:
    bool filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;
};

class AllOfFilter : public FilterContainer {
    Q_OBJECT

public:
    using FilterContainer::FilterContainer;

protected:
    bool filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;
};

}

#endif // FILTER_H
