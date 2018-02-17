# Qt >=5.7 requires C++11, which will be implicitly checked with CMake >3.1.0
cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
cmake_policy(VERSION 3.10.2)

set(docs_SOURCES
  index.html
  qml-allof.html
  qml-allof-members.html
  qml-anyof.html
  qml-anyof-members.html
  qml-expressionfilter.html
  qml-expressionfilter-members.html
  qml-expressionrole.html
  qml-expressionrole-members.html
  qml-expressionsorter.html
  qml-expressionsorter-members.html
  qml-filter.html
  qml-filter-members.html
  qml-indexfilter.html
  qml-indexfilter-members.html
  qml-joinrole.html
  qml-joinrole-members.html
  qml-proxyrole.html
  qml-proxyrole-members.html
  qml-rangefilter.html
  qml-rangefilter-members.html
  qml-regexpfilter.html
  qml-regexpfilter-members.html
  qml-rolefilter.html
  qml-rolefilter-members.html
  qml-rolesorter.html
  qml-rolesorter-members.html
  qml-sorter.html
  qml-sorter-members.html
  qml-sortfilterproxymodel.html
  qml-sortfilterproxymodel-members.html
  qml-stringsorter.html
  qml-stringsorter-members.html
  qml-switchrole.html
  qml-switchrole-members.html
  qml-valuefilter.html
  qml-valuefilter-members.html
  sortfilterproxymodel.index)

###
# Project Structure: subdirectories
###
add_subdirectory(style)


###
# Installing
###
install(FILES ${docs_SOURCES}
  COMPONENT ${project}
  DESTINATION ${SORTFILTERPROXYMODEL_DOC_DIR}/${project}/
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

