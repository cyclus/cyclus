# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( FixedInst      FixedInst.cpp )

SET( CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} FixedInst )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Inst/FixedInst/FixedInst.rng
${CMAKE_CURRENT_BINARY_DIR}/FixedInst.rng
COPYONLY
)
