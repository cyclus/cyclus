# Build the cyclus executable from the CYCLUS_SRC source files
ADD_SUBDIRECTORY( StubStubComm )

SET(ModelTestSource ${ModelTestSource} ${StubCommTestSource} PARENT_SCOPE)
