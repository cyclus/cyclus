# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubFacility        StubFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubFacility )
SET(TestSource ${TestSource} ${CMAKE_CURRENT_SOURCE_DIR}/StubFacilityTests.cpp PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/StubFacility/StubFacility.rng
${CMAKE_CURRENT_BINARY_DIR}/StubFacility.rng
COPYONLY
)
