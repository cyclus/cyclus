# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( ConditioningFacility  ConditioningFacility.cpp )
ADD_LIBRARY( EnrichmentFacility  EnrichmentFacility.cpp )
ADD_LIBRARY( GenericRepository   GenericRepository.cpp GenericRepository/Component.cpp GenericRepository/StubNuclide.cpp GenericRepository/StubThermal.cpp)
ADD_LIBRARY( NullFacility        NullFacility.cpp )
ADD_LIBRARY( RecipeReactor       RecipeReactor.cpp )
ADD_LIBRARY( SeparationsMatrixFacility      SeparationsMatrixFacility.cpp )
ADD_LIBRARY( SinkFacility        SinkFacility.cpp )
#ADD_LIBRARY( StorageFacility      StorageFacility.cpp )
ADD_LIBRARY( StubFacility        StubFacility.cpp )
ADD_LIBRARY( SourceFacility      SourceFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES}
  ConditioningFacility
  EnrichmentFacility
  GenericRepository
  NullFacility
  RecipeReactor
  SeparationsMatrixFacility
  SinkFacility
  SourceFacility
  StubFacility
  )
