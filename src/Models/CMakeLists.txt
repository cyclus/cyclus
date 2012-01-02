
# Debian installs useful LibXML2 files to /usr/include/libxml2/libxml
FIND_PACKAGE( LibXml2 REQUIRED )
INCLUDE_DIRECTORIES ("${LIBXML2_INCLUDE_DIR}")
SET( INTERFACE_LIBRARIES ${INTERFACE_LIRARIES} FacilityModelTests.cpp )

# Add subdirs and build for the models
ADD_SUBDIRECTORY( Stub )
ADD_SUBDIRECTORY( StubComm )
ADD_SUBDIRECTORY( Market )
ADD_SUBDIRECTORY( Facility )
ADD_SUBDIRECTORY( Region )
ADD_SUBDIRECTORY( Inst )
ADD_SUBDIRECTORY( Converter )

