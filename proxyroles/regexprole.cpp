#include "regexprole.h"
#include "qqmlsortfilterproxymodel.h"
#include <QDebug>

namespace qqsfpm {

QString RegExpRole::roleName() const
{
    return m_roleName;
}

void RegExpRole::setRoleName(const QString& roleName)
{
    if (m_roleName == roleName)
        return;

    m_roleName = roleName;
    Q_EMIT roleNameChanged();
}

QString RegExpRole::pattern() const
{
    return m_regularExpression.pattern();
}

void RegExpRole::setPattern(const QString& pattern)
{
    if (m_regularExpression.pattern() == pattern)
        return;

    Q_EMIT namesAboutToBeChanged();
    m_regularExpression.setPattern(pattern);
    invalidate();
    Q_EMIT patternChanged();
    Q_EMIT namesChanged();
}

Qt::CaseSensitivity RegExpRole::caseSensitivity() const
{
    return m_regularExpression.patternOptions() & QRegularExpression::CaseInsensitiveOption ?
                Qt::CaseInsensitive : Qt::CaseSensitive;
}

void RegExpRole::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
    if (this->caseSensitivity() == caseSensitivity)
        return;

    m_regularExpression.setPatternOptions(m_regularExpression.patternOptions() ^ QRegularExpression::CaseInsensitiveOption); //toggle the option
    Q_EMIT caseSensitivityChanged();
}

QStringList RegExpRole::names()
{
    QStringList nameCaptureGroups = m_regularExpression.namedCaptureGroups();
    nameCaptureGroups.removeAll("");
    return nameCaptureGroups;
}

QVariant RegExpRole::data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel, const QString &name)
{
    QString text = proxyModel.sourceData(sourceIndex, m_roleName).toString();
    QRegularExpressionMatch match = m_regularExpression.match(text);
    return match.hasMatch() ? (match.captured(name)) : QVariant{};
}

}
