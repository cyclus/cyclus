if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5")
    message("-- Found '${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5'")
else()
    message("-- Downloading http://regtests.fuelcycle.org/cyclus_nuc_data.h5 to "
        "'${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5'...")
    file(DOWNLOAD "http://regtests.fuelcycle.org/cyclus_nuc_data.h5"
        "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5")
    message("-- ...download complete!")
endif()

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5"
    "${CYCLUS_BINARY_DIR}/share/cyclus/cyclus_nuc_data.h5"
    COPYONLY
    )

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_default_unit_test_driver.cc"
    "${CYCLUS_BINARY_DIR}/share/cyclus/cyclus_default_unit_test_driver.cc"
    COPYONLY
    )

SET(
    CYCLUS_DEFAULT_TEST_DRIVER
    "${CYCLUS_BINARY_DIR}/share/cyclus/cyclus_default_unit_test_driver.cc"
    PARENT_SCOPE
    )

INSTALL(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_default_unit_test_driver.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus.rng.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus-flat.rng.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5"
    DESTINATION share/cyclus
    COMPONENT core
    )
