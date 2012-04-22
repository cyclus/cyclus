# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( BatchReactor       BatchReactor.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} BatchReactor )
SET(FacilityTestSource ${FacilityTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/BatchReactor.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/BatchReactorTests.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/BatchReactor/BatchReactor.rng
${CMAKE_CURRENT_BINARY_DIR}/BatchReactor.rng
COPYONLY
)


SET(RNG_INCLUDES ${RNG_INCLUDES}
  "<include href='../Models/Facility/BatchReactor/BatchReactor.rng'/>"
  PARENT_SCOPE)
SET(FAC_REFS ${FAC_REFS}
  "<ref name='BatchReactor'/>"
  PARENT_SCOPE)
