# Build the cyclus executable from the CYCLUS_SRC source files
ADD_SUBDIRECTORY( StubInst )
ADD_SUBDIRECTORY( FixedInst )
ADD_SUBDIRECTORY( DeployInst )
ADD_SUBDIRECTORY( BuildInst )

SET(ModelTestSource ${ModelTestSource} ${InstTestSource} PARENT_SCOPE)

