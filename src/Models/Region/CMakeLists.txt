# Build the cyclus executable from the CYCLUS_SRC source files
ADD_SUBDIRECTORY( NullRegion )
ADD_SUBDIRECTORY( StubRegion )
ADD_SUBDIRECTORY( BuildRegion )
ADD_SUBDIRECTORY( CapacityRegion )

SET(ModelTestSource ${ModelTestSource} ${RegionTestSource} PARENT_SCOPE)
