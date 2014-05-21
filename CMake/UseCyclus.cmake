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
# USE_CYCLUS on all of the agents that will go into this module.  Then this macro
# need only be called once.  For example,
#
#   use_cyclus("agents" "sink")
#   use_cyclus("agents" "source")
#   use_cyclus("agents" "k_facility")
#   use_cyclus("agents" "prey")
#   use_cyclus("agents" "predator")
#   install_cyclus_module("agents" "")
#
# Signtaures:
#   use_cyclus(lib_root src_root)
#   install_cyclus_standalone(lib_root src_root lib_dir)
#   install_cyclus_module(lib_root lib_dir)
#
# Arguments:
#   lib_root : the root library name, e.g., MyAgent
#   src_root : the root name of source files, e.g., my_agent for my_agent.h 
#              and my_agent.cc
#   lib_dir : the directory to install the module or agent into.
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
# <lib_root>_LIB      : the name of the library target
# <lib_root>_TEST_LIB : the name of the test library target, if test source 
#                       exists
#
MACRO(USE_CYCLUS lib_root src_root)
  MESSAGE(STATUS "Starting construction of build files for agent: ${lib_root}")

  # output directory
  SET(AGENT_PATH "cyclus/${lib_root}")
  SET(
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${AGENT_PATH}
    )
  
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
  IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET(PREPROCESSOR "--cpp-path=clang++")
  ELSE()
    SET(PREPROCESSOR "--cpp-path=cpp")
  ENDIF()

  # process header
  SET(ORIG "--pass3-use-orig")
  SET(HIN "${CMAKE_CURRENT_SOURCE_DIR}/${src_root}.h")
  SET(HOUT "${BUILD_DIR}/${src_root}.h")
  SET(HFLAG "-o=${HOUT}")
  IF(EXISTS "${HIN}")
    IF(NOT EXISTS ${HOUT})
      MESSAGE(STATUS "Executing ${CYCPP} ${HIN} ${PREPROCESSOR} ${HFLAG} ${ORIG} ${INCL_ARGS}")
      EXECUTE_PROCESS(COMMAND ${CYCPP} ${HIN} ${PREPROCESSOR} ${HFLAG} ${ORIG} ${INCL_ARGS})
    ENDIF(NOT EXISTS ${HOUT})
    SET(
      "${lib_root}_H" 
      "${${lib_root}_H}" "${HOUT}"
      CACHE INTERNAL "Agent header" FORCE
      )
  ENDIF(EXISTS "${HIN}")

  # process impl
  SET(CCIN "${CMAKE_CURRENT_SOURCE_DIR}/${src_root}.cc")
  SET(CCOUT "${BUILD_DIR}/${src_root}.cc")
  SET(CCFLAG "-o=${CCOUT}")
  IF(NOT EXISTS ${CCOUT})
    MESSAGE(STATUS "Executing ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}")
    EXECUTE_PROCESS(COMMAND ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS})
  ENDIF(NOT EXISTS ${CCOUT})

  SET("${lib_root}_CC" "${${lib_root}_CC}" "${CCOUT}" CACHE INTERNAL "Agent source" FORCE)
ENDMACRO()

