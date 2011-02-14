# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubInst      MODULE StubInst.cpp )
ADD_LIBRARY( FixedInst     MODULE FixedInst.cpp )
ADD_LIBRARY( DeployInst    MODULE DeployInst.cpp )

SET( CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES}
  StubInst
  FixedInst
  DeployInst
  )
