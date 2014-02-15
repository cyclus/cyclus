# ---------------------- Add Models For Cyclus -------------------------------

# This file hasn't been tested at all. 
# This setup is a trial.

FILE(GLOB all_valid_subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*/CMakeLists.txt")

ADD_SUBDIRECTORY(SinkFacility)

ADD_SUBDIRECTORY(SourceFacility)

ADD_SUBDIRECTORY(NullInst)

ADD_SUBDIRECTORY(NullRegion)

SET( TestSource ${TestSource} PARENT_SCOPE)