MACRO(INSTALL_CYCLUS_STANDALONE lib_root src_root lib_dir)
  # clear variables before starting
  SET("${lib_root}_H" "" CACHE INTERNAL "Agent header" FORCE)
  SET("${lib_root}_CC" "" CACHE INTERNAL "Agent source" FORCE)
  SET("${lib_root}_LIB" "" CACHE INTERNAL "Agent library alias." FORCE)
  SET("${lib_root}_Test_H" "" CACHE INTERNAL "Agent test headers" FORCE)
  SET("${lib_root}_TEST_CC" "" CACHE INTERNAL "Agent test source" FORCE)
  SET("${lib_root}_TEST_LIB" "" CACHE INTERNAL "Agent test library alias." FORCE)

  # setup
  USE_CYCLUS("${lib_root}" "${src_root}")

  # add library
  ADD_LIBRARY(${lib_root} ${CCOUT})
  TARGET_LINK_LIBRARIES(${lib_root} dl ${LIBS})
  SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} ${lib_root})
  ADD_DEPENDENCIES(${lib_root} ${HIN} ${HOUT} ${CCIN} ${CCOUT})

  IF(EXISTS "${HIN}")
    ADD_CUSTOM_COMMAND(
      OUTPUT ${CCOUT} 
      OUTPUT ${HOUT}
      COMMAND ${CYCPP} ${HIN} ${PREPROCESSOR} ${HFLAG} ${ORIG} ${INCL_ARGS}
      COMMAND ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}
      DEPENDS ${HIN}
      DEPENDS ${CCIN}
      DEPENDS ${CYCPP}
      COMMENT "Executing ${CYCPP} ${HIN} ${PREPROCESSOR} ${HFLAG} ${ORIG} ${INCL_ARGS}"
      COMMENT "Executing ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}"
      )
  ELSE(EXISTS "${HIN}")
    ADD_CUSTOM_COMMAND(
      OUTPUT ${CCOUT}
      COMMAND ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}
      DEPENDS ${CCIN}
      DEPENDS ${CYCPP}
      COMMENT "Executing ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}"
      )
  ENDIF(EXISTS "${HIN}")

  # install library
  install(
    TARGETS ${lib_root}
    LIBRARY DESTINATION lib/cyclus/${lib_dir}
    COMPONENT ${lib_root}
    )
  SET(
    "${lib_root}_LIB" 
    "${lib_root}" 
    CACHE INTERNAL "Agent library alias." FORCE
    )
  
  # install headers
  IF(EXISTS "${HOUT}")
    INSTALL(
      FILES ${HOUT}
      DESTINATION include/cyclus
      COMPONENT ${lib_root}
      )
  ENDIF(EXISTS "${HOUT}")

  # add tests
  SET(CCTIN "${CMAKE_CURRENT_SOURCE_DIR}/${src_root}_tests.cc")
  SET(CCTOUT "${BUILD_DIR}/${src_root}_tests.cc")
  SET(HTIN "${CMAKE_CURRENT_SOURCE_DIR}/${src_root}_tests.h")
  SET(HTOUT "${BUILD_DIR}/${src_root}_tests.h")
  SET(CMD "cp")
  IF(EXISTS "${CCTIN}")
    IF(EXISTS "${HTIN}")
      # install test headers
      MESSAGE(STATUS "Copying ${HTIN} to ${HTOUT}.")
      EXECUTE_PROCESS(COMMAND ${CMD} ${HTIN} ${HTOUT})
      ADD_CUSTOM_COMMAND(
      	OUTPUT ${HTOUT}
      	OUTPUT ${CCTOUT}
	COMMAND ${CMD} ${HTIN} ${HTOUT}
	COMMAND ${CMD} ${CCTIN} ${CCTOUT}
	DEPENDS ${HIN}
	DEPENDS ${CCIN}
	DEPENDS ${HTIN}
	DEPENDS ${CCTIN}
	COMMENT "Copying ${HTIN} to ${HTOUT}."
	COMMENT "Copying ${CCTIN} to ${CCTOUT}."
	)
      SET(
    "${lib_root}_Test_H" 
    "${HTOUT}"
    CACHE INTERNAL "Agent test headers" FORCE
    )
    ENDIF(EXISTS "${HTIN}")

    # install test impl
    MESSAGE(STATUS "Copying ${CCTIN} to ${CCTOUT}.")
    EXECUTE_PROCESS(COMMAND ${CMD} ${CCTIN} ${CCTOUT})
    ADD_CUSTOM_COMMAND(
      OUTPUT ${CCTOUT}
      COMMAND ${CMD} ${CCTIN} ${CCTOUT}
      DEPENDS ${CCTIN} 
      DEPENDS ${CCIN}
      COMMENT "Copying ${CCTIN} to ${CCTOUT}."
      )
    SET(
      "${lib_root}_TEST_CC" 
      "${CCOUT}"
      "${CCTOUT}"
      CACHE INTERNAL "Agent test source" FORCE
      )

    ADD_LIBRARY(${lib_root}Tests ${${lib_root}_TEST_CC})
    TARGET_LINK_LIBRARIES(${lib_root}Tests dl ${LIBS} ${CYCLUS_GTEST_LIBRARIES})
    SET_TARGET_PROPERTIES(${lib_root}Tests PROPERTIES LINKER_LANGUAGE CXX)
    SET(
      "${lib_root}_TEST_LIB"
      "${lib_root}Tests"
      CACHE INTERNAL "Agent test library alias." FORCE
      )

  ENDIF(EXISTS "${CCTIN}")

  MESSAGE(STATUS "Finished construction of build files for agent: ${lib_root}")
ENDMACRO()

MACRO(INSTALL_CYCLUS_MODULE lib_root lib_dir)
  # add library
  ADD_LIBRARY("${lib_root}" ${${lib_root}_CC})
  TARGET_LINK_LIBRARIES(${lib_root} dl ${LIBS})
  SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} ${lib_root})
  ADD_DEPENDENCIES(${lib_root} "${${lib_root}_H}" "${${lib_root}_CC}")

  # install library
  install(TARGETS ${lib_root} LIBRARY DESTINATION lib/cyclus/${lib_dir} COMPONENT ${lib_root})
  SET("${lib_root}_LIB" "${lib_root}" CACHE INTERNAL "Agent library alias." FORCE)
  
  # install headers
  SET(HOUT "${lib_root}.h")
  IF(EXISTS "${HOUT}")
    INSTALL(FILES ${HOUT} DESTINATION include/cyclus COMPONENT ${lib_root})
  ENDIF(EXISTS "${HOUT}")

  # clear variables before returning
  SET("${lib_root}_H" "" CACHE INTERNAL "Agent header" FORCE)
  SET("${lib_root}_CC" "" CACHE INTERNAL "Agent source" FORCE)
  SET("${lib_root}_Test_H" "" CACHE INTERNAL "Agent test headers" FORCE)
  SET("${lib_root}_TEST_CC" "" CACHE INTERNAL "Agent test source" FORCE)
  SET("${lib_root}_TEST_LIB" "" CACHE INTERNAL "Agent test library alias." FORCE)
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
