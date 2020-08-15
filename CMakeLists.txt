cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(SortFilterProxyModel LANGUAGES CXX)
set(SFPM_VERSION_MAJOR "0")
set(SFPM_VERSION_MINOR "1")
set(SFPM_VERSION_PATCH "1")

set(PROJECT_VERSION "${SFPM_VERSION_MAJOR}.${SFPM_VERSION_MINOR}.${SFPM_VERSION_PATCH}")

# cmake macros
include(FeatureSummary)
include(GNUInstallDirs)

# qt5 libs
find_package(Qt5 COMPONENTS Core Qml CONFIG REQUIRED)

# Build options (exclusive on or the other - not both)
# * developers might prefer BUILD_OBJECT_LIB
# * packagers should use BUILD_SHARED_LIB
option(BUILD_OBJECT_LIB "Build object library. Add code in your project and add \$<TARGET_OBJECTS:SortFilterProxyModel> in your CMakeLists.txt" ON)
option(BUILD_SHARED_LIB "Build shared library. Build and install library to a folder wher cmake can find it for your project" OFF)

if(BUILD_OBJECT_LIB AND BUILD_SHARED_LIB)
    message(FATAL_ERROR "You cannot select both BUILD_OBJECT_LIB and BUILD_SHARED_LIB")
endif(BUILD_OBJECT_LIB AND BUILD_SHARED_LIB)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON) # This is to find generated *.moc and *.h files in build dir

set(SFPM_SOURCES
    qqmlsortfilterproxymodel.cpp
    filters/filter.cpp
    filters/filtercontainer.cpp
    filters/rolefilter.cpp
    filters/valuefilter.cpp
    filters/indexfilter.cpp
    filters/regexpfilter.cpp
    filters/rangefilter.cpp
    filters/expressionfilter.cpp
    filters/filtercontainerfilter.cpp
    filters/anyoffilter.cpp
    filters/alloffilter.cpp
    filters/filtersqmltypes.cpp
    sorters/sorter.cpp
    sorters/sortercontainer.cpp
    sorters/rolesorter.cpp
    sorters/stringsorter.cpp
    sorters/expressionsorter.cpp
    sorters/sortersqmltypes.cpp
    proxyroles/proxyrole.cpp
    proxyroles/proxyrolecontainer.cpp
    proxyroles/joinrole.cpp
    proxyroles/switchrole.cpp
    proxyroles/expressionrole.cpp
    proxyroles/proxyrolesqmltypes.cpp   
    proxyroles/singlerole.cpp   
    proxyroles/regexprole.cpp
    sorters/filtersorter.cpp
    proxyroles/filterrole.cpp
    )

if(BUILD_OBJECT_LIB)
    message("Build SortFilterProxyModel object library for in-tree usage")

    add_library(SortFilterProxyModel OBJECT ${SFPM_SOURCES})

    target_include_directories(SortFilterProxyModel PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}
        $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:Qt5::Qml,INTERFACE_INCLUDE_DIRECTORIES>
        )
endif(BUILD_OBJECT_LIB)

if(BUILD_SHARED_LIB)
    message("Build SortFilterProxyModel shared library for target installation")

    add_library(SortFilterProxyModel SHARED ${SFPM_SOURCES} )
    set_target_properties(SortFilterProxyModel PROPERTIES VERSION ${PROJECT_VERSION})
    set_target_properties(SortFilterProxyModel PROPERTIES SOVERSION ${SFPM_VERSION_MAJOR})

    target_link_libraries(SortFilterProxyModel
        PRIVATE
            Qt5::Core
            Qt5::Qml
            # ask linker to help us finding unresolved symbols
            "-Wl,--no-undefined"
        )
    # install library
    install(TARGETS SortFilterProxyModel
        EXPORT SortFilterProxyModelExport
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
    # configure SortFilterProxyModelConfigVersion.cmake
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        SortFilterProxyModelConfigVersion.cmake
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY SameMinorVersion
        )

    # configure SortFilterProxyModelConfig.cmake
    configure_file(SortFilterProxyModelConfig.cmake.in SortFilterProxyModelConfig.cmake @ONLY)

    # install SortFilterProxyModelConfig(Version).cmake
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/SortFilterProxyModelConfig.cmake"
                  "${CMAKE_CURRENT_BINARY_DIR}/SortFilterProxyModelConfigVersion.cmake"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SortFilterProxyModel
        )

    # install targets cmake-files
    install(EXPORT SortFilterProxyModelExport
        FILE SortFilterProxyModelTargets.cmake
        NAMESPACE SortFilterProxyModel::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SortFilterProxyModel
        )

endif(BUILD_SHARED_LIB)
