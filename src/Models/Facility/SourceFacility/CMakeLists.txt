# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SourceFacility      SourceFacility.cpp )
# SET(TEST_SOURCE ${TEST_SOURCE} SourceFacilityTests.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} SourceFacility )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/SourceFacility/SourceFacility.rng
${CMAKE_CURRENT_BINARY_DIR}/SourceFacility.rng
COPYONLY
)
