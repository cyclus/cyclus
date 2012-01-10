# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( FixedInst      FixedInst.cpp )

SET( CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} FixedInst )
SET(InstTestSource ${InstTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/FixedInstTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/FixedInst.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Inst/FixedInst/FixedInst.rng
${CMAKE_CURRENT_BINARY_DIR}/FixedInst.rng
COPYONLY
)
