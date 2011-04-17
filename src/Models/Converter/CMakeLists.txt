# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SWUeUF6Converter      SWUeUF6Converter.cpp )
#ADD_LIBRARY( NullConverter         NullConverter.cpp )
ADD_LIBRARY( StubConverter         StubConverter.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES}
  SWUeUF6Converter
  #  NullConverter
  StubConverter
  )
