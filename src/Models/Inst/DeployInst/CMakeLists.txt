# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( DeployInst     DeployInst.cpp )

SET( CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} DeployInst )
SET(InstTestSource ${InstTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/DeployInstTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/DeployInst.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Inst/DeployInst/DeployInst.rng
${CMAKE_CURRENT_BINARY_DIR}/DeployInst.rng
COPYONLY
)
