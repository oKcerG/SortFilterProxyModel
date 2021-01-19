cmake_minimum_required(VERSION 3.1)

project(SortFilterProxyModel LANGUAGES CXX)

find_package(Qt5 REQUIRED
    Core
    Qml
    )

# Module version
set(SFP_MAJOR 0)
set(SFP_MINOR 2)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON) # This is to find generated *.moc and *.h files in build dir

set(SOURCES
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

# --- Object library to link directly with the application
add_library(SortFilterProxyModel OBJECT ${SOURCES})
set_target_properties(SortFilterProxyModel
    PROPERTIES
        CXX_STANDARD 11
        EXCLUDE_FROM_ALL TRUE
    )
target_link_libraries(SortFilterProxyModel PUBLIC
    Qt5::Core
    Qt5::Qml
    )

# --- Plugin library
add_library(SortFilterProxyModelPlugin SHARED
    ${SOURCES}
    sortfilterproxymodelplugin.cpp
    )
target_link_libraries(SortFilterProxyModelPlugin PUBLIC
    Qt5::Core
    Qt5::Qml
    )

target_compile_definitions(SortFilterProxyModelPlugin
    PRIVATE
        QML_PLUGIN
        QML_IMPORT_MAJOR_VERSION=${SFP_MAJOR}
        QML_IMPORT_MINOR_VERSION=${SFP_MINOR}
    )
set_target_properties(SortFilterProxyModelPlugin
    PROPERTIES
        CXX_STANDARD 11
        OUTPUT_NAME sortfilterproxymodel
        DEBUG_POSTFIX d
        EXCLUDE_FROM_ALL TRUE
    )

# --- Create installed package
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
set(PACKAGE_NAME ${CMAKE_PROJECT_NAME})

set(QML_MODULE_PATH qml_module)
configure_package_config_file(
    cmake/SortFilterProxyModelConfig.cmake.in
    ${CMAKE_BINARY_DIR}/${PACKAGE_NAME}Config.cmake
    INSTALL_DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${PACKAGE_NAME}
    PATH_VARS
        QML_MODULE_PATH
    )

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake
    VERSION
        "${SFP_MAJOR}.${SFP_MINOR}"
    COMPATIBILITY
        SameMajorVersion
    )

set(SFP_INSTALL_PATH "${QML_MODULE_PATH}/QmlSortFilterProxyModel")

# qml module
install(TARGETS SortFilterProxyModelPlugin
    RUNTIME DESTINATION ${SFP_INSTALL_PATH}
    COMPONENT sortfilterproxymodel
    )
install(
    FILES module/qmldir module/plugins.qmltypes
    DESTINATION ${SFP_INSTALL_PATH}
    COMPONENT sortfilterproxymodel
    )

# cmake package config
install(
    FILES
        ${CMAKE_BINARY_DIR}/${PACKAGE_NAME}Config.cmake
        ${CMAKE_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PACKAGE_NAME}
    )
