# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubInst       StubInst.cpp )
ADD_LIBRARY( FixedInst      FixedInst.cpp )
ADD_LIBRARY( DeployInst     DeployInst.cpp )
ADD_LIBRARY( BuildInst      BuildInst.cpp )

SET( CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES}
  StubInst
  FixedInst
  DeployInst
  )
