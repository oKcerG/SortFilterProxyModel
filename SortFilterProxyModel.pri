!c++11: warning("SortFilterProxyModel needs c++11, add CONFIG += c++11 to your .pro")

INCLUDEPATH += $$PWD

HEADERS += $$PWD/qqmlsortfilterproxymodel.h \
    $$PWD/filter.h \
    $$PWD/sorter.h
SOURCES += $$PWD/qqmlsortfilterproxymodel.cpp \
    $$PWD/filter.cpp \
    $$PWD/sorter.cpp
