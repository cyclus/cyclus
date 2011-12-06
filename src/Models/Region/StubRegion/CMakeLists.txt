# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubRegion  StubRegion.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubRegion )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Region/StubRegion/StubRegion.rng
${CMAKE_CURRENT_BINARY_DIR}/StubRegion.rng
COPYONLY
)
