#ifndef REGEXPFILTER_H
#define REGEXPFILTER_H

#include "rolefilter.h"
#include <QRegularExpression>

namespace qqsfpm {

class RegExpFilter : public RoleFilter {
  Q_OBJECT
  Q_PROPERTY(
      QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
  Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE
                 setCaseSensitivity NOTIFY caseSensitivityChanged)

public:
  using RoleFilter::RoleFilter;

  QString pattern() const;
  void setPattern(const QString &pattern);

  Qt::CaseSensitivity caseSensitivity() const;
  void setCaseSensitivity(Qt::CaseSensitivity caseSensitivity);

protected:
  bool filterRow(const QModelIndex &sourceIndex,
                 const QQmlSortFilterProxyModel &proxyModel) const override;

Q_SIGNALS:
  void patternChanged();
  void caseSensitivityChanged();

private:
  QRegularExpression m_regularExpression;
  QString m_pattern{m_regularExpression.pattern()};
};

} // namespace qqsfpm

#endif // REGEXPFILTER_H
