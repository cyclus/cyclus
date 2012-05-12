
# Debian installs useful LibXML2 files to /usr/include/libxml2/libxml
FIND_PACKAGE( LibXml2 REQUIRED )
INCLUDE_DIRECTORIES ("${LIBXML2_INCLUDE_DIR}")

file(GLOB all_valid_subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*/CMakeLists.txt")

foreach(dir ${all_valid_subdirs})
    if(${dir} MATCHES "^([^/]*)//CMakeLists.txt")
        string(REGEX REPLACE "^([^/]*)//CMakeLists.txt" "\\1" dir_trimmed ${dir})
        add_subdirectory(${dir_trimmed})
    endif()
endforeach(dir)

# # Add subdirs and build for the models
# ADD_SUBDIRECTORY( Stub )
# ADD_SUBDIRECTORY( StubComm )
# ADD_SUBDIRECTORY( Market )
# ADD_SUBDIRECTORY( Facility )
# ADD_SUBDIRECTORY( Region )
# ADD_SUBDIRECTORY( Inst )
# ADD_SUBDIRECTORY( Converter )

SET( TestSource ${ModelTestSource} PARENT_SCOPE )
