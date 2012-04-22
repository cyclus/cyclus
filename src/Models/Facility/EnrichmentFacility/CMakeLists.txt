# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( EnrichmentFacility  EnrichmentFacility.cpp )
SET(FacilityTestSource ${FacilityTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/EnrichmentFacility.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/EnrichmentFacilityTests.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/EnrichmentFacility/EnrichmentFacility.rng
${CMAKE_CURRENT_BINARY_DIR}/EnrichmentFacility.rng
COPYONLY
)

SET(RNG_INCLUDES 
  "<include href="../Models/Facility/EnrichmentFacility/EnrichmentFacility.rng"/>
  @RNG_INCLUDES@")
SET(FAC_REFS 
  "<ref name="EnrichmentFacility"/>
  @CONV_REFS@")
CONFIGURE_FILE(
  ${PROJECT_BINARY_DIR}/Data/cyclus.rng ${PROJECT_BINARY_DIR}/Data/cyclus.rng @ONLY
  )
