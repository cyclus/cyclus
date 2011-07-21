# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( EnrichmentFacility  EnrichmentFacility.cpp )
ADD_LIBRARY( NullFacility        NullFacility.cpp )
ADD_LIBRARY( RecipeReactor       RecipeReactor.cpp )
ADD_LIBRARY( SeparationsMatrixFacility      SeparationsMatrixFacility.cpp )
ADD_LIBRARY( SinkFacility        SinkFacility.cpp )
#ADD_LIBRARY( StorageFacility      StorageFacility.cpp )
ADD_LIBRARY( StubFacility        StubFacility.cpp )
ADD_LIBRARY( SourceFacility      SourceFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES}
  EnrichmentFacility
  NullFacility
  RecipeReactor
  SeparationsMatrixFacility
  SinkFacility
  SourceFacility
  StorageFacility
  StubFacility
  )
