# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SeparationsMatrixFacility      SeparationsMatrixFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} SeparationsMatrixFacility )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/SeparationsMatrixFacility/SeparationsMatrixFacility.rng
${CMAKE_CURRENT_BINARY_DIR}/SeparationsMatrixFacility.rng
COPYONLY
)
