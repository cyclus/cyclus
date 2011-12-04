# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubConverter StubConverter.cpp )
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubConverter )
