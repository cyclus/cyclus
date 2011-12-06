# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( RecipeReactor       RecipeReactor.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} RecipeReactor )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/RecipeReactor/RecipeReactor.rng
${CMAKE_CURRENT_BINARY_DIR}/RecipeReactor.rng
COPYONLY
)
