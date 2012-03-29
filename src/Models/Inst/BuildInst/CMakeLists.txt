# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( BuildInst      BuildInst.cpp )

SET( CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} BuildInst )

SET(InstTestSource ${InstTestSource} 
 ${CMAKE_CURRENT_SOURCE_DIR}/BuildInstTests.cpp 
 ${CMAKE_CURRENT_SOURCE_DIR}/BuildInst.cpp 
 PARENT_SCOPE)

CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Inst/BuildInst/BuildInst.rng
${CMAKE_CURRENT_BINARY_DIR}/BuildInst.rng
COPYONLY
)
