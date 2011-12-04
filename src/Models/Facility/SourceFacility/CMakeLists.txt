# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SourceFacility      SourceFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} SourceFacility )
