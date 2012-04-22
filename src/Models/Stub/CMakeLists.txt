# Build the cyclus executable from the CYCLUS_SRC source files
file(GLOB all_valid_subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*/CMakeLists.txt")

foreach(dir ${all_valid_subdirs})
    if(${dir} MATCHES "^([^/]*)//CMakeLists.txt")
        string(REGEX REPLACE "^([^/]*)//CMakeLists.txt" "\\1" dir_trimmed ${dir})
        add_subdirectory(${dir_trimmed})
    endif()
endforeach(dir)


SET(ModelTestSource ${ModelTestSource} ${StubTestSource} PARENT_SCOPE)
#SET(STUB_REFS "")
#CONFIGURE_FILE(${PROJECT_BINARY_DIR}/Data/cyclus.rng 
#  ${PROJECT_BINARY_DIR}/Data/cyclus.rng @ONLY)
SET(STUB_REFS ${STUB_REFS} PARENT_SCOPE)
SET(RNG_INCLUDES ${RNG_INCLUDES} PARENT_SCOPE)
