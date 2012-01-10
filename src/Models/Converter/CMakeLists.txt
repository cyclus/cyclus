ADD_SUBDIRECTORY( StubConverter )
ADD_SUBDIRECTORY( SWUeUF6Converter )

SET(ModelTestSource ${ModelTestSource} ${ConverterTestSource} PARENT_SCOPE)

