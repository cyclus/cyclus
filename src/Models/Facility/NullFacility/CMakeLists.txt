# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( NullFacility        NullFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} NullFacility )
SET(FacilityTestSource ${FacilityTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/NullFacilityTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/NullFacility.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/NullFacility/NullFacility.rng
${CMAKE_CURRENT_BINARY_DIR}/NullFacility.rng
COPYONLY
)
