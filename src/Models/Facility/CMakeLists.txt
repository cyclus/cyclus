# Build the cyclus executable from the CYCLUS_SRC source files
ADD_SUBDIRECTORY( ConditioningFacility )
ADD_SUBDIRECTORY( EnrichmentFacility )
ADD_SUBDIRECTORY( GenericRepository )
ADD_SUBDIRECTORY( NullFacility )
ADD_SUBDIRECTORY( RecipeReactor )
ADD_SUBDIRECTORY( SeparationsMatrixFacility )
ADD_SUBDIRECTORY( SinkFacility )
ADD_SUBDIRECTORY( StubFacility )
ADD_SUBDIRECTORY( SourceFacility )

SET(TestSource ${TestSource} PARENT_SCOPE)
