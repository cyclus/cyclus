if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5")
    message("-- Found '${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5'")
else()
    message("-- Downloading https://raw.githubusercontent.com/cyclus/data/main/cyclus_nuc_data.h5 to "
        "'${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5'...")
    file(DOWNLOAD "https://raw.githubusercontent.com/cyclus/data/main/cyclus_nuc_data.h5"
        "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5" SHOW_PROGRESS)
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

EXECUTE_PROCESS(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/dbtypes_gen.py ${DATA_MODEL_VERSION} OUTPUT_VARIABLE DBTYPES_JSON_ENTRIES RESULT_VARIABLE res_var)
IF(NOT "${res_var}" STREQUAL "0")
  message(FATAL_ERROR "Process dbtypes_gen.py failed, res_var = '${res_var}'")
ENDIF()
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/dbtypes.json.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/dbtypes.json"
    @ONLY
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
    "${CMAKE_CURRENT_SOURCE_DIR}/dbtypes.json"
    DESTINATION share/cyclus
    COMPONENT core
    )

INSTALL(DIRECTORY webapp/ DESTINATION share/cyclus/webapp
        COMPONENT core)
