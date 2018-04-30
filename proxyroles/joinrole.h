#ifndef JOINROLE_H
#define JOINROLE_H

#include "proxyrole.h"

namespace qqsfpm {

class JoinRole : public ProxyRole
{
    Q_OBJECT
    Q_PROPERTY(QStringList roleNames READ roleNames WRITE setRoleNames NOTIFY roleNamesChanged)
    Q_PROPERTY(QString separator READ separator WRITE setSeparator NOTIFY separatorChanged)

public:
    using ProxyRole::ProxyRole;

    QStringList roleNames() const;
    void setRoleNames(const QStringList& roleNames);

    QString separator() const;
    void setSeparator(const QString& separator);

Q_SIGNALS:
    void roleNamesChanged();

    void separatorChanged();

private:
    QStringList m_roleNames;
    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;
    QString m_separator = " ";
};

}

#endif // JOINROLE_H
