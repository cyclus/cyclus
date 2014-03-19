macro(use_cyclus _dir _name)
  MESSAGE(STATUS "Starting construction of build files for agent: ${_dir}")

  # output directory
  SET(AGENT_PATH "/cyclus/${_dir}")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}${AGENT_PATH})
  
  # get preprocessor script
  IF(NOT DEFINED CYCPP)
    SET(CYCPP "${CYCLUS_CORE_INCLUDE_DIRS}/../cycpp.py")
  ENDIF(NOT DEFINED CYCPP)

  # make a build directory
  SET(BUILD_DIR ${PROJECT_BINARY_DIR}/${_dir})
  FILE(MAKE_DIRECTORY ${BUILD_DIR})

  # set CPLUS_INCLUDE_PATH to include our include paths
  GET_PROPERTY(DIRS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)  
  SET(INCL_ARG "$ENV{CPLUS_INCLUDE_PATH}")
  FOREACH(DIR ${DIRS})
    SET(INCL_ARG "${INCL_ARG}:${DIR}")
  ENDFOREACH(DIR ${DIRS})
  SET(ENV{CPLUS_INCLUDE_PATH} ${INCL_ARG})

  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET(PREPROCESSOR "-cpp-path clang++")
  else()
    SET(PREPROCESSOR "-cpp-path cpp")
  endif()

  # process header
  SET(HIN "${CMAKE_CURRENT_SOURCE_DIR}/${_name}.h")
  SET(HOUT "-o=${BUILD_DIR}/${_name}.h")
  IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_name}.h")
    EXECUTE_PROCESS(COMMAND ${HIN} ${CYCPLUS} ${PREPROCESSOR} ${HOUT})
    MESSAGE(STATUS "Executing ${HIN} ${CYCPLUS} ${PREPROCESSOR} ${HOUT}")
  ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_name}.h")

  # process impl
  SET(CCIN "${CMAKE_CURRENT_SOURCE_DIR}/${_name}.cc")
  SET(CCOUT "-o=${BUILD_DIR}/${_name}.cc")
  SET(ORIG "--pass3-use-orig")
  MESSAGE(STATUS "Executing ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCOUT} ${ORIG}")
  EXECUTE_PROCESS(COMMAND ${CYCPP} ${CCIN} ${PREPROCESSOR} ${CCOUT} ${ORIG})

  # add library
  ADD_LIBRARY(${_dir} ${BUILD_DIR}/${_name}.cc)
  TARGET_LINK_LIBRARIES(${_dir} dl cycluscore)
  SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} ${_dir})

  # install library
  install(TARGETS ${_dir}
    LIBRARY DESTINATION lib${_dir}
    COMPONENT ${_dir}
    )
  
  # install headers
  IF(EXISTS "${BUILD_DIR}/${_name}.h")
    install(FILES ${BUILD_DIR}/${_name}.h
      DESTINATION include/cyclus
      COMPONENT ${_dir}
      )
  ENDIF(EXISTS "${BUILD_DIR}/${_name}.h")

  # add tests
  IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_name}_tests.cc")
    SET(TestSource ${TestSource} 
      ${CMAKE_CURRENT_SOURCE_DIR}/${_name}_tests.cc
      PARENT_SCOPE)
  ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_name}_tests.cc")

  MESSAGE(STATUS "Finished construction of build files for agent: ${_dir}")
endmacro()

macro(add_all_subdirs)
  file(GLOB all_valid_subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*/CMakeLists.txt")
  
  foreach(dir ${all_valid_subdirs})
      if(${dir} MATCHES "^([^/]*)//CMakeLists.txt")
          string(REGEX REPLACE "^([^/]*)//CMakeLists.txt" "\\1" dir_trimmed ${dir})
          add_subdirectory(${dir_trimmed})
      endif()
  endforeach(dir)
endmacro()
