# Qt >=5.7 requires C++11, which will be implicitly checked with CMake >3.1.0
cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
cmake_policy(VERSION 3.10.2)

set(doc_SOURCES
  offline.css)

###
# Installing
###
install(CODE "message(\"Installing Documentation: ...\")")
install(FILES ${doc_SOURCES}
    COMPONENT ${project}
    DESTINATION ${SORTFILTERPROXYMODEL_DOC_DIR}/${project}/style
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

