#ifndef REGEXPFILTER_H
#define REGEXPFILTER_H

#include "rolefilter.h"
#include <QRegularExpression>

namespace qqsfpm {

class RegExpFilter : public RoleFilter {
    Q_OBJECT
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
    Q_PROPERTY(PatternSyntax syntax READ syntax WRITE setSyntax NOTIFY syntaxChanged)
    Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE setCaseSensitivity NOTIFY caseSensitivityChanged)

public:
    enum PatternSyntax {
        RegExp,// = QRegularExpression::RegExp,
        Wildcard,// = QRegularExpression::Wildcard,
        FixedString,// = QRegularExpression::FixedString,
        RegExp2,// = QRegularExpression::RegExp2,
        WildcardUnix,// = QRegularExpression::WildcardUnix,
        W3CXmlSchema11,/* = QRegularExpression::W3CXmlSchema11*/ };
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
    QRegularExpression m_regExp;
    Qt::CaseSensitivity m_caseSensitivity = m_regExp.caseSensitivity();
    PatternSyntax m_syntax = static_cast<PatternSyntax>(m_regExp.patternSyntax());
    QString m_pattern = m_regExp.pattern();
};

}

#endif // REGEXPFILTER_H
