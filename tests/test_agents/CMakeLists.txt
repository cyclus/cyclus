INCLUDE_DIRECTORIES(BEFORE ${CYCLUS_CORE_INCLUDE_DIRS})

install_cyclus_standalone("TestAgent" "test_agent" "tests")
install_cyclus_standalone("TestFacility" "test_facility" "tests")
install_cyclus_standalone("TestInst" "test_inst" "tests")
install_cyclus_standalone("TestRegion" "test_region" "tests")
install_cyclus_standalone("TestPositionFac" "test_positionfac" "tests")
install_cyclus_standalone("InjectableFacility" "injectable_facility" "tests")

# Since the injectable_facility needs to use the Clone, Snapshot, etc fxns
# generated with cycpp (#pragma cyclus), and because install_cyclus_standalone
# would make its own test executable if the tests were adjacent to the header,
# we need to tell cyclus_unit_tests where to find the version of the header 
# generated on build via passing its include dir along thru baseagentunittests 
target_include_directories(baseagentunittests INTERFACE
  "${PROJECT_BINARY_DIR}/InjectableFacility")
add_dependencies(baseagentunittests InjectableFacility-sources)

# Rerun cycpp whenever one of the injectable (cycpp) files change, not just
# when the InjectableFacility changes. Otherwise tests won't see changes.
file(GLOB injectable_headers CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/toolkit/*.cycpp.h")
add_custom_command(OUTPUT ${InjectableFacility_H}
  APPEND DEPENDS ${injectable_headers})
