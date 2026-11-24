if(EXISTS "${CYCLUS_SHARE_BINARY_DIR}/cyclus_nuc_data.h5")
    message("-- Found '${CYCLUS_SHARE_BINARY_DIR}/cyclus_nuc_data.h5'")
else()
    message("-- Downloading https://raw.githubusercontent.com/cyclus/data/main/cyclus_nuc_data.h5 to "
        "'${CYCLUS_SHARE_BINARY_DIR}/cyclus_nuc_data.h5'...")
    # Fails the configure step if the download fails, so that the build does
    # not continue without the data file.
    cyclus_download("https://raw.githubusercontent.com/cyclus/data/main/cyclus_nuc_data.h5"
        "${CYCLUS_SHARE_BINARY_DIR}/cyclus_nuc_data.h5")
    message("-- ...download complete!")
endif()

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_default_unit_test_driver.cc"
    "${CYCLUS_SHARE_BINARY_DIR}/cyclus_default_unit_test_driver.cc"
    COPYONLY
    )

EXECUTE_PROCESS(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/dbtypes_gen.py ${DATA_MODEL_VERSION} OUTPUT_VARIABLE DBTYPES_JSON_ENTRIES RESULT_VARIABLE res_var)
IF(NOT "${res_var}" STREQUAL "0")
  message(FATAL_ERROR "Process dbtypes_gen.py failed, res_var = '${res_var}'")
ENDIF()
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/dbtypes.json.in"
    "${CYCLUS_DBTYPES_JSON}"
    @ONLY
    )

SET(
    CYCLUS_DEFAULT_TEST_DRIVER
    "${CYCLUS_SHARE_BINARY_DIR}/cyclus_default_unit_test_driver.cc"
    PARENT_SCOPE
    )

INSTALL(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_default_unit_test_driver.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus.rng.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus-flat.rng.in"
    "${CYCLUS_SHARE_BINARY_DIR}/cyclus_nuc_data.h5"
    "${CYCLUS_DBTYPES_JSON}"
    DESTINATION share/cyclus
    COMPONENT core
    )

INSTALL(DIRECTORY webapp/ DESTINATION share/cyclus/webapp
        COMPONENT core)
