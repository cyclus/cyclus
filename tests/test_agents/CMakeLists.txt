INCLUDE_DIRECTORIES(BEFORE ${CYCLUS_CORE_INCLUDE_DIRS})

install_cyclus_standalone("TestAgent" "test_agent" "tests")
install_cyclus_standalone("TestFacility" "test_facility" "tests")
install_cyclus_standalone("TestInst" "test_inst" "tests")
install_cyclus_standalone("TestRegion" "test_region" "tests")
install_cyclus_standalone("TestPositionFac" "test_positionfac" "tests")

# Injectable test hosts are built as one test-only Cycpp module. Their
# colocated test sources are added to cyclus_unit_tests below.
add_subdirectory(injection_facilities)

set(CYCLUS_CORE_TEST_SOURCE
    "${CYCLUS_CORE_TEST_SOURCE}"
    "${INJECTION_FACILITIES_TEST_SOURCE}"
    PARENT_SCOPE)
set(CYCLUS_TEST_LIBRARIES
    "${CYCLUS_TEST_LIBRARIES}"
    "${INJECTION_FACILITIES_TEST_LIBRARY}"
    PARENT_SCOPE)
