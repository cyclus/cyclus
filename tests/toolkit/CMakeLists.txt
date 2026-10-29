SET(
  CYCLUS_CORE_TEST_INCLUDE_DIRS ${CYCLUS_CORE_TEST_INCLUDE_DIRS}
  "${CMAKE_CURRENT_SOURCE_DIR}"
  PARENT_SCOPE
  )

set(CYCLUS_TOOLKIT_TEST_COIN_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/building_manager_tests.cc"
    )

FILE(GLOB cc_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

# Run this test archetype through the same cycpp path as production archetypes.
# Compile the generated copy of its tests so quoted includes find its generated
# header, rather than the unprocessed header in this source directory.
use_cyclus("injectable_facility" "injectable_facility")
list(REMOVE_ITEM cc_files
  "${CMAKE_CURRENT_SOURCE_DIR}/injectable_facility.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/injectable_facility_tests.cc")
# Keep generation and compilation in this directory so CMake connects the
# custom commands to their consumers, including incremental test edits.
add_library(injectable_facility_tests OBJECT
  ${injectable_facility_H}
  ${injectable_facility_CC}
  ${injectable_facility_TEST_CC})
target_link_libraries(injectable_facility_tests PRIVATE cyclus gtest)
list(APPEND cc_files $<TARGET_OBJECTS:injectable_facility_tests>)

# cycpp reads included snippets while generating configuration/state methods.
# Regenerate those methods when any snippet changes or a new one is added.
file(GLOB injectable_headers CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/toolkit/*.cycpp.h")
add_custom_command(
  OUTPUT "${PROJECT_BINARY_DIR}/injectable_facility/injectable_facility.h"
  APPEND DEPENDS ${injectable_headers})

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
