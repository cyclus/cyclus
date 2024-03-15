# Python extension modules
#include_directories(${CYMETRIC_INCLUDE_DIRS} ${INCLUDE_DIRECTORIES})
#include_directories(${CYCLUS_INCLUDE_DIRECTORIES})
#SET(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${CYCLUS_CORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS})

#
# First things first, code-generation
#
message(STATUS "Generating Type System API for Python")
EXECUTE_PROCESS(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/gentypesystem.py
    "--src-dir=${CMAKE_CURRENT_SOURCE_DIR}"
    "--cyclus-version=${core_version}"
    "--verbose"
    RESULT_VARIABLE res_var)
IF(NOT "${res_var}" STREQUAL "0")
  message(FATAL_ERROR "Generating type system failed (gentypesystem.py), res_var = '${res_var}'")
ENDIF()

#
# Build cython files
#
file(GLOB CYCLUS_CYTHON_FILES "${CYCLUS_PYSOURCE_DIR}/*.pyx")

set_source_files_properties(${CYCLUS_CYTHON_FILES}
    PROPERTIES CYTHON_IS_CXX TRUE)

# Print include dir
get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
message("-- Include paths for ${CMAKE_CURRENT_SOURCE_DIR}: ${inc_dirs}")

foreach(file ${CYCLUS_CYTHON_FILES})
    get_filename_component(name ${file} NAME_WE)
    set(targ "py_${name}_")
    cython_add_module(${targ} ${file})
    target_link_libraries(${targ} PRIVATE dl ${LIBS} cyclus)
    set_target_properties(${targ} PROPERTIES LINKER_LANGUAGE CXX
                                             OUTPUT_NAME ${name}
                                             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    # the following install is needed so that CMake sets the correct RPATH for the
    # python extension modules, even though they are installed (with the wrong RPATH)
    # by setup.py, along with the pure python files.
    install(TARGETS ${targ} LIBRARY DESTINATION "${PYTHON_SITE_PACKAGES}/cyclus")
endforeach()

FILE(GLOB CYCLUS_PYTHON_FILES "${CYCLUS_PYSOURCE_DIR}/*.py")
INSTALL(FILES ${CYCLUS_PYTHON_FILES} DESTINATION "${PYTHON_SITE_PACKAGES}/cyclus")
