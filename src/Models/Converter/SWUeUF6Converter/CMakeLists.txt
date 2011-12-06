# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SWUeUF6Converter      SWUeUF6Converter.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} SWUeUF6Converter )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Converter/SWUeUF6Converter/SWUeUF6Converter.rng
${CMAKE_CURRENT_BINARY_DIR}/SWUeUF6Converter.rng
COPYONLY
)
