# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( EnrichmentFacility  EnrichmentFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} EnrichmentFacility )
