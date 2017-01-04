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
    )

set_source_files_properties(pyne_decay.cc PROPERTIES COMPILE_FLAGS "-O0")

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
