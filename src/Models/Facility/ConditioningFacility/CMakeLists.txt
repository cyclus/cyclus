# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( ConditioningFacility  ConditioningFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} ConditioningFacility )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/ConditioningFacility/ConditioningFacility.rng
${CMAKE_CURRENT_BINARY_DIR}/ConditioningFacility.rng
COPYONLY
)
