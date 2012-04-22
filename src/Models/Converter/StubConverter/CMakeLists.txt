# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubConverter      StubConverter.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubConverter )
SET(ConverterTestSource ${ConverterTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/StubConverterTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/StubConverter.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Converter/StubConverter/StubConverter.rng
${CMAKE_CURRENT_BINARY_DIR}/StubConverter.rng
COPYONLY
)


SET(RNG_INCLUDES 
  "<include href="../Models/Converter/StubConverter/StubConverter.rng"/>
  @RNG_INCLUDES@")
SET(CONV_REFS 
  "<ref name="StubConverter"/>
  @CONV_REFS@")
CONFIGURE_FILE(
  ${PROJECT_BINARY_DIR}/Data/cyclus.rng ${PROJECT_BINARY_DIR}/Data/cyclus.rng @ONLY
  )
