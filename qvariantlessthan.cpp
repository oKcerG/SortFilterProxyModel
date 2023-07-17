#include "qvariantlessthan.h"

#include <QDateTime>

namespace qqsfpm {

/*!
    \brief Less-than operator for generic QVariants

    Since Qt 5.15 deprecated the less-than operator of QVariant, we
    have to provide our own implementation. On older Qt versions,
    use the original implementation.

    Includes special implementations for numberic types, char, date and
    time. Everything else is converted to String and compared then.
*/
bool lessThan(const QVariant &lhs, const QVariant &rhs)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    return lhs < rhs;
#else
    static const auto numericTypes = QVector<QMetaType::Type>{
        QMetaType::Int,
        QMetaType::UInt,
        QMetaType::LongLong,
        QMetaType::ULongLong,
        QMetaType::Float,
        QMetaType::Double,
    };
    static const auto unsignedTypes = QVector<QMetaType::Type>{
        QMetaType::UInt,
        QMetaType::ULongLong,
    };
    static const auto dateTimeTypes = QVector<QMetaType::Type>{
        QMetaType::QDate,
        QMetaType::QTime,
        QMetaType::QDateTime,
    };

    const auto lt = static_cast<QMetaType::Type>(lhs.userType());
    const auto rt = static_cast<QMetaType::Type>(rhs.userType());
    if (numericTypes.contains(lt) && numericTypes.contains(rt)) {
        if (lt == QMetaType::Double || lt == QMetaType::Float
                || rt == QMetaType::Double || rt == QMetaType::Float) {
            return lhs.toDouble() < rhs.toDouble();
        } else {
            const auto ul = unsignedTypes.contains(lt);
            const auto ur = unsignedTypes.contains(rt);
            if (ul && ur) {
                return lhs.toULongLong() < rhs.toULongLong();
            } else if (!ul && !ur) {
                return lhs.toLongLong() < rhs.toLongLong();
            } else if (ul) {
                const auto r = rhs.toLongLong();
                return r > 0 &&
                    lhs.toULongLong() < static_cast<unsigned long long>(r);
            } else {
                const auto l = lhs.toLongLong();
                return l < 0 ||
                    static_cast<unsigned long long>(l) < rhs.toULongLong();
            }
        }
    } else if (dateTimeTypes.contains(lt) && dateTimeTypes.contains(rt)) {
        if (lt == QMetaType::QDate && rt == QMetaType::QDate) {
            return lhs.toDate() < rhs.toDate();
        } else if (lt == QMetaType::QTime && rt == QMetaType::QTime) {
            return lhs.toTime() < rhs.toTime();
        } else {
            return lhs.toDateTime() < rhs.toDateTime();
        }
    } else if (lt == QMetaType::Char && rt == QMetaType::Char) {
        return lhs.toChar() < rhs.toChar();
    } else {
        return lhs.toString() < rhs.toString();
    }
#endif
}

} // namespace qqsfpm
