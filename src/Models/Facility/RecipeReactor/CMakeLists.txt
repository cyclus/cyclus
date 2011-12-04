# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( RecipeReactor       RecipeReactor.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} RecipeReactor )
