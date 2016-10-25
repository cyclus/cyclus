# Python extension modules
#include_directories(${CYMETRIC_INCLUDE_DIRS} ${INCLUDE_DIRECTORIES})
#include_directories(${CYCLUS_INCLUDE_DIRECTORIES})
#SET(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${CYCLUS_CORE_INCLUDE_DIR})

file(GLOB CYCLUS_CYTHON_FILES "${CYCLUS_PYSOURCE_DIR}/*.pyx")

set_source_files_properties(${CYCLUS_CYTHON_FILES}
    PROPERTIES CYTHON_IS_CXX TRUE)

# Print include dir
get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
message("-- Include paths for ${CMAKE_CURRENT_SOURCE_DIR}: ${inc_dirs}")

foreach(file ${CYCLUS_CYTHON_FILES})
    get_filename_component(name ${file} NAME_WE)
    set(targ "py${name}")
    cython_add_module(${targ} ${file})
    target_link_libraries(${targ} dl ${LIBS} cyclus)
    set_target_properties(${targ} PROPERTIES LINKER_LANGUAGE CXX)
endforeach()

