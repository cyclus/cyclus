# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SinkFacility        SinkFacility.cpp )
ADD_LIBRARY( EnrichmentFacility  EnrichmentFacility.cpp )
ADD_LIBRARY( NullFacility        NullFacility.cpp )
ADD_LIBRARY( SeparationsMatrixFacility      SeparationsMatrixFacility.cpp )
ADD_LIBRARY( StubFacility        StubFacility.cpp )
ADD_LIBRARY( SourceFacility      SourceFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES}
  SinkFacility
  EnrichmentFacility
  StubFacility
  SeparationsMatrixFacility
  NullFacility
  )
