# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( CapacityRegion  CapacityRegion.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} CapacityRegion )
SET(RegionTestSource ${RegionTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/CapacityRegionTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/CapacityRegion.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Region/CapacityRegion/CapacityRegion.rng
${CMAKE_CURRENT_BINARY_DIR}/CapacityRegion.rng
COPYONLY
)
