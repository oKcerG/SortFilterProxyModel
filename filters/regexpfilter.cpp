#include "regexpfilter.h"
#include <QVariant>

namespace qqsfpm {

/*!
    \qmltype RegExpFilter
    \inherits RoleFilter
    \inqmlmodule SortFilterProxyModel
    \ingroup Filters
    \brief  Filters rows matching a regular expression.

    A RegExpFilter is a \l RoleFilter that accepts rows matching a regular
   rexpression.

    In the following example, only rows with their \c lastName role beggining
   with the content of textfield the will be accepted: \code TextField { id:
   nameTextField
    }

    SortFilterProxyModel {
       sourceModel: contactModel
       filters: RegExpFilter {
           roleName: "lastName"
           pattern: "^" + nameTextField.displayText
       }
    }
    \endcode
*/

/*!
    \qmlproperty bool RegExpFilter::pattern

    The pattern used to filter the contents of the source model.

    \sa syntax
*/
QString RegExpFilter::pattern() const { return m_pattern; }

void RegExpFilter::setPattern(const QString &pattern) {
  if (m_pattern == pattern)
    return;

  m_pattern = pattern;
  m_regularExpression.setPattern(pattern);
  Q_EMIT patternChanged();
  invalidate();
}

/*!
    \qmlproperty Qt::CaseSensitivity RegExpFilter::caseSensitivity

    This property holds the caseSensitivity of the filter.
*/
Qt::CaseSensitivity RegExpFilter::caseSensitivity() const {
  return m_regularExpression.patternOptions() &
                 QRegularExpression::CaseInsensitiveOption
             ? Qt::CaseInsensitive
             : Qt::CaseSensitive;
}

void RegExpFilter::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity) {
  if (this->caseSensitivity() == caseSensitivity)
    return;

  m_regularExpression.setPatternOptions(
      m_regularExpression.patternOptions() ^
      QRegularExpression::CaseInsensitiveOption); // toggle the option
  Q_EMIT caseSensitivityChanged();
  invalidate();
}

bool RegExpFilter::filterRow(const QModelIndex &sourceIndex,
                             const QQmlSortFilterProxyModel &proxyModel) const {
  QString string = sourceData(sourceIndex, proxyModel).toString();
  QRegularExpressionMatch match = m_regularExpression.match(string);
  return match.hasMatch();
}

} // namespace qqsfpm
