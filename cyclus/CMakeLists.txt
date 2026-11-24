# Python extension modules
#include_directories(${CYMETRIC_INCLUDE_DIRS} ${INCLUDE_DIRECTORIES})
#include_directories(${CYCLUS_INCLUDE_DIRECTORIES})
#SET(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${CYCLUS_CORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS})

#
# First things first, code-generation
#
CONFIGURE_FILE(system.py.in "${CMAKE_CURRENT_BINARY_DIR}/system.py" @ONLY)
message(STATUS "Generating Type System API for Python")
EXECUTE_PROCESS(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gentypesystem.py
    "--out-dir=${CMAKE_CURRENT_BINARY_DIR}"
    "--dbtypes-json=${CYCLUS_DBTYPES_JSON}"
    "--data-model-version=${DATA_MODEL_VERSION}"
    "--verbose"
    RESULT_VARIABLE res_var)
IF(NOT "${res_var}" STREQUAL "0")
  message(FATAL_ERROR "Generating type system failed (gentypesystem.py), res_var = '${res_var}'")
ENDIF()

#
# Build cython files
#
file(GLOB CYCLUS_CYTHON_FILES "${CYCLUS_PYSOURCE_DIR}/*.pyx")
list(APPEND CYCLUS_CYTHON_FILES "${CMAKE_CURRENT_BINARY_DIR}/typesystem.pyx")

# typesystem.pyx, typesystem.pxd, and cpp_typesystem.pxd are generated into
# the cyclus/ directory of the build tree, which the ReplicatePythonSourceTree
# target makes a complete copy of the cyclus package. Cython looks up
# "cimport cyclus.<module>" in the cyclus/ directory next to the file it
# compiles and then in that of each of its include directories.
set(CYTHON_FLAGS ${CYTHON_FLAGS} -I "${PROJECT_BINARY_DIR}")

set_source_files_properties(${CYCLUS_CYTHON_FILES}
    PROPERTIES CYTHON_IS_CXX TRUE)

# Print include dir
get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
message("-- Include paths for ${CMAKE_CURRENT_SOURCE_DIR}: ${inc_dirs}")

foreach(file ${CYCLUS_CYTHON_FILES})
    get_filename_component(name ${file} NAME_WE)
    set(targ "py_${name}_")
    cython_add_module(${targ} ${file})
    add_dependencies(${targ} ReplicatePythonSourceTree)
    target_link_libraries(${targ} PRIVATE dl ${LIBS} cyclus)
    set_target_properties(${targ} PROPERTIES LINKER_LANGUAGE CXX
                                             OUTPUT_NAME ${name}
                                             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    if (CYTHON_VERSION_MAJOR GREATER_EQUAL 3)
        target_compile_definitions(${targ} PRIVATE NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
    endif()
    # the following install is needed so that CMake sets the correct RPATH for the
    # python extension modules, even though they are installed (with the wrong RPATH)
    # by setup.py, along with the pure python files.
    install(TARGETS ${targ} LIBRARY DESTINATION "${PYTHON_SITE_PACKAGES}/cyclus")
endforeach()
