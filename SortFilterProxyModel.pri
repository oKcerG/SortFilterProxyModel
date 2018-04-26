!contains( CONFIG, c\+\+1[14] ): warning("SortFilterProxyModel needs at least c++11, add CONFIG += c++11 to your .pro")

INCLUDEPATH += $$PWD

HEADERS += $$PWD/qqmlsortfilterproxymodel.h \
    $$PWD/sorter.h \
    $$PWD/proxyrole.h \
    $$PWD/filters/filter.h \
    $$PWD/filters/filtercontainer.h \
    $$PWD/filters/rolefilter.h \
    $$PWD/filters/valuefilter.h \
    $$PWD/filters/indexfilter.h \
    $$PWD/filters/regexpfilter.h \
    $$PWD/filters/rangefilter.h \
    $$PWD/filters/expressionfilter.h \
    $$PWD/filters/filtercontainerfilter.h \
    $$PWD/filters/anyoffilter.h \
    $$PWD/filters/alloffilter.h

SOURCES += $$PWD/qqmlsortfilterproxymodel.cpp \
    $$PWD/sorter.cpp \
    $$PWD/proxyrole.cpp \
    $$PWD/filters/filter.cpp \
    $$PWD/filters/filtercontainer.cpp \
    $$PWD/filters/rolefilter.cpp \
    $$PWD/filters/valuefilter.cpp \
    $$PWD/filters/indexfilter.cpp \
    $$PWD/filters/regexpfilter.cpp \
    $$PWD/filters/rangefilter.cpp \
    $$PWD/filters/expressionfilter.cpp \
    $$PWD/filters/filtercontainerfilter.cpp \
    $$PWD/filters/anyoffilter.cpp \
    $$PWD/filters/alloffilter.cpp \
    $$PWD/filters/filtersqmltypes.cpp
