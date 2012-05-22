# Build the cyclus executable from the CYCLUS_SRC source files
#ADD_LIBRARY( StorageFacility      StorageFacility.cpp )
#SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StorageFacility )
#SET(FacilityTestSource ${FacilityTestSource} 
#  ${CMAKE_CURRENT_SOURCE_DIR}/StorageFacilityTests.cpp 
#  ${CMAKE_CURRENT_SOURCE_DIR}/StorageFacility.cpp 
#  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/StorageFacility/StorageFacility.rng
${CMAKE_CURRENT_BINARY_DIR}/StorageFacility.rng
COPYONLY
)

SET(RNG_INCLUDES ${RNG_INCLUDES} 
  "<include href='../Models/Facility/StorageFacility/StorageFacility.rng'/>"
  PARENT_SCOPE)
SET(FAC_REFS ${FAC_REFS}
  "<ref name='StorageFacility'/>"
  PARENT_SCOPE)
