# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( NullFacility        NullFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} NullFacility )
