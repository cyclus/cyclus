# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SinkFacility        SinkFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} SinkFacility )
