INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND git describe --tags OUTPUT_VARIABLE stubs_version OUTPUT_STRIP_TRAILING_WHITESPACE)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/stub_version.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/stub_version.h" @ONLY)

install_cyclus_standalone("StubFacility" "stub_facility" "stubs")
install_cyclus_standalone("StubInst" "stub_inst" "stubs")
install_cyclus_standalone("StubRegion" "stub_region" "stubs")
