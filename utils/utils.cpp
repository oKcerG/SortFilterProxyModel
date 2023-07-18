#include "utils.h"
#include <QDebug>
#include <QDateTime>

namespace qqsfpm {

int compareVariants(const QVariant &lhs, const QVariant &rhs)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0) // qt 5
    // Do the QString check first because otherwise the canConvert<int> check will get hit for strings.
    if (static_cast<QMetaType::Type>(lhs.type()) == QMetaType::QString && static_cast<QMetaType::Type>(rhs.type()) == QMetaType::QString) {
        const auto lhsValue = lhs.toString();
        const auto rhsValue = rhs.toString();
        if (lhsValue == rhsValue)
            return 0;
        return lhsValue.compare(rhsValue, Qt::CaseInsensitive);
    } else if (static_cast<QMetaType::Type>(lhs.type()) == QMetaType::Bool && static_cast<QMetaType::Type>(rhs.type()) == QMetaType::Bool) {
        const auto lhsValue = lhs.toBool();
        const auto rhsValue = rhs.toBool();
        if (lhsValue == rhsValue)
            return 0;
        // false < true.
        return !lhsValue ? -1 : 1;
    } else if (static_cast<QMetaType::Type>(lhs.type()) == QMetaType::QDate && static_cast<QMetaType::Type>(rhs.type()) == QMetaType::QDate) {
        const auto lhsValue = lhs.toDate();
        const auto rhsValue = rhs.toDate();
        if (lhsValue == rhsValue)
            return 0;
        return lhsValue < rhsValue ? -1 : 1;
    } else if (static_cast<QMetaType::Type>(lhs.type()) == QMetaType::QDateTime && static_cast<QMetaType::Type>(rhs.type()) == QMetaType::QDateTime) {
        const auto lhsValue = lhs.toDateTime();
        const auto rhsValue = rhs.toDateTime();
        if (lhsValue == rhsValue)
            return 0;
        return lhsValue < rhsValue ? -1 : 1;
    } else if (static_cast<QMetaType::Type>(lhs.type()) == QMetaType::QStringList && static_cast<QMetaType::Type>(rhs.type()) == QMetaType::QStringList) {
        const auto lhsValue = lhs.toStringList();
        const auto rhsValue = rhs.toStringList();
        if (lhsValue == rhsValue)
            return 0;
        return lhsValue < rhsValue ? -1 : 1;
    } else if (lhs.canConvert<int>() && rhs.canConvert<int>()) {
        const auto lhsValue = lhs.toInt();
        const auto rhsValue = rhs.toInt();
        if (lhsValue == rhsValue)
            return 0;
        return lhsValue < rhsValue ? -1 : 1;
    } else if (lhs.canConvert<qreal>() && rhs.canConvert<qreal>()) {
        const auto lhsValue = lhs.toReal();
        const auto rhsValue = rhs.toReal();
        if (qFuzzyCompare(lhsValue, rhsValue))
            return 0;
        return lhsValue < rhsValue ? -1 : 1;
    }

    qWarning() << "Don't know how to compare" << lhs << "against" << rhs << "- returning 0";
    return 0;
#else
    return QPartialOrdering::Less == QVariant::compare(lhs, rhs);
#endif
}

}
