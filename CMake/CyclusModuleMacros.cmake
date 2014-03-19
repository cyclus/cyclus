macro(cyclus_init  _path _dir _name)

  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}${_path})
  
  IF(NOT DEFINED CYCPP)
    SET(CYCPP "${CYCLUS_CORE_INCLUDE_DIRS}/../cycpp.py")
  ENDIF(NOT DEFINED CYCPP)

  SET(BUILD_DIR ${PROJECT_BINARY_DIR}/${_dir})
  FILE(MAKE_DIRECTORY ${BUILD_DIR})

  GET_PROPERTY(DIRS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)  
  #SET(INCL_ARG "")
  SET(INCL_ARG "$ENV{CPLUS_INCLUDE_PATH}")
  FOREACH(DIR ${DIRS})
    SET(INCL_ARG "${INCL_ARG}:${DIR}")
  ENDFOREACH(DIR ${DIRS})
  # SET(INCL_ARG "-I=\"${INCL_ARG}\"")
  SET(ENV{CPLUS_INCLUDE_PATH} ${INCL_ARG})
  MESSAGE("CPLUS_INCLUDE_PATH: $ENV{CPLUS_INCLUDE_PATH}")
  #SET(INCL_ARG "-I=\"${DIRS}\"")
  #MESSAGE("INCL ARG: ${INCL_ARG}")

  SET(HIN "${CMAKE_CURRENT_SOURCE_DIR}/${_name}.h")
  SET(HOUT "-o=${BUILD_DIR}/${_name}.h")
  IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_name}.h")
    EXECUTE_PROCESS(COMMAND ${CYCPLUS} ${HIN} ${HOUT})# ${INCL_ARG})
    MESSAGE("Executing ${CYCPLUS} ${HIN} ${HOUT}")# ${INCL_ARG}")
  ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_name}.h")

  SET(CCIN "${CMAKE_CURRENT_SOURCE_DIR}/${_name}.cc")
  SET(CCOUT "-o=${BUILD_DIR}/${_name}.cc")
  MESSAGE("Executing ${CYCPP} ${CCIN} ${CCOUT}")# ${INCL_ARG}")
  EXECUTE_PROCESS(COMMAND ${CYCPP} ${CCIN} ${CCOUT}) # ${INCL_ARG})
  
  # Build the cyclus executable from the CYCLUS_SRC source files
  ADD_LIBRARY( ${_dir} ${BUILD_DIR}/${_name}.cc )
  # Link the libraries to libcycluscore
  TARGET_LINK_LIBRARIES(${_dir} dl cycluscore)
  SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} ${_dir} )
  
  install(TARGETS ${_dir}
    LIBRARY DESTINATION lib${_path}
    COMPONENT ${_path}
    )
endmacro()
  
macro(cyclus_init_agent _dir _name)
  SET(MODEL_PATH "/cyclus/${_dir}")
  cyclus_init(${MODEL_PATH} ${_dir} ${_name})

  SET(TestSource ${TestSource} 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}_tests.cc
    PARENT_SCOPE)
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
