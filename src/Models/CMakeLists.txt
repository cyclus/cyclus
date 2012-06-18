
# Debian installs useful LibXML2 files to /usr/include/libxml2/libxml
FIND_PACKAGE( LibXml2 REQUIRED )
INCLUDE_DIRECTORIES ("${LIBXML2_INCLUDE_DIR}")

# Add the core model files to the core source
SET( CYCLUS_CORE_SRC 
  ConverterModel.cpp
  FacilityModel.cpp
  InstModel.cpp
  MarketModel.cpp
  Message.cpp
  Model.cpp
  StubCommModel.cpp
  StubModel.cpp
  StubTimeAgent.cpp
  Transaction.cpp
  PARENT_SCOPE 
  )

SET( TestSource ${ModelTestSource} PARENT_SCOPE )
CONFIGURE_FILE(${PROJECT_BINARY_DIR}/share/cyclus.rng 
  ${PROJECT_BINARY_DIR}/share/cyclus.rng @ONLY)

install(FILES 
  Communicator.h
  ConverterModel.h
  FacilityModel.h
  InstModel.h
  MarketModel.h
  Message.h
  Model.h
  RegionModel.h
  StubCommModel.h
  StubModel.h
  StubTimeAgent.h
  Transaction.h
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

