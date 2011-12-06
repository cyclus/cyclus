# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( DeployInst     DeployInst.cpp )

SET( CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} DeployInst )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Inst/DeployInst/DeployInst.rng
${CMAKE_CURRENT_BINARY_DIR}/DeployInst.rng
COPYONLY
)
