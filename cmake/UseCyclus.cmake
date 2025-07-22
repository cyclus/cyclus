#
# The USE_CYCLUS, INSTALL_CYCLUS_STANDALONE, INSTALL_CYCLUS_MODULE macros builds
# agent libraries for Cyclus given some source files.
#
# INSTALL_CYCLUS_STANDALONE is meant to build a single agent into its own
# module.  It implicitly calls USE_CYCLUS.  For example,
#
#   install_cyclus_standalone("TestFacility" "test_facility" "tests")
#
# INSTALL_CYCLUS_MODULE meanwhile is meant to be able to build many agents into
# the same module.  To do this the environment must first be prepared with
# USE_CYCLUS on all of the agents that will go into this module.  Then this
# macro need only be called once.  For example,
#
#   use_cyclus("agents" "sink")
#   use_cyclus("agents" "source")
#   use_cyclus("agents" "k_facility")
#   use_cyclus("agents" "prey")
#   use_cyclus("agents" "predator")
#   install_cyclus_module("agents" "")
#
# If test files (named *_tests.[h|cc]) are present, a unit test executable will
# be automatically generated. A custom test driver (i.e., a source file that
# contains a main() function for gtest to run) can optionally be provided to the
# INSTALL_CYCLUS_STANDALONE or INSTALL_CYCLUS_MODULE macros. If the driver
# supplied is NONE, then a test executable will *NOT* be created.
#
# Signtaures:
#   use_cyclus(lib_root src_root)
#   install_cyclus_standalone(lib_root src_root lib_dir [test_driver])
#   install_cyclus_module(lib_root lib_dir [test_driver])
#
# Arguments:
#   lib_root : the root library name, e.g., MyAgent
#   src_root : the root name of source files, e.g., my_agent for my_agent.h
#              and my_agent.cc
#   lib_dir : the directory to install the module or agent into.
#   test_driver : (optional) the custom test driver to use with unit tests,
#                 or NONE
#
# The CYCLUS_CUSTOM_HEADERS variable can optionally be set to contain one or
# more (space separated) header files before calling the USE_CYCLUS
# macro to add shared headers used by the archetype library being built.
#
# The following vars are updated.
#
# CYCLUS_LIBRARIES   : updated to include <lib_root>_LIB
#
# The following vars are set.
#
# <lib_root>_H       : the headers used for the agent
# <lib_root>_CC      : the srcs used for the agent
# <lib_root>_TEST_H  : the headers used for the agent tests, if it exists
# <lib_root>_TEST_CC : the srcs used for the agent tests, if it exists
#
# Target names that are valid:
#
# <lib_root>_LIB        : the name of the library target
# <lib_root>_unit_tests : the name of the unit test executable, if test source
#                         exists
#
MACRO(USE_CYCLUS lib_root src_root)
    MESSAGE(STATUS "Starting construction of build files for agent: ${src_root}")

    # output directory
    SET(AGENT_PATH "cyclus/${lib_root}")
    IF(NOT "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" MATCHES
            ".*${AGENT_PATH}$")
        SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${AGENT_PATH})
    ENDIF(NOT "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" MATCHES
        ".*${AGENT_PATH}$")

    # get preprocessor script
    IF(NOT DEFINED CYCPP)
        SET(CYCPP "${CYCLUS_CORE_INCLUDE_DIRS}/../../bin/cycpp.py")
    ENDIF(NOT DEFINED CYCPP)

    # make a build directory
    SET(BUILD_DIR ${PROJECT_BINARY_DIR}/${lib_root})
    FILE(MAKE_DIRECTORY ${BUILD_DIR})

    # collect include directories argument
    GET_PROPERTY(DIRS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    SET(INCL_ARGS "-I=")
    FOREACH(DIR ${DIRS})
        SET(INCL_ARGS "${INCL_ARGS}:${DIR}")
    ENDFOREACH(DIR ${DIRS})


    # set cpp path
    # Now check first if the $CPP enrironment variable is defined, if not
    # fallback on the previous behavior, so in order:
    #   1- if defined, uses ${CPP}
    #   2- if clang++ is present, uses it
    #   3- otherwise, uses cpp
    IF(DEFINED ENV{CPP})
        SET(SYS_CPP "$ENV{CPP}")
    ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        SET(SYS_CPP "clang++")
    ELSE()
        SET(SYS_CPP "cpp")
    ENDIF()
    SET(PREPROCESSOR "--cpp-path=${SYS_CPP}")

    # copy custom headers
    FOREACH(fname ${CYCLUS_CUSTOM_HEADERS})
        SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${fname}")
        SET(dst "${BUILD_DIR}/${fname}")
        MESSAGE(STATUS "Copying ${src} to ${dst}.")
        EXECUTE_PROCESS(COMMAND "cp" "${src}" "${dst}")
        ADD_CUSTOM_COMMAND(
            OUTPUT "${dst}"
            COMMAND "cp" "${src}" "${dst}"
            DEPENDS "${src}"
            COMMENT "Copying ${src} to ${dst}."
            )
    ENDFOREACH()
    MESSAGE("dstcustomheaders: ${CYCLUS_CUSTOM_HEADERS}")

    # process header
    SET(ORIG "--pass3-use-orig")
    SET(HIN "${CMAKE_CURRENT_SOURCE_DIR}/${src_root}.h")
    SET(HOUT "${BUILD_DIR}/${src_root}.h")
    SET(HFLAG "-o=${HOUT}")
    SET(CCIN "${CMAKE_CURRENT_SOURCE_DIR}/${src_root}.cc")
    SET(CCOUT "${BUILD_DIR}/${src_root}.cc")
    SET(CCFLAG "-o=${CCOUT}")

    # do all processing for CC file - always needed
    IF(NOT EXISTS ${CCOUT})
        PREPROCESS_CYCLUS_FILE_(${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS})
    ENDIF(NOT EXISTS ${CCOUT})
    SET(
        "${lib_root}_CC"
        "${${lib_root}_CC}" "${CCOUT}"
        CACHE INTERNAL "Agent impl" FORCE
        )

    # check for existing of header file
    IF(EXISTS "${HIN}")
        # Do all processing for header file
        IF(NOT EXISTS ${HOUT})
            PREPROCESS_CYCLUS_FILE_( ${CYCPP} ${HIN} ${PREPROCESSOR} ${HFLAG} ${ORIG} ${INCL_ARGS})
        ENDIF(NOT EXISTS ${HOUT})
        SET(
            "${lib_root}_H"
            "${${lib_root}_H}" "${HOUT}"
            CACHE INTERNAL "Agent header" FORCE
            )

        # make custom Makefile target for CC and H file together for joint dependency
        ADD_CUSTOM_COMMAND(
            OUTPUT ${CCOUT}
            OUTPUT ${HOUT}
            COMMAND ${CYCPP} ${HIN} ${PREPROCESSOR} ${HFLAG} ${ORIG} ${INCL_ARGS}
            COMMAND ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}
            DEPENDS ${HIN}
            DEPENDS ${CCIN}
            DEPENDS ${CYCPP}
            DEPENDS ${CYCLUS_CUSTOM_HEADERS}
            COMMENT "Executing ${CYCPP} ${HIN} ${PREPROCESSOR} ${HFLAG} ${ORIG} ${INCL_ARGS}"
            COMMENT "Executing ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}"
            )
        SET(DEP_LIST ${DEP_LIST} ${CCOUT} ${HOUT})
    ELSE(EXISTS "${HIN}")
        # Make custom Makefile target for CC file alone if ho header
        ADD_CUSTOM_COMMAND(
            OUTPUT ${CCOUT}
            COMMAND ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}
            DEPENDS ${CCIN}
            DEPENDS ${CYCPP}
            DEPENDS ${CYCLUS_CUSTOM_HEADERS}
            COMMENT "Executing ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}"
            )
        SET(DEP_LIST ${CCOUT})
    ENDIF(EXISTS "${HIN}")

    # add tests
    SET(CCTIN "${CMAKE_CURRENT_SOURCE_DIR}/${src_root}_tests.cc")
    SET(CCTOUT "${BUILD_DIR}/${src_root}_tests.cc")
    SET(HTIN "${CMAKE_CURRENT_SOURCE_DIR}/${src_root}_tests.h")
    SET(HTOUT "${BUILD_DIR}/${src_root}_tests.h")
    SET(CMD "cp")

    IF(EXISTS "${CCTIN}")
        MESSAGE(STATUS "Copying ${CCTIN} to ${CCTOUT}.")
        EXECUTE_PROCESS(COMMAND ${CMD} ${CCTIN} ${CCTOUT})    
        SET("${lib_root}_TEST_CC" "${${lib_root}_TEST_CC}" "${CCTOUT}"
            CACHE INTERNAL "Agent test source" FORCE)

        IF(EXISTS "${HTIN}")
            # install test headers
            MESSAGE(STATUS "Copying ${HTIN} to ${HTOUT}.")
            EXECUTE_PROCESS(COMMAND ${CMD} ${HTIN} ${HTOUT})
            SET("${lib_root}_TEST_H" "${${lib_root}_TEST_H}" "${HTOUT}"
                CACHE INTERNAL "Agent test headers" FORCE)
            # Create custom Makefile target for CC and H file together for joint dependency
            ADD_CUSTOM_COMMAND(
                OUTPUT ${HTOUT}
                OUTPUT ${CCTOUT}
                COMMAND ${CMD} ${HTIN} ${HTOUT}
                COMMAND ${CMD} ${CCTIN} ${CCTOUT}
                DEPENDS ${HIN}
                DEPENDS ${CCIN}
                DEPENDS ${HTIN}
                DEPENDS ${CCTIN}
                DEPENDS ${CYCLUS_CUSTOM_HEADERS}
                COMMENT "Copying ${HTIN} to ${HTOUT}."
                COMMENT "Copying ${CCTIN} to ${CCTOUT}."
                )
            SET(DEP_LIST ${DEP_LIST} ${HTOUT} ${CCTOUT})
        ELSE(EXISTS "${HTIN}")
            # create custom Makefile target for CC only
            ADD_CUSTOM_COMMAND(
                OUTPUT ${CCTOUT}
                COMMAND ${CMD} ${CCTIN} ${CCTOUT}
                DEPENDS ${CCTIN}
                DEPENDS ${CCIN}
                DEPENDS ${CYCLUS_CUSTOM_HEADERS}
                COMMENT "Copying ${CCTIN} to ${CCTOUT}."
                )
            SET(DEP_LIST ${DEP_LIST} ${CCTOUT})
        ENDIF(EXISTS "${HTIN}")
    ENDIF(EXISTS "${CCTIN}")

    MESSAGE(STATUS "Finished construction of build files for agent: ${src_root}")
