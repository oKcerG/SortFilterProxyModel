#ifndef INDEXFILTER_H
#define INDEXFILTER_H

#include "filter.h"
#include <QVariant>

namespace qqsfpm {

class IndexFilter: public Filter {
    Q_OBJECT
    Q_PROPERTY(QVariant minimumIndex READ minimumIndex WRITE setMinimumIndex NOTIFY minimumIndexChanged)
    Q_PROPERTY(QVariant maximumIndex READ maximumIndex WRITE setMaximumIndex NOTIFY maximumIndexChanged)
    Q_PROPERTY(QVariant indexes READ indexes WRITE setIndexes NOTIFY indexesChanged)

public:
    using Filter::Filter;

    const QVariant& minimumIndex() const;
    void setMinimumIndex(const QVariant& minimumIndex);

    const QVariant& maximumIndex() const;
    void setMaximumIndex(const QVariant& maximumIndex);

    const QVariant& indexes() const;
    void setIndexes(const QVariant& indexes);

protected:
    bool filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;

Q_SIGNALS:
    void minimumIndexChanged();
    void maximumIndexChanged();
    void indexesChanged();

private:
    QVariant m_minimumIndex;
    QVariant m_maximumIndex;
    QVariant m_indexes;
};

}

#endif // INDEXFILTER_H
