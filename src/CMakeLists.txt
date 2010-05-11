PROJECT( cyclus )
SET( cyclus_SRC App.cpp Facility.cpp MktFactory.cpp NullFac.cpp SinkFac.cpp
FacFactory.cpp Market.cpp NetFlowMkt.cpp RecipeFac.cpp SourceFac.cpp )
ADD_EXECUTABLE( cyclus ${cyclus_SRC} )
