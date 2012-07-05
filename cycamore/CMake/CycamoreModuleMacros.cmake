macro(cyclus_init  _path _name)

  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}${_path})

  # Build the libraries from the CYCAMORE_SRC source files
  ADD_LIBRARY( ${_name}       ${_name}.cpp )
  # Link the libraries to libcycluscore 
  TARGET_LINK_LIBRARIES(${_name} dl ${LIBS})
  SET(CYCAMORE_LIBRARIES ${CYCAMORE_LIBRARIES} ${_name} )

  
  CONFIGURE_FILE(
    ${CYCAMORE_SOURCE_DIR}${_path}/${_name}.rng
    ${PROJECT_BINARY_DIR}/lib${_path}/${_name}.rng
    COPYONLY
    )
  
  install(TARGETS ${_name}
    LIBRARY DESTINATION cyclus/lib${_path}
    COMPONENT ${_path}
    )
  
  SET(RNG_INCLUDES ${RNG_INCLUDES}
    "<include href='../lib${_path}/${_name}.rng'/>"
    PARENT_SCOPE)

  install(FILES "${_name}.rng"
    DESTINATION cyclus/lib${_path}
    COMPONENT "${_path}.rng"
    )
endmacro()
  

macro(cyclus_init_model _type _name)
  SET(MODEL_PATH "/Models/${_type}/${_name}")
  cyclus_init(${MODEL_PATH} ${_name})

  SET(${_type}TestSource ${${_type}TestSource} 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}Tests.cpp 
    PARENT_SCOPE)

  SET(${_type}_REFS ${${_type}_REFS}
    "<ref name='${_name}'/>"
    PARENT_SCOPE)
endmacro()

