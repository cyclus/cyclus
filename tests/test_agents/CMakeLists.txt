INCLUDE_DIRECTORIES(BEFORE ${CYCLUS_CORE_INCLUDE_DIRS})

install_cyclus_standalone("TestAgent" "test_agent" "tests")
install_cyclus_standalone("TestFacility" "test_facility" "tests")
install_cyclus_standalone("TestInst" "test_inst" "tests")
install_cyclus_standalone("TestRegion" "test_region" "tests")
install_cyclus_standalone("TestPositionFac" "test_positionfac" "tests")
install_cyclus_standalone("InjectableFacility" "injectable_facility" "tests")

# ----- A bunch of CMake to get the InjectableFacility tests to work -----

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

# Generate concrete C++ registrations from the existing backend type table.
# The script locates its source inputs; CMake tracks them as build dependencies.
set(injectable_type_generator
  "${CMAKE_CURRENT_SOURCE_DIR}/generate_injectable_types.py")
set(injectable_type_header "${CMAKE_CURRENT_BINARY_DIR}/injectable_dbtypes.h")
add_custom_command(OUTPUT "${injectable_type_header}"
  COMMAND "${Python3_EXECUTABLE}" "${injectable_type_generator}"
          "${injectable_type_header}"
  DEPENDS "${injectable_type_generator}"
          "${PROJECT_SOURCE_DIR}/share/dbtypes.json"
          "${PROJECT_SOURCE_DIR}/cli/cycpp.py"
  VERBATIM)
# share/CMakeLists.txt regenerates the table during configuration.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/query_backend.h")
add_custom_target(injectable-dbtypes DEPENDS "${injectable_type_header}")
add_dependencies(baseagentunittests injectable-dbtypes)
target_include_directories(baseagentunittests INTERFACE
  "${CMAKE_CURRENT_BINARY_DIR}")

# ----- End InjectableFacility CMake -----
