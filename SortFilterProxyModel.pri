!contains( CONFIG, c\+\+1[14] ): warning("SortFilterProxyModel needs at least c++11, add CONFIG += c++11 to your .pro")

INCLUDEPATH += $$PWD

HEADERS += $$PWD/qqmlsortfilterproxymodel.h \
    $$PWD/filter.h \
    $$PWD/sorter.h \
    $$PWD/proxyrole.h
SOURCES += $$PWD/qqmlsortfilterproxymodel.cpp \
    $$PWD/filter.cpp \
    $$PWD/sorter.cpp \
    $$PWD/proxyrole.cpp
