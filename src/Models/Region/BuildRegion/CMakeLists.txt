# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( BuildRegion  BuildRegion.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} BuildRegion )
SET(RegionTestSource ${RegionTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/BuildRegionTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/BuildRegion.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Region/BuildRegion/BuildRegion.rng
${CMAKE_CURRENT_BINARY_DIR}/BuildRegion.rng
COPYONLY
)
