# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SWUeUF6Converter      SWUeUF6Converter.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} SWUeUF6Converter )
#SET(ConverterTestSource ${ConverterTestSource} 
#  ${CMAKE_CURRENT_SOURCE_DIR}/SWUeUF6ConverterTests.cpp 
#  ${CMAKE_CURRENT_SOURCE_DIR}/SWUeUF6Converter.cpp 
#  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Converter/SWUeUF6Converter/SWUeUF6Converter.rng
${CMAKE_CURRENT_BINARY_DIR}/SWUeUF6Converter.rng
COPYONLY
)
