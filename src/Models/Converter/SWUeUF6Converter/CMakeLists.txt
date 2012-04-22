# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SWUeUF6Converter      SWUeUF6Converter.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} SWUeUF6Converter )
SET(ConverterTestSource ${ConverterTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/SWUeUF6ConverterTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/SWUeUF6Converter.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Converter/SWUeUF6Converter/SWUeUF6Converter.rng
${CMAKE_CURRENT_BINARY_DIR}/SWUeUF6Converter.rng
COPYONLY
)

SET(RNG_INCLUDES 
  "<include href="../Models/Converter/SWUeUF6Converter/SWUeUF6Converter.rng"/>
  @RNG_INCLUDES@")
SET(CONV_REFS 
  "<ref name="SWUeUF6Converter"/>
  @CONV_REFS@")
CONFIGURE_FILE(
  ${PROJECT_BINARY_DIR}/Data/cyclus.rng ${PROJECT_BINARY_DIR}/Data/cyclus.rng @ONLY
  )
