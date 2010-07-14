# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubFacility     MODULE StubFacility.cpp )
ADD_LIBRARY( SourceFacility   MODULE SourceFacility.cpp )
ADD_LIBRARY( NullFacility     MODULE NullFacility.cpp )
