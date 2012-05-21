# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SeparationsMatrixFacility      SeparationsMatrixFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} SeparationsMatrixFacility )
SET(FacilityTestSource ${FacilityTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/SeparationsMatrixFacilityTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/SeparationsMatrixFacility.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/SeparationsMatrixFacility/SeparationsMatrixFacility.rng
${CMAKE_CURRENT_BINARY_DIR}/SeparationsMatrixFacility.rng
COPYONLY
)


SET(RNG_INCLUDES ${RNG_INCLUDES}
  "<include href='../Models/Facility/SeparationsMatrixFacility/SeparationsMatrixFacility.rng'/>"
  PARENT_SCOPE)
SET(FAC_REFS ${FAC_REFS} 
  "<ref name='SeparationsMatrixFacility'/>"
  PARENT_SCOPE)
