# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubRegion  StubRegion.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubRegion )
SET(RegionTestSource ${RegionTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/StubRegionTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/StubRegion.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Region/StubRegion/StubRegion.rng
${CMAKE_CURRENT_BINARY_DIR}/StubRegion.rng
COPYONLY
)
