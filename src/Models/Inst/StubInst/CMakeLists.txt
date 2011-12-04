# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubInst       StubInst.cpp )

SET( CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubInst )
