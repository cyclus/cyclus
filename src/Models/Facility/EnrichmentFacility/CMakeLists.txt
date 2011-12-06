# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( EnrichmentFacility  EnrichmentFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} EnrichmentFacility )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/EnrichmentFacility/EnrichmentFacility.rng
${CMAKE_CURRENT_BINARY_DIR}/EnrichmentFacility.rng
COPYONLY
)
