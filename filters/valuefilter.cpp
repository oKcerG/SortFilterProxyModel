#include "valuefilter.h"

namespace qqsfpm {

/*!
    \qmltype ValueFilter
    \inherits RoleFilter
    \inqmlmodule SortFilterProxyModel
    \ingroup Filters
    \brief Filters rows matching exactly a value.

    A ValueFilter is a simple \l RoleFilter that accepts rows matching exactly the filter's value

    In the following example, only rows with their \c favorite role set to \c true will be accepted when the checkbox is checked :
    \code
    CheckBox {
       id: showOnlyFavoriteCheckBox
    }

    SortFilterProxyModel {
       sourceModel: contactModel
       filters: ValueFilter {
           roleName: "favorite"
           value: true
           enabled: showOnlyFavoriteCheckBox.checked
       }
    }
    \endcode

*/

/*!
    \qmlproperty variant ValueFilter::value

    This property holds the value used to filter the contents of the source model.
*/
const QVariant &ValueFilter::value() const
{
    return m_value;
}

void ValueFilter::setValue(const QVariant& value)
{
    if (m_value == value)
        return;

    m_value = value;
    Q_EMIT valueChanged();
    invalidate();
}

bool ValueFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    QVariant srcData = sourceData(sourceIndex, proxyModel);
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    // Implicitly convert the types. This was the behavior in Qt5 and makes QML
    // interop much easier, e.g. when comparing QByteArray against QString
    if (srcData.metaType() != m_value.metaType()) {
        QVariant converted = srcData;
        if (converted.convert(m_value.metaType())) {
            srcData = converted;
        }
    }
#endif
    return !m_value.isValid() || m_value == srcData;
}

}
