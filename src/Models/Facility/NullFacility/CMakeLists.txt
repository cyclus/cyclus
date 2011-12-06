# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( NullFacility        NullFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} NullFacility )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/NullFacility/NullFacility.rng
${CMAKE_CURRENT_BINARY_DIR}/NullFacility.rng
COPYONLY
)
