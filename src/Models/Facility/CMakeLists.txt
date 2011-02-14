# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SinkFacility       MODULE SinkFacility.cpp )
ADD_LIBRARY( EnrichmentFacility MODULE EnrichmentFacility.cpp )
ADD_LIBRARY( StubFacility       MODULE StubFacility.cpp )
ADD_LIBRARY( SourceFacility     MODULE SourceFacility.cpp )
ADD_LIBRARY( NullFacility       MODULE NullFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES}
  SinkFacility
  EnrichmentFacility
  StubFacility
  NullFacility
  )
