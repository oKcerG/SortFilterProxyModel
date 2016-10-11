#ifndef FILTER_H
#define FILTER_H

#include <QObject>
#include "qqmlsortfilterproxymodel.h"

namespace qqsfpm {

class Filter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool inverted READ inverted WRITE setInverted NOTIFY invertedChanged)
    friend class QQmlSortFilterProxyModel;

public:
    explicit Filter(QObject *parent = 0);
    virtual ~Filter() = default;

    bool enabled() const;
    void setEnabled(bool enabled);

    bool inverted() const;
    void setInverted(bool inverted);

    bool filterAcceptsRow(const QModelIndex &sourceIndex) const;

signals:
    void enabledChanged();
    void invertedChanged();
    void filterChanged();
    void invalidate();

protected:
    QQmlSortFilterProxyModel* proxyModel() const;
    virtual bool filterRow(const QModelIndex &sourceIndex) const = 0;
    virtual void proxyModelCompleted();

private slots:
    void onFilterChanged();

private:
    bool m_enabled = true;
    bool m_inverted = false;
    QQmlSortFilterProxyModel* m_proxyModel = nullptr;
};

class RoleFilter : public Filter
{
    Q_OBJECT
    Q_PROPERTY(QString roleName READ roleName WRITE setRoleName NOTIFY roleNameChanged)

public:
    using Filter::Filter;

    const QString& roleName() const;
    void setRoleName(const QString& roleName);

signals:
    void roleNameChanged();

protected:
    QVariant sourceData(const QModelIndex &sourceIndex) const;

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
    bool filterRow(const QModelIndex &sourceIndex) const override;

signals:
    void valueChanged();

private:
    QVariant m_value;
};

class IndexFilter: public Filter {
    Q_OBJECT
    Q_PROPERTY(int minimumIndex READ minimumIndex WRITE setMinimumIndex NOTIFY minimumIndexChanged RESET resetMinimumIndex)
    Q_PROPERTY(int maximumIndex READ maximumIndex WRITE setMaximumIndex NOTIFY maximumIndexChanged RESET resetMaximumIndex)

public:
    using Filter::Filter;

    int minimumIndex() const;
    void setMinimumIndex(int minimumIndex);
    void resetMinimumIndex();

    int maximumIndex() const;
    void setMaximumIndex(int maximumIndex);
    void resetMaximumIndex();

protected:
    bool filterRow(const QModelIndex &sourceIndex) const override;

signals:
    void minimumIndexChanged();
    void maximumIndexChanged();

private:
    int m_minimumIndex;
    bool m_minimumIndexIsSet = false;

    int m_maximumIndex;
    bool m_maximumIndexIsSet = false;
};

}

#endif // FILTER_H
