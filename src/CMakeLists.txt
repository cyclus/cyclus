#################################################################
############# special file configuration ########################
#################################################################
if(COIN_FOUND)
  set(cyclus_has_coin 1)
else()
  set(cyclus_has_coin 0)
endif()
CONFIGURE_FILE(platform.h.in "${CMAKE_CURRENT_SOURCE_DIR}/platform.h" @ONLY)
CONFIGURE_FILE(version.cc.in "${CMAKE_CURRENT_SOURCE_DIR}/version.cc" @ONLY)
CONFIGURE_FILE(version.h.in "${CMAKE_CURRENT_SOURCE_DIR}/version.h" @ONLY)
CONFIGURE_FILE(cyc_limits.h.in "${CMAKE_CURRENT_SOURCE_DIR}/cyc_limits.h" @ONLY)

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

EXECUTE_PROCESS(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "VAL_TO_BUF_H" OUTPUT_VARIABLE HDF5_BACK_CC_VAL_TO_BUF_H RESULT_VARIABLE res_var_h)
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

EXECUTE_PROCESS(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "QUERY" OUTPUT_VARIABLE HDF5_BACK_CC_QUERY RESULT_VARIABLE res_var_q)
IF(NOT "${res_var_q}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'QUERY' failed, result = '${res_var_q}'")
ENDIF()

EXECUTE_PROCESS(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "CREATE" OUTPUT_VARIABLE HDF5_BACK_CC_CREATE RESULT_VARIABLE res_var_c)
IF(NOT "${res_var_c}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'CREATE' failed, result = '${res_var_c}'")
ENDIF()

EXECUTE_PROCESS(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "VL_DATASET" OUTPUT_VARIABLE HDF5_BACK_CC_VL_DATASET RESULT_VARIABLE res_var_v)
IF(NOT "${res_var_v}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'VL_DATASET' failed, result = '${res_var_v}'")
ENDIF()

EXECUTE_PROCESS(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "FILL_BUF" OUTPUT_VARIABLE HDF5_BACK_CC_FILL_BUF RESULT_VARIABLE res_var_f)
IF(NOT "${res_var_f}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'FILL_BUF' failed, result = '${res_var_f}'")
ENDIF()

EXECUTE_PROCESS(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "WRITE" OUTPUT_VARIABLE HDF5_BACK_CC_WRITE RESULT_VARIABLE res_var_w)
IF(NOT "${res_var_w}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'WRITE' failed, result = '${res_var_w}'")
ENDIF()

EXECUTE_PROCESS(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "VAL_TO_BUF" OUTPUT_VARIABLE HDF5_BACK_CC_VAL_TO_BUF RESULT_VARIABLE res_var_vb)
IF(NOT "${res_var_vb}" STREQUAL "0")
  message(FATAL_ERROR "Process hdf5_back_gen.py 'VAL_TO_BUF' failed, result = '${res_var_vb}'")
ENDIF()

EXECUTE_PROCESS(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_gen.py "BUF_TO_VAL" OUTPUT_VARIABLE HDF5_BACK_CC_BUF_TO_VAL RESULT_VARIABLE res_var_bv)
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
SET(NoCythonLibs ${LIBS})
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

    if( "${Python3_VERSION_MAJOR}" MATCHES "3" )
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
        set(_generated_api "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc_api.h")
        set(_generated_cc "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc.gen")
        set(_h_file "${CMAKE_CURRENT_SOURCE_DIR}/${name}.h")
        set(_api_file "${CMAKE_CURRENT_SOURCE_DIR}/${name}_api.h")
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

        ADD_LIBRARY(${name} ${_cc_file})
        SET(CythonModuleSrc ${CythonModuleSrc} ${_cc_file})
        SET_TARGET_PROPERTIES(${name} PROPERTIES
          OUTPUT_NAME "${name}"
          PREFIX ""
          INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
          INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
          )
        IF (CYTHON_VERSION_MAJOR GREATER_EQUAL 3)
            TARGET_COMPILE_DEFINITIONS(${name} PRIVATE NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
        ENDIF()

        TARGET_LINK_LIBRARIES(${name} dl ${NoCythonLibs})

        INSTALL(
          TARGETS ${name}
          LIBRARY DESTINATION lib
          )
        # Add symlinks to PYTHON_SITE_PACKAGES so python can find our cython modules
        # Python will only import libraries with .so extensions
        # so we hardcode the symlink with .so even on APPLE
        INSTALL(CODE "
            EXECUTE_PROCESS(
                COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${CMAKE_INSTALL_PREFIX}/lib/${name}${suffix}
                ${PYTHON_SITE_PACKAGES}/${name}.so
            )"
          )

        copy_when_diff(${_generated_h} ${_h_file})
        copy_when_diff(${_generated_api} ${_api_file})
        copy_when_diff(${_generated_cc} ${_cc_file})
    endforeach()
endif(Cython_FOUND)
###########################################################
########### end cython configuration ######################
###########################################################

#################################################################
######### build libcyclus #######################################
#################################################################

INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}")

set(CYCLUS_COIN_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/prog_solver.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/coin_helpers.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/prog_translator.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/solver_factory.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/OsiCbcSolverInterface.cpp"
    )

SET(CYCLUS_CORE_INCLUDE_DIRS ${CYCLUS_CORE_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}"
    PARENT_SCOPE
    )
FILE(GLOB cc_files "${CMAKE_CURRENT_SOURCE_DIR}/[^_]*.cc")
foreach(ccfile ${CYCLUS_COIN_SRC})
  list(REMOVE_ITEM cc_files ${ccfile})
endforeach()
list(REMOVE_ITEM cc_files ${CMAKE_CURRENT_SOURCE_DIR}/coin_helpers.cc)
list(REMOVE_ITEM cc_files ${CMAKE_CURRENT_SOURCE_DIR}/prog_translator.cc)
# The cython modules are built as their own shared libraries
# We don't want them to be included in libcyclus.so
if(Cython_FOUND)
  list(REMOVE_ITEM cc_files ${CythonModuleSrc})
endif()
SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} ${cc_files})

# Add Cyclus source files that use COIN, if we have COIN
if(COIN_FOUND)
  SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} ${CYCLUS_COIN_SRC})
endif()

# Add bateman solver from PyNE
#add_definitions(-DPYNE_IS_AMALGAMATED)
download_platform("https://raw.githubusercontent.com/pyne/data/master" "decay"
                  ".cpp" ".s")
fast_compile(decay.cpp "-O0" "-O0" "-O0")
set_source_files_properties(decay.cpp PROPERTIES COMPILE_DEFINITIONS PYNE_IS_AMALGAMATED)

# Add CRAM solver from PyNE
download_platform("http://raw.githubusercontent.com/pyne/data/master" "cram"
                     ".c" ".s")
fast_compile(cram.c
  "-O0 -fcx-fortran-rules -fcx-limited-range -ftree-sra -ftree-ter -fexpensive-optimizations"
  "-O0 -ffast-math"
  "-O0")

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

TARGET_LINK_LIBRARIES(cyclus dl ${LIBS} Python3::Python)

INSTALL(
    TARGETS cyclus
    LIBRARY DESTINATION lib
    COMPONENT cyclus
    )

SET(LIBS ${LIBS} cyclus PARENT_SCOPE)
#################################################################
######### end build libcyclus ###################################
#################################################################
