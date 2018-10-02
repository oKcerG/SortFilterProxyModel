# Qt >=5.7 requires C++11, which will be implicitly checked with CMake >3.1.0
cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
cmake_policy(VERSION 3.10.2)

set(SORTFILTERPROXYMODEL_BIN_DIR bin)
set(SORTFILTERPROXYMODEL_QML_DIR qmldir)

# Set target complile flags
#get_target_property(QtTestFLAGS ${tst_sortfilterproxymodel} COMPILE_FLAGS)
if(QtTestFLAGS STREQUAL "QtTest_FLAGS-NOTFOUND")
  SET(QtTestFLAGS "") # set to empty string
else()
  # a space to cleanly separate from existing content
  SET(QtTestFlags "${QtTestFLAGS} ")
endif()
# append our values
SET(QtTestFLAGS "${QtTestFLAGS}warn_on qmltestcase qml_debug no_keywords" )
set_target_properties(${tst_sortfilterproxymodel} PROPERTIES COMPILE_FLAGS ${QtTestFLAGS} )
message(STATUS "QtTest Compiler flags for target 'tst_sortfilterproxymodel': '${QtTestFLAGS}'")

add_definitions(
  #"-DQT_QML_DEBUG"
  "-DQUICK_TEST_SOURCE_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"")

# Find QT5 libraries
#find_package(Qt5 COMPONENTS Test Widgets Gui QuickTest Quick Qml REQUIRED)
find_package(Qt5 COMPONENTS QuickTest Quick Qml REQUIRED)

INCLUDE_DIRECTORIES(${SortFilterProxyModule_SOURCE_DIR} ${Qt5_INCLUDE_DIR} )

set(test_quickpages_SOURCES
  tst_rangefilter.qml
  tst_indexfilter.qml
  tst_sourceroles.qml
  tst_sorters.qml
  tst_helpers.qml
  tst_builtins.qml
  tst_rolesorter.qml
  tst_stringsorter.qml
  tst_proxyroles.qml
  tst_joinrole.qml
  tst_switchrole.qml
  tst_expressionrole.qml)

###
# binary creation
###
set(test_sortfilterproxymodel_HEADERS
  indexsorter.cpp
  testroles.cpp)

set(test_sortfilterproxymodel_SOURCES
  indexsorter.cpp
  testroles.cpp
  tst_sortfilterproxymodel.cpp)

add_executable(test_sortfilterproxymodel
  ${test_sortfilterproxymodel_SOURCES})

# Qt test libraries
set(test_LIBRARIES
  Qt5::QuickTest
  Qt5::Qml
  Qt5::Core)

# include needed QT-Headers (and referenced for MOC)
target_include_directories(test_sortfilterproxymodel
  PUBLIC
  ${test_LIBRARIES})

target_link_libraries(test_sortfilterproxymodel
  $<TARGET_OBJECTS:sortfilterproxymodel>
  ${test_LIBRARIES})

foreach(page ${test_quickpages_SOURCES})
add_custom_command(TARGET test_sortfilterproxymodel POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_SOURCE_DIR}/${page}
      ${CMAKE_CURRENT_BINARY_DIR}/
  COMMENT "Copying QtQuick 'Source-Pages (qml)' to '${CMAKE_CURRENT_BINARY_DIR}'")
endforeach(page)

SET_TARGET_PROPERTIES(
  test_sortfilterproxymodel
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

###
# Tests
###
#set(TestTarget SortFilterProxyModel)
set(TestTarget test_sortfilterproxymodel)

add_test(NAME ${TestTarget}
  #CONFIGURATIONS [Debug|Release]
  COMMAND ./${TestTarget})

add_custom_command(
  TARGET ${TestTarget}
  COMMENT "Run tests"
  POST_BUILD
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  #COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^${TestTarget}$" --output-on-failures
  COMMAND ${TestTarget})

###
# Installing
###
#install(CODE "message(\"Installing TARGETS: ...\")")
#install(TARGETS test_sortfilterproxymodel
#  COMPONENT Test_SortFilterProxyModel
#  RUNTIME DESTINATION ${SORTFILTERPROXYMODEL_BIN_DIR} )

#install(TARGETS test_quickpages_SOURCES
#  COMPONENT Test_SortFilterProxyModel
#  RUNTIME DESTINATION ${SORTFILTERPROXYMODEL_QML_DIR} )
