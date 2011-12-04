# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( GenericRepository   GenericRepository.cpp Component.cpp StubNuclide.cpp StubThermal.cpp)
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} GenericRepository )
