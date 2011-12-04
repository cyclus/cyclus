# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( DeployInst     DeployInst.cpp )

SET( CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} DeployInst )
