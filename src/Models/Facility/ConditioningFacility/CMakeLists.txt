# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( ConditioningFacility  ConditioningFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} ConditioningFacility )
SET(FacilityTestSource ${FacilityTestSource}
  ${CMAKE_CURRENT_SOURCE_DIR}/ConditioningFacilityTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/ConditioningFacility.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/ConditioningFacility/ConditioningFacility.rng
${CMAKE_CURRENT_BINARY_DIR}/ConditioningFacility.rng
COPYONLY
)

SET(RNG_INCLUDES 
  "<include href="../Models/Facility/ConditioningFacility/ConditioningFacility.rng"/>
  @RNG_INCLUDES@")
SET(FAC_REFS 
  "<ref name="ConditioningFacility"/>
  @CONV_REFS@")
CONFIGURE_FILE(
  ${PROJECT_BINARY_DIR}/Data/cyclus.rng ${PROJECT_BINARY_DIR}/Data/cyclus.rng @ONLY
  )
