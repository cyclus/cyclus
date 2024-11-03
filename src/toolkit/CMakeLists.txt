INCLUDE_DIRECTORIES(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  )

SET(
  CYCLUS_CORE_INCLUDE_DIRS ${CYCLUS_CORE_INCLUDE_DIRS}
  "${CMAKE_CURRENT_SOURCE_DIR}"
  PARENT_SCOPE
  )

set(CYCLUS_TOOLKIT_COIN_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/building_manager.cc"
    )

FILE(GLOB cc_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

# Remove toolkit source files that rely on COIN if we don't have it
if(NOT COIN_FOUND)
  foreach(ccfile ${CYCLUS_TOOLKIT_COIN_SRC})
    list(REMOVE_ITEM cc_files ${ccfile})
  endforeach()
endif()

SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} ${cc_files} PARENT_SCOPE)

FILE(GLOB h_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
FILE(GLOB cycpp_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cycpp.h")
INSTALL(
  FILES ${h_files} ${cycpp_files}
  DESTINATION include/cyclus/toolkit
  COMPONENT core
  )
