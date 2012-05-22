# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubStubComm  StubStubComm.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubStubComm )
#SET(StubCommTestSource ${StubCommTestSource} 
#  ${CMAKE_CURRENT_SOURCE_DIR}/StubStubCommTests.cpp 
#  ${CMAKE_CURRENT_SOURCE_DIR}/StubStubComm.cpp 
#  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/StubComm/StubStubComm/StubStubComm.rng
${CMAKE_CURRENT_BINARY_DIR}/StubStubComm.rng
COPYONLY
)

SET(RNG_INCLUDES ${RNG_INCLUDES} 
  "<include href='../Models/StubComm/StubStubComm/StubStubComm.rng'/>"
  PARENT_SCOPE)
SET(STUBCOMM_REFS ${STUBCOMM_REFS} 
  "<ref name='StubStubComm'/>"
  PARENT_SCOPE)
