# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubInst       StubInst.cpp )

SET( CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubInst )
SET(InstTestSource ${InstTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/StubInstTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/StubInst.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Inst/StubInst/StubInst.rng
${CMAKE_CURRENT_BINARY_DIR}/StubInst.rng
COPYONLY
)
