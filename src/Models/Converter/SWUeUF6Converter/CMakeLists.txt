# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SWUeUF6Converter      SWUeUF6Converter.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} SWUeUF6Converter )
