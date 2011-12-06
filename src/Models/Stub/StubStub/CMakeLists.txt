# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubStub  StubStub.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubStub )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Stub/StubStub/StubStub.rng
${CMAKE_CURRENT_BINARY_DIR}/StubStub.rng
COPYONLY
)