ENDMACRO()

MACRO(PREPROCESS_CYCLUS_FILE_ cycpp filein preproc flags orig incl_args)
    MESSAGE(STATUS "Executing ${cycpp} ${filein} ${preproc} ${flags} ${orig} ${incl_args}")
    EXECUTE_PROCESS(COMMAND ${cycpp} ${filein} ${PREPROCESSOR} ${flags}
                    ${orig} ${incl_args} RESULT_VARIABLE res_var)
    IF(NOT "${res_var}" STREQUAL "0")
        message(FATAL_ERROR "${cycpp} failed on '${filein}' with exit code '${res_var}'")
    ENDIF()

ENDMACRO()

MACRO(INSTALL_CYCLUS_STANDALONE lib_root src_root lib_dir)
    # clear variables before starting
    SET("${lib_root}_H" "" CACHE INTERNAL "Agent header" FORCE)
    SET("${lib_root}_CC" "" CACHE INTERNAL "Agent source" FORCE)
    SET("${lib_root}_LIB" "" CACHE INTERNAL "Agent library alias." FORCE)
    SET("${lib_root}_TEST_H" "" CACHE INTERNAL "Agent test headers" FORCE)
    SET("${lib_root}_TEST_CC" "" CACHE INTERNAL "Agent test source" FORCE)
    SET("${lib_root}_TEST_LIB" "" CACHE INTERNAL "Agent test library alias." FORCE)

    # check if a test driver was provided, otherwise use the default
    IF(${ARGC} GREATER 3 AND NOT "${ARGV4}" STREQUAL "")
        SET(DRIVER "${ARGV4}")
    ELSE(${ARGC} GREATER 3  AND NOT "${ARGV4}" STREQUAL "")
        SET(DRIVER "${CYCLUS_DEFAULT_TEST_DRIVER}")
    ENDIF(${ARGC} GREATER 3  AND NOT "${ARGV4}" STREQUAL "")

    USE_CYCLUS("${lib_root}" "${src_root}")
    INSTALL_CYCLUS_MODULE("${lib_root}" "${lib_dir}" ${DRIVER})
