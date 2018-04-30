#ifndef TESTROLES_H
#define TESTROLES_H

#include "proxyroles/proxyrole.h"
#include <QVariant>

class StaticRole : public qqsfpm::ProxyRole
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
public:
    using qqsfpm::ProxyRole::ProxyRole;

    QVariant value() const;
    void setValue(const QVariant& value);

Q_SIGNALS:
    void valueChanged();

protected:

private:
    QVariant data(const QModelIndex& sourceIndex, const qqsfpm::QQmlSortFilterProxyModel& proxyModel) override;
    QVariant m_value;
};

class SourceIndexRole : public qqsfpm::ProxyRole
{
public:
    using qqsfpm::ProxyRole::ProxyRole;

private:
    QVariant data(const QModelIndex& sourceIndex, const qqsfpm::QQmlSortFilterProxyModel& proxyModel) override;
};

#endif // TESTROLES_H
