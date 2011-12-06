# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubStubComm  StubStubComm.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubStubComm )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/StubComm/StubStubComm/StubStubComm.rng
${CMAKE_CURRENT_BINARY_DIR}/StubStubComm.rng
COPYONLY
)
