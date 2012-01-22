# Build the cyclus executable from the CYCLUS_SRC source files
ADD_SUBDIRECTORY( NullRegion )
ADD_SUBDIRECTORY( StubRegion )

SET(ModelTestSource ${ModelTestSource} ${RegionTestSource} PARENT_SCOPE)
