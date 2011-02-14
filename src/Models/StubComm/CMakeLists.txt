# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubStubComm MODULE StubStubComm.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES}
  StubStubCom
  )
