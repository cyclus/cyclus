macro(cyclus_init  _path _dir _name)

  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}${_path})

  # Build the cyclus executable from the CYCLUS_SRC source files
  ADD_LIBRARY( ${_dir}       ${_name}.cc )
  # Link the libraries to libcycluscore
  TARGET_LINK_LIBRARIES(${_dir} dl cycluscore)
  SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} ${_dir} )
  
  CONFIGURE_FILE(
    ${CYCLUS_SOURCE_DIR}/tests${_path}/${_name}.rng
    ${PROJECT_BINARY_DIR}/lib${_path}/${_name}.rng
    COPYONLY
    )
  
  install(TARGETS ${_dir}
    LIBRARY DESTINATION lib${_path}
    COMPONENT ${_path}
    )

  install(FILES "${_name}.rng"
    DESTINATION lib${_path}
    COMPONENT "${_path}.rng"
    )
endmacro()
  
macro(cyclus_init_model _type _dir _name)
  SET(MODEL_PATH "/Models/${_type}/${_dir}")
  cyclus_init(${MODEL_PATH} ${_dir} ${_name})

  SET(${_type}TestSource ${${_type}TestSource} 
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