ENDMACRO()

MACRO(INSTALL_CYCLUS_MODULE lib_root lib_dir)
    SET(LIB_NAME "${lib_root}")
    SET(LIB_SRC "${${lib_root}_CC}")
    SET(LIB_H "${${lib_root}_H}")
    SET(TEST_SRC "${${lib_root}_TEST_CC}")
    SET(TEST_H "${${lib_root}_TEST_H}")
    SET(INST_DIR "${lib_dir}")

    # check if a test driver was provided, otherwise use the default
    IF(${ARGC} GREATER 2 AND NOT "${ARGV2}" STREQUAL "")
        SET(DRIVER "${ARGV2}")
    ELSE(${ARGC} GREATER 2 AND NOT "${ARGV2}" STREQUAL "")
        SET(DRIVER "${CYCLUS_DEFAULT_TEST_DRIVER}")
    ENDIF(${ARGC} GREATER 2 AND NOT "${ARGV2}" STREQUAL "")

    INSTALL_AGENT_LIB_("${LIB_NAME}" "${LIB_SRC}" "${LIB_H}" "${INST_DIR}")
    INSTALL_AGENT_TESTS_("${LIB_NAME}" "${TEST_SRC}" "${TEST_H}" "${DRIVER}" "${INST_DIR}")
ENDMACRO()

