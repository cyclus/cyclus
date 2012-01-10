# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SourceFacility      SourceFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} SourceFacility )
SET(FacilityTestSource ${FacilityTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/SourceFacilityTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/SourceFacility.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/SourceFacility/SourceFacility.rng
${CMAKE_CURRENT_BINARY_DIR}/SourceFacility.rng
COPYONLY
)
