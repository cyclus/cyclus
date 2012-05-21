# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( RecipeReactor       RecipeReactor.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} RecipeReactor )
SET(FacilityTestSource ${FacilityTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/RecipeReactor.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/RecipeReactorTests.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/RecipeReactor/RecipeReactor.rng
${CMAKE_CURRENT_BINARY_DIR}/RecipeReactor.rng
COPYONLY
)

SET(RNG_INCLUDES ${RNG_INCLUDES}
  "<include href='../Models/Facility/RecipeReactor/RecipeReactor.rng'/>"
  PARENT_SCOPE)
SET(FAC_REFS ${FAC_REFS}
  "<ref name='RecipeReactor'/>"
  PARENT_SCOPE)