MACRO(INSTALL_AGENT_LIB_ lib_name lib_src lib_h inst_dir)

    ADD_CUSTOM_TARGET(${lib_name}-sources DEPENDS ${lib_src} ${lib_h})

    # add lib
    ADD_LIBRARY(${lib_name} ${lib_src})
    TARGET_LINK_LIBRARIES(${lib_name} dl ${LIBS})
    SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} ${lib_root})
    ADD_DEPENDENCIES(${lib_name} ${lib_name}-sources)

    set(dest_ "lib/cyclus")
    string(COMPARE EQUAL "${inst_dir}" "" is_empty)
    if (NOT is_empty)
      set(dest_ "${dest_}/${inst_dir}")
    endif()

    SET_TARGET_PROPERTIES(${lib_name}
      PROPERTIES INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${dest_}"
    )

    # install library
    INSTALL(
        TARGETS ${lib_name}
        LIBRARY DESTINATION "${dest_}"
        COMPONENT ${lib_name}
        )
    SET(${lib_name}_LIB ${lib_name} CACHE INTERNAL "Agent library alias." FORCE)

ENDMACRO()

MACRO(INSTALL_AGENT_TESTS_ lib_name test_src test_h driver inst_dir)

    # build & install test impl
    IF(NOT "${test_src}" STREQUAL "" AND NOT "${driver}" STREQUAL "NONE")
        SET(TGT ${lib_name}_unit_tests)
        MESSAGE(STATUS "Building agent unit test binary: ${TGT}")
        MESSAGE(STATUS "Using source: ${test_src}")
        MESSAGE(STATUS "And test driver: ${driver}")
        ADD_EXECUTABLE(
            ${TGT}
            ${driver}
            ${test_src}
            )
        TARGET_LINK_LIBRARIES(
            ${TGT} dl
            ${LIBS}
            ${lib_name}
            ${CYCLUS_TEST_LIBRARIES}
            )
        INSTALL(
            TARGETS ${TGT}
            RUNTIME DESTINATION bin
            COMPONENT ${lib_name}_testing
            )
    ENDIF(NOT "${test_src}" STREQUAL "" AND NOT "${driver}" STREQUAL "NONE")
