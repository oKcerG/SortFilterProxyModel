#ifndef FILTER_H
#define FILTER_H

#include <QObject>
#include "qqmlsortfilterproxymodel.h"

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

#endif // FILTER_H
