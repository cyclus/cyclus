
#################################################################
############# special file configuration ########################
#################################################################

CONFIGURE_FILE(suffix.h.in "${CMAKE_CURRENT_SOURCE_DIR}/suffix.h" @ONLY)

CONFIGURE_FILE(version.cc.in "${CMAKE_CURRENT_SOURCE_DIR}/version.cc" @ONLY)

SET(cyclus_install_dir "${CMAKE_INSTALL_PREFIX}")
SET(cyclus_build_dir "${CYCLUS_BINARY_DIR}")
if(DEFINED DEFAULT_ALLOW_MILPS)
    if (${DEFAULT_ALLOW_MILPS})
        SET(default_allow_milps "true")
    else (${DEFAULT_ALLOW_MILPS})
        SET(default_allow_milps "false")
    endif (${DEFAULT_ALLOW_MILPS})
else(DEFINED DEFAULT_ALLOW_MILPS)
    SET(default_allow_milps "true")
endif(DEFINED DEFAULT_ALLOW_MILPS)
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/env.cc.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/env.cc"
    @ONLY
    )

EXECUTE_PROCESS(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "QUERY" OUTPUT_VARIABLE HDF5_BACK_CC_QUERY RESULT_VARIABLE res_var_q)
IF(NOT "${res_var_q}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'QUERY' failed, result = '${res_var_q}'")
ENDIF()

EXECUTE_PROCESS(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "CREATE" OUTPUT_VARIABLE HDF5_BACK_CC_CREATE RESULT_VARIABLE res_var_c)
IF(NOT "${res_var_c}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'CREATE' failed, result = '${res_var_c}'")
ENDIF()

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.cc.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.cc"
    @ONLY
    )
############################################################
############# end special configuration ####################
############################################################

#################################################################
######### build libcyclus #######################################
#################################################################

INCLUDE_DIRECTORIES(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    )

SET(
    CYCLUS_CORE_INCLUDE_DIRS ${CYCLUS_CORE_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}"
    PARENT_SCOPE
    )

FILE(GLOB cc_files "${CMAKE_CURRENT_SOURCE_DIR}/[^_]*.cc")
SET(
    CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC}
    ${cc_files}
    "${CMAKE_CURRENT_SOURCE_DIR}/OsiCbcSolverInterface.cpp"
    )

set_source_files_properties(pyne_decay.cc PROPERTIES COMPILE_FLAGS "-O0")

# write the include directories to a file for later use
get_property(incdirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
file(WRITE "${CMAKE_BINARY_DIR}/includes.txt" "${incdirs}")

FILE(GLOB h_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
FILE(GLOB hpp_files "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
SET(
    h_files ${h_files} ${hpp_files}
    )

INSTALL(
    FILES ${h_files}
    DESTINATION include/cyclus
    COMPONENT core
    )

ADD_SUBDIRECTORY(toolkit)

ADD_LIBRARY(cyclus ${CYCLUS_CORE_SRC})

SET_TARGET_PROPERTIES(cyclus
    PROPERTIES
    INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
    )

TARGET_LINK_LIBRARIES(cyclus dl ${LIBS})

INSTALL(
    TARGETS cyclus
    LIBRARY DESTINATION lib
    COMPONENT cyclus
    )

SET(LIBS ${LIBS} cyclus PARENT_SCOPE)
#################################################################
######### end build libcyclus ###################################
#################################################################
