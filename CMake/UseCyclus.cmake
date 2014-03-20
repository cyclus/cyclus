MACRO(use_cyclus _dir _name)
  MESSAGE(STATUS "Starting construction of build files for agent: ${_dir}")

  # output directory
  SET(AGENT_PATH "/cyclus/${_dir}")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}${AGENT_PATH})
  
  # get preprocessor script
  IF(NOT DEFINED CYCPP)
    SET(CYCPP "${CYCLUS_CORE_INCLUDE_DIRS}/../../bin/cycpp.py")
  ENDIF(NOT DEFINED CYCPP)

  # make a build directory
  SET(BUILD_DIR ${PROJECT_BINARY_DIR}/${_dir})
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
  SET(HIN "${CMAKE_CURRENT_SOURCE_DIR}/${_name}.h")
  SET(HOUT "${BUILD_DIR}/${_name}.h")
  SET(HFLAG "-o=${HOUT}")
  IF(EXISTS "${HIN}")
    IF(NOT EXISTS ${HOUT})
      MESSAGE(STATUS "Executing ${CYCPP} ${HIN} ${PREPROCESSOR} ${HFLAG} ${INCL_ARGS}")
      EXECUTE_PROCESS(COMMAND ${CYCPP} ${HIN} ${PREPROCESSOR} ${HFLAG} ${INCL_ARGS})
    ENDIF(NOT EXISTS ${HOUT})
  ENDIF(EXISTS "${HIN}")

  # process impl
  SET(CCIN "${CMAKE_CURRENT_SOURCE_DIR}/${_name}.cc")
  SET(CCOUT "${BUILD_DIR}/${_name}.cc")
  SET(CCFLAG "-o=${CCOUT}")
  SET(ORIG "--pass3-use-orig")
  IF(NOT EXISTS ${CCOUT})
    MESSAGE(STATUS "Executing ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}")
    EXECUTE_PROCESS(COMMAND ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS})
  ENDIF(NOT EXISTS ${CCOUT})

  # add library
  ADD_LIBRARY(${_dir} ${CCOUT})
  TARGET_LINK_LIBRARIES(${_dir} dl cycluscore)
  SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} ${_dir})
  ADD_DEPENDENCIES(${_dir} ${HIN} ${HOUT} ${CCIN} ${CCOUT})

  IF(EXISTS "${HIN}")
    ADD_CUSTOM_COMMAND(
      OUTPUT ${HOUT}
      COMMAND ${CYCPP} ${HIN} ${PREPROCESSOR} ${HFLAG} ${ORIG} ${INCL_ARGS}
      DEPENDS ${HIN}
      COMMENT "Executing ${CYCPP} ${HIN} ${PREPROCESSOR} ${HFLAG} ${INCL_ARGS}"
      )
    ADD_CUSTOM_COMMAND(
      OUTPUT ${CCOUT}
      COMMAND ${CYCPP} ${HIN} ${PREPROCESSOR} ${HFLAG} ${ORIG} ${INCL_ARGS}
      COMMAND ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}
      DEPENDS ${CCIN}
      DEPENDS ${HIN}
      COMMENT "Executing ${CYCPP} ${HIN} ${PREPROCESSOR} ${HFLAG} ${ORIG} ${INCL_ARGS}"
      COMMENT "Executing ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}"
      )
  ELSE(EXISTS "${HIN}")
    ADD_CUSTOM_COMMAND(
      OUTPUT ${CCOUT}
      COMMAND ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}
      DEPENDS ${CCIN}
      COMMENT "Executing ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCFLAG} ${ORIG} ${INCL_ARGS}"
      )
  ENDIF(EXISTS "${HIN}")

  # install library
  install(TARGETS ${_dir}
    LIBRARY DESTINATION lib${_dir}
    COMPONENT ${_dir}
    )
  
  # install headers
  IF(EXISTS "${HOUT}")
    install(FILES ${HOUT}
      DESTINATION include/cyclus
      COMPONENT ${_dir}
      )
  ENDIF(EXISTS "${HOUT}")

  # add tests
  IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_name}_tests.cc")
    SET(TestSource ${TestSource} 
      ${CMAKE_CURRENT_SOURCE_DIR}/${_name}_tests.cc
      PARENT_SCOPE)
  ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_name}_tests.cc")

  MESSAGE(STATUS "Finished construction of build files for agent: ${_dir}")
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
