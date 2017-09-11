TEMPLATE = app
TARGET = tst_sortfilterproxymodel
QT += qml quick
CONFIG += c++11 warn_on qmltestcase qml_debug no_keywords

include(../SortFilterProxyModel.pri)

HEADERS += \
    indexsorter.h

SOURCES += \
    tst_sortfilterproxymodel.cpp \
    indexsorter.cpp

OTHER_FILES += \
    tst_rangefilter.qml \
    tst_indexfilter.qml \
    tst_sourceroles.qml \
    tst_sorters.qml \
    tst_helpers.qml \
    tst_builtins.qml \
    tst_rolesorter.qml
