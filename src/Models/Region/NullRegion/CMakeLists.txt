# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( NullRegion  NullRegion.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} NullRegion )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Region/NullRegion/NullRegion.rng
${CMAKE_CURRENT_BINARY_DIR}/NullRegion.rng
COPYONLY
)
