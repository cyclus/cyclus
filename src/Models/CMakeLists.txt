
# Debian installs useful LibXML2 files to /usr/include/libxml2/libxml
FIND_PACKAGE( LibXml2 REQUIRED )
INCLUDE_DIRECTORIES ("${LIBXML2_INCLUDE_DIR}")


SET( TestSource ${ModelTestSource} PARENT_SCOPE )
CONFIGURE_FILE(${PROJECT_BINARY_DIR}/share/cyclus.rng 
  ${PROJECT_BINARY_DIR}/share/cyclus.rng @ONLY)

install(FILES 
  ConverterModel.h
  FacilityModel.h
  InstModel.h
  MarketModel.h
  Model.h
  RegionModel.h
  DESTINATION cyclus/include
  )

#file(GLOB all_valid_subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*/CMakeLists.txt")
#
#foreach(dir ${all_valid_subdirs})
#    if(${dir} MATCHES "^([^/]*)//CMakeLists.txt")
#        string(REGEX REPLACE "^([^/]*)//CMakeLists.txt" "\\1" dir_trimmed ${dir})
#        add_subdirectory(${dir_trimmed})
#    endif()
#endforeach(dir)

