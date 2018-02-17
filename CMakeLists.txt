cmake_minimum_required(VERSION 3.10.2)
cmake_policy(VERSION 3.10.2)
project(SortFilterProxyModel)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  # MinGW: 1) cd; mkdir ./build; cd ./build;
  # 	   2) cmake -G "MinGW Makefiles" <src_path>
  #        2) mingw32-make
  set(QT_STYLE_OVERRIDE Windows)
  # MinGW bases Qt Tree
  set(Qt5_DIR "C:/Qt/Qt5.10.0/5.10.0/mingw53_32/lib/cmake/Qt5")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  # Install adwaita-qt5 (GTK+ style)
  set(QT_STYLE_OVERRIDE adwaita)
endif()

# Use our modified modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SORTFILTERPROXYMODEL_SOURCE_DIR}/cmake")
set(SORTFILTERPROXYMODEL_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

# run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to includes this location
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# run rcc code generator automatically
set(CMAKE_AUTORCC ON)

# enable tests (seated in subdir tests)
enable_testing()

# Bump up warning levels appropriately for clang, gcc & msvc and build in debug mode
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU"
    OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11 -fPIC")
	#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
	#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O3")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
endif()

message(STATUS "Using C++ Compiler specific attributes for ${CMAKE_CXX_COMPILER_ID}: ${CMAKE_CXX_FLAGS}")
#message(STATUS "Using C++ Compiler specific warnings for ${CMAKE_C_COMPILER_ID}: ${CMAKE_CXX_FLAGS_WARNINGS}")

####
# Makefile Rules
###
if(EXISTS "rules.ninja")
    add_custom_target(release COMMAND ${CMAKE_COMMAND} -GNinja -DCMAKE_BUILD_TYPE=Release . WORKING_DIRECTORY .)
    add_custom_target(debug COMMAND ${CMAKE_COMMAND} -GNinja -DCMAKE_BUILD_TYPE=Debug . WORKING_DIRECTORY .)
else()
    add_custom_target(release COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release . WORKING_DIRECTORY .)
    add_custom_target(debug COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug . WORKING_DIRECTORY .)
endif()

###
# Project Settings: Global
###
set(SORTFILTERPROXYMODEL_VERSION_MAJOR 0)
set(SORTFILTERPROXYMODEL_VERSION_MINOR 2)
set(SORTFILTERPROXYMODEL_VERSION_PATCHLEVEL 0)
set(SORTFILTERPROXYMODEL_VERSION_STRING ${SORTFILTERPROXYMODEL_VERSION_MAJOR}.${SORTFILTERPROXYMODEL_VERSION_MINOR}.${SORTFILTERPROXYMODEL_VERSION_PATCHLEVEL})

message(STATUS "${project}: global definitions:")
message(STATUS " SORTFILTERPROXYMODEL_VERSION_MAJOR='${SORTFILTERPROXYMODEL_VERSION_MAJOR}'")
message(STATUS " SORTFILTERPROXYMODEL_VERSION_MINOR='${SORTFILTERPROXYMODEL_VERSION_MINOR}'")
message(STATUS " SORTFILTERPROXYMODEL_VERSION_PATCHLEVEL='${SORTFILTERPROXYMODEL_VERSION_PATCHLEVEL}'")
message(STATUS " SORTFILTERPROXYMODEL_VERSION_STRING='${SORTFILTERPROXYMODEL_VERSION_STRING}'")
message(STATUS " Qt5_DIR: ${Qt5_DIR}")
message(STATUS " QT_STYLE_OVERRIDE: ${QT_STYLE_OVERRIDE}")

set(SORTFILTERPROXYMODEL_BIN_DIR  "${CMAKE_INSTALL_PREFIX}/bin")
set(SORTFILTERPROXYMODEL_LIB_DIR  "${CMAKE_INSTALL_PREFIX}/lib")
set(SORTFILTERPROXYMODEL_HEADER_DIR  "${CMAKE_INSTALL_PREFIX}/inclde/${project}")
set(SORTFILTERPROXYMODEL_PLUGINS_DIR  "${CMAKE_INSTALL_PREFIX}/etc/${project}/plugins")
set(SORTFILTERPROXYMODEL_DOC_DIR  "${CMAKE_INSTALL_PREFIX}/share/doc")
set(SORTFILTERPROXYMODEL_LOCAL_DIR  "${CMAKE_INSTALL_PREFIX}/share/locale")

if(AQBANKING_ENABLE_DEV_OPTIONS)
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "SortFilterProxyModel")
  set(CPACK_PACKAGE_VENDOR "")
  set(CPACK_PACKAGE_CONTACT "http://")
  set(CPACK_PACKAGE_VERSION_MAJOR ${SORTFILTERPROXYMODEL_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${SORTFILTERPROXYMODEL_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_DATABASE ${SORTFILTERPROXYMODEL_VERSION_DATABASE})
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
  set(CPACK_PACKAGE_VERSION "${AQBANKING_VERSION}")
  include(CPack)
endif()

###
# Project Settings: pre-processor
###
if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
  string(APPEND CMAKE_C_FLAGS " -D_FORTIFY_SOURCE=2")
endif()

add_definitions(
  "-DQT_SYLE_OVERRIDE")

get_directory_property(DirDefs COMPILE_DEFINITIONS)
message(STATUS "SortFilterProxyModel: active pre-processor definitions:")
foreach(d ${DirDefs})
    message( STATUS " " ${d})
endforeach()

#feature_summary(INCLUDE_QUIET_PACKAGES WHAT ALL)
if (NOT DEFINED SortFilterProxyModule_SOURCE_DIR)
  #  sources in {CMAKE_CURRENT_SOURCE_DIR} include headers seated in given subdirs
  set(SortFilterProxyModule_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(AFTER ${SortFilterProxyModule_SOURCE_DIR})
  message(STATUS "Project Source-Dir: ${SortFilterProxyModule_SOURCE_DIR}")
endif()

###
# HEADERS / SOURCES
###
set(sortfilterproxymodel_HEADERS
  qqmlsortfilterproxymodel.h 
  filter.h
  sorter.h
  proxyrole.h)

set(sortfilterproxymodel_SOURCES
  qqmlsortfilterproxymodel.cpp
  filter.cpp
  sorter.cpp
  proxyrole.cpp)

###
# LIBRARY
###
# Find QT5 libraries
find_package(Qt5 COMPONENTS Qml Core REQUIRED)

# Qt libraries
set(sortfilterproxymodel_LIBRARIES
  Qt5::Core
  Qt5::Qml)

# Object library
add_library(sortfilterproxymodel OBJECT
  ${sortfilterproxymodel_SOURCES})

# include needed QT-Headers (they are referenced by MOC)
target_include_directories(sortfilterproxymodel
  PUBLIC
  #${sortfilterproxymodel_LIBRARIES}
  $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
  $<TARGET_PROPERTY:Qt5::Qml,INTERFACE_INCLUDE_DIRECTORIES>
)

###
# Project Structure: subdirectories
###
add_subdirectory(docs)
add_subdirectory(tests)

set("SORTFILTERPROXYMODEL_DOC"
  LICENSE
  SortFilterProxyModel.pri
  README.md)

###
# Installing
###
install(FILES ${SORTFILTERPROXYMODEL_DOC}
  COMPONENT ${project}
  DESTINATION ${SORTFILTERPROXYMODEL_DOC_DIR}/${project}
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
