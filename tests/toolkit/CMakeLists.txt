SET(
  CYCLUS_CORE_TEST_INCLUDE_DIRS ${CYCLUS_CORE_TEST_INCLUDE_DIRS}
  "${CMAKE_CURRENT_SOURCE_DIR}"
  PARENT_SCOPE
  )

set(CYCLUS_TOOLKIT_TEST_COIN_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/building_manager_tests.cc"
    )

FILE(GLOB cc_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

# Remove toolkit test source files that rely on COIN if we don't have it
if(NOT COIN_FOUND)
  foreach(ccfile ${CYCLUS_TOOLKIT_TEST_COIN_SRC})
    list(REMOVE_ITEM cc_files ${ccfile})
  endforeach()
endif()

SET(
  CYCLUS_CORE_TEST_SOURCE ${CYCLUS_CORE_TEST_SOURCE}
  ${cc_files}
  PARENT_SCOPE
  )
