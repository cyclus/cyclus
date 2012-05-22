# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubStub  StubStub.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubStub )
SET(StubTestSource ${StubTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/StubStubTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/StubStub.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Stub/StubStub/StubStub.rng
${CMAKE_CURRENT_BINARY_DIR}/StubStub.rng
COPYONLY
)

SET(RNG_INCLUDES ${RNG_INCLUDES} 
  "<include href='../Models/Stub/StubStub/StubStub.rng'/>"
  PARENT_SCOPE)
SET(STUB_REFS ${STUB_REFS}
  "<ref name='StubStub'/>"
  PARENT_SCOPE)
