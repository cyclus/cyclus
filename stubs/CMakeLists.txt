ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src)

INSTALL(
    FILES
        AUTHORS.rst 
        license.rst
        README.rst
        usercmake/CMakeLists.txt
    DESTINATION share/cyclus/stubs
    COMPONENT stubs
    )

INSTALL(
    FILES
        input/example.xml
        input/README.rst
    DESTINATION share/cyclus/stubs/input
    COMPONENT stubs
    )

INSTALL(
    FILES
        src/CMakeLists.txt
        src/stub_facility.h
        src/stub_facility.cc
        src/stub_facility_tests.cc
        src/stub_inst.h
        src/stub_inst.cc
        src/stub_inst_tests.cc
        src/stub_region.h
        src/stub_region.cc
        src/stub_region_tests.cc
        src/stub_unit_test_driver.cc
    DESTINATION share/cyclus/stubs/src
    COMPONENT stubs
    )
