# Null the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( NullRegion  NullRegion.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} NullRegion )
SET(RegionTestSource ${RegionTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/NullRegionTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/NullRegion.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Region/NullRegion/NullRegion.rng
${CMAKE_CURRENT_BINARY_DIR}/NullRegion.rng
COPYONLY
)
