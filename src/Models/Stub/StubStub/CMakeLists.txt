# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubStub  StubStub.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubStub )
