# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( SeparationsMatrixFacility      SeparationsMatrixFacility.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} SeparationsMatrixFacility )
