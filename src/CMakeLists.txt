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
    if("${COIN_VERSION}" VERSION_GREATER_EQUAL "2.9")
        SET(default_allow_milps "false")
    else("${COIN_VERSION}" VERSION_GREATER_EQUAL "2.9")
        SET(default_allow_milps "true")
    endif("${COIN_VERSION}" VERSION_GREATER_EQUAL "2.9")
endif(DEFINED DEFAULT_ALLOW_MILPS)
MESSAGE("-- default_allow_milps: ${default_allow_milps}")
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/env.cc.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/env.cc"
    @ONLY
    )

EXECUTE_PROCESS(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "VAL_TO_BUF_H" OUTPUT_VARIABLE HDF5_BACK_CC_VAL_TO_BUF_H RESULT_VARIABLE res_var_h)
IF(NOT "${res_var_h}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'VAL_TO_BUF_H' failed, result = '${res_var_h}'")
ENDIF()

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.h.gen"
    @ONLY
    )
copy_when_diff(
    "${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.h.gen"
    "${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.h"
    )

EXECUTE_PROCESS(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "QUERY" OUTPUT_VARIABLE HDF5_BACK_CC_QUERY RESULT_VARIABLE res_var_q)
IF(NOT "${res_var_q}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'QUERY' failed, result = '${res_var_q}'")
ENDIF()

EXECUTE_PROCESS(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "CREATE" OUTPUT_VARIABLE HDF5_BACK_CC_CREATE RESULT_VARIABLE res_var_c)
IF(NOT "${res_var_c}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'CREATE' failed, result = '${res_var_c}'")
ENDIF()

EXECUTE_PROCESS(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "VL_DATASET" OUTPUT_VARIABLE HDF5_BACK_CC_VL_DATASET RESULT_VARIABLE res_var_v)
IF(NOT "${res_var_v}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'VL_DATASET' failed, result = '${res_var_v}'")
ENDIF()

EXECUTE_PROCESS(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "FILL_BUF" OUTPUT_VARIABLE HDF5_BACK_CC_FILL_BUF RESULT_VARIABLE res_var_f)
IF(NOT "${res_var_f}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'FILL_BUF' failed, result = '${res_var_f}'")
ENDIF()

EXECUTE_PROCESS(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "WRITE" OUTPUT_VARIABLE HDF5_BACK_CC_WRITE RESULT_VARIABLE res_var_w)
IF(NOT "${res_var_w}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'WRITE' failed, result = '${res_var_w}'")
ENDIF()

EXECUTE_PROCESS(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "VAL_TO_BUF" OUTPUT_VARIABLE HDF5_BACK_CC_VAL_TO_BUF RESULT_VARIABLE res_var_vb)
IF(NOT "${res_var_vb}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'VAL_TO_BUF' failed, result = '${res_var_vb}'")
ENDIF()

EXECUTE_PROCESS(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "BUF_TO_VAL" OUTPUT_VARIABLE HDF5_BACK_CC_BUF_TO_VAL RESULT_VARIABLE res_var_bv)
IF(NOT "${res_var_bv}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'BUF_TO_VAL' failed, result = '${res_var_bv}'")
ENDIF()

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.cc.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.cc.gen"
    @ONLY
    )
copy_when_diff(
    "${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.cc.gen"
    "${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.cc"
    )
############################################################
############# end special configuration ####################
############################################################

###########################################################
############# cython configuration ########################
###########################################################
if(Cython_FOUND)
    # some setup
    set(cython_include_directories "")
    get_directory_property(cmake_include_directories
                           DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                           INCLUDE_DIRECTORIES)
    list(APPEND cython_include_directories ${cmake_include_directories})
    list(REMOVE_DUPLICATES cython_include_directories)
    set(include_directory_arg "")
    foreach(_include_dir ${cython_include_directories})
        set(include_directory_arg ${include_directory_arg} "-I" "${_include_dir}")
    endforeach()

    if( "${PYTHONLIBS_VERSION_STRING}" MATCHES "^2." )
        set( version_arg "-2" )
    elseif( "${PYTHONLIBS_VERSION_STRING}" MATCHES "^3." )
        set( version_arg "-3" )
    else()
        set( version_arg )
    endif()

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR
       "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
        set( cython_debug_arg "--gdb" )
    endif()

    # OK compile everything
    file(GLOB CYCLUS_CORE_CYTHON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.pyx")
    set_source_files_properties(${CYCLUS_CORE_CYTHON_FILES}
        PROPERTIES CYTHON_IS_CXX TRUE)
    foreach(file ${CYCLUS_CORE_CYTHON_FILES})
        message(STATUS "Cython Compiling ${file}")
        get_filename_component(name ${file} NAME_WE)
        set(_generated_h "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc.h")
        set(_generated_cc "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc.gen")
        set(_h_file "${CMAKE_CURRENT_SOURCE_DIR}/${name}.h")
        set(_cc_file "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc")
        set_source_files_properties(${_cc_file} PROPERTIES GENERATED TRUE)

        EXECUTE_PROCESS(COMMAND ${CYTHON_EXECUTABLE} --cplus
            ${include_directory_arg} ${version_arg}
            ${cython_debug_arg} ${CYTHON_FLAGS}
            --output-file  ${_generated_cc} ${file}
            RESULT_VARIABLE res_var_c)
        IF(NOT "${res_var_c}" STREQUAL "0")
            message(FATAL_ERROR "Cython compilation of ${file} failed!")
        ENDIF()
        copy_when_diff(${_generated_h} ${_h_file})
        copy_when_diff(${_generated_cc} ${_cc_file})
    endforeach()
endif(Cython_FOUND)
###########################################################
########### end cython configuration ######################
###########################################################

#################################################################
######### build libcyclus #######################################
#################################################################

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")

SET(CYCLUS_CORE_INCLUDE_DIRS ${CYCLUS_CORE_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}"
    PARENT_SCOPE
    )
FILE(GLOB cc_files "${CMAKE_CURRENT_SOURCE_DIR}/[^_]*.cc")

SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC}
    ${cc_files}
    "${CMAKE_CURRENT_SOURCE_DIR}/OsiCbcSolverInterface.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/transmute.c"
    )

add_definitions(-DPYNE_DECAY_IS_DUMMY)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # using Clang
  set_source_files_properties(transmute.c PROPERTIES COMPILE_FLAGS "-O0 -ffast-math")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
  set_source_files_properties(transmute.c PROPERTIES COMPILE_FLAGS
    "-O0 -fcx-fortran-rules -fcx-limited-range -ftree-sra -ftree-ter -fexpensive-optimizations"
  )
endif()


# write the include directories to a file for later use
get_property(incdirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
file(WRITE "${CMAKE_BINARY_DIR}/includes.txt" "${incdirs}")

FILE(GLOB h_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
FILE(GLOB hpp_files "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
SET(h_files ${h_files} ${hpp_files})

INSTALL(FILES ${h_files} DESTINATION include/cyclus COMPONENT core)

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
