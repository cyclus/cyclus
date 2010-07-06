# This project name is cyclus.
PROJECT( CYCLUS )
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Direct any out-of-source builds to this directory
SET( CYCLUS_SRC_DIR ${CMAKE_SOURCE_DIR})

# Add any new source files to this list.
SET( CYCLUS_SRC App.cpp Commodity.cpp Communicator.cpp Facility.cpp 
FacFactory.cpp GenException.cpp InputDB.cpp Inst.cpp LMatrix.cpp Logician.cpp
Market.cpp Material.cpp Message.cpp MktFactory.cpp NetFlowMkt.cpp NullFac.cpp 
RecipeFac.cpp Region.cpp SinkFac.cpp SourceFac.cpp Timer.cpp UniformTaylor.cpp)

# This is the directory that holds the doxygen doxyfile template (doxy.conf.in)
SET( DOC_INPUT_DIR ${CYCLUS_SRC_DIR}/doc)

# If doxygen exists, use the doc/CMakeLists.txt file for further instructions. 
FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND)
	ADD_SUBDIRECTORY(doc)
	SET( DOC_OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc )
ELSE (DOXYGEN_FOUND)
	MESSAGE(STATUS "WARNING: Doxygen not found - doc won't be created")
ENDIF (DOXYGEN_FOUND)

# Build the cyclus executable from the CYCLUS_SRC source files
ADD_EXECUTABLE( cyclus ${CYCLUS_SRC} )
