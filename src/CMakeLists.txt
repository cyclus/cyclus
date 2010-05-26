PROJECT( CYCLUS )
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET( CYCLUS_SRC_DIR ${CMAKE_SOURCE_DIR})
SET( CYCLUS_SRC App.cpp Facility.cpp MktFactory.cpp NullFac.cpp SinkFac.cpp
FacFactory.cpp Market.cpp NetFlowMkt.cpp RecipeFac.cpp SourceFac.cpp )

SET( DOC_INPUT_DIR ${CYCLUS_SRC_DIR}/doc)
FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND)
	ADD_SUBDIRECTORY(doc)
	SET( DOC_OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc )
ELSE (DOXYGEN_FOUND)
	MESSAGE(STATUS "WARNING: Doxygen not found - doc won't be created")
ENDIF (DOXYGEN_FOUND)

ADD_EXECUTABLE( cyclus ${CYCLUS_SRC} )
