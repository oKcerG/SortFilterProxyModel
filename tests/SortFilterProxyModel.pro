TEMPLATE = app
TARGET = tst_sortfilterproxymodel
QT += qml quick
CONFIG += c++11 warn_on qmltestcase qml_debug

include(../SortFilterProxyModel.pri)

SOURCES += tst_sortfilterproxymodel.cpp

OTHER_FILES += \
    tst_rangefilter.qml