ENDMACRO()



macro(add_all_subdirs)
    file(GLOB all_valid_subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*/CMakeLists.txt")

    foreach(dir ${all_valid_subdirs})
        if(${dir} MATCHES "^([^/]*)//CMakeLists.txt")
            string(REGEX REPLACE "^([^/]*)//CMakeLists.txt" "\\1" dir_trimmed ${dir})
            add_subdirectory(${dir_trimmed})
        endif()
    endforeach(dir)
endmacro()


# sets a unique platform string
macro(cyclus_platform)
  if(NOT DEFINED CYCLUS_PLATFORM)
    # first set OS
    if (WIN32)
      set(_plat "win")
    elseif(APPLE)
      set(_plat "apple")
    else()
      set(_plat "linux")
    endif()
    # next set compiler
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
      set(_plat "${_plat}-gnu")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
      set(_plat "${_plat}-clang")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
      set(_plat "${_plat}-clang")
    else()
      set(_plat "${_plat}-NOTFOUND")
    endif()
    # then set architecture
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
      set(_plat "${_plat}-arm64")
    endif()
    set(CYCLUS_PLATFORM "${_plat}")
  endif()
endmacro()


macro(cyclus_set_fast_compile)
  if(NOT DEFINED CYCLUS_FAST_COMPILE)
    set(CYCLUS_FAST_COMPILE TRUE)
  endif()
  message(STATUS "CYCLUS Fast Compile: ${CYCLUS_FAST_COMPILE}")
endmacro()


# fast compile with assembly, if available.
macro(fast_compile _srcname _gnuflags _clangflags _otherflags)
  # get the assembly file name
  get_filename_component(_base "${_srcname}" NAME_WE)  # get the base name, without the extension
  if (CYCLUS_PLATFORM)
    set(_asmname "${_base}-${CYCLUS_PLATFORM}.s")
  else()
    set(_asmname "${_base}-NOTFOUND")
  endif()

  # pick the filename to compile, either source or assembly
  if(NOT CYCLUS_FAST_COMPILE)
    message(STATUS "Not fast compiling ${_srcname} since PyNE fast compile is disabled.")
    set(_filename "${_srcname}")
  elseif(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_asmname}")
    message(STATUS "Not fast compiling ${_srcname} since ${CMAKE_CURRENT_SOURCE_DIR}/${_asmname} does not exist.")
    set(_filename "${_srcname}")
  else()
    message(STATUS "Compiling ${_srcname} fast from assembly ${_asmname}")
    set(_filename "${_asmname}")
  endif()
  set(CYCLUS_CORE_SRC "${_filename}" ${CYCLUS_CORE_SRC})

  # set some compile flags for the selected file
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set_source_files_properties("${_filename}" PROPERTIES COMPILE_FLAGS "${_gnuflags}")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set_source_files_properties("${_filename}" PROPERTIES COMPILE_FLAGS "${_clangflags}")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set_source_files_properties("${_filename}" PROPERTIES COMPILE_FLAGS "${_clangflags}")
  else()
    set_source_files_properties("${_filename}" PROPERTIES COMPILE_FLAGS "${_otherflags}")
  endif()
endmacro()

