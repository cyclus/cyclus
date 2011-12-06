# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( CapacityRegion  CapacityRegion.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} CapacityRegion )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Region/CapacityRegion/CapacityRegion.rng
${CMAKE_CURRENT_BINARY_DIR}/CapacityRegion.rng
COPYONLY
)
