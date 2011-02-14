# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SWUeUF6Converter     MODULE SWUeUF6Converter.cpp )
#ADD_LIBRARY( NullConverter        MODULE NullConverter.cpp )
ADD_LIBRARY( StubConverter        MODULE StubConverter.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES}
  SWUeUF6Converter
  #  NullConverter
  StubConverter
  )
