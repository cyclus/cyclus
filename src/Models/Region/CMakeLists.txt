# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( NullRegion MODULE NullRegion.cpp )
ADD_LIBRARY( StubRegion MODULE StubRegion.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES}
  NullRegion
  StubRegion
  )
