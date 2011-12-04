# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( FixedInst      FixedInst.cpp )

SET( CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} FixedInst )
