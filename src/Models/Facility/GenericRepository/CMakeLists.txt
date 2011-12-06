# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( GenericRepository   GenericRepository.cpp Component.cpp StubNuclide.cpp StubThermal.cpp)
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} GenericRepository )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/GenericRepository/genericrepository.rng
${CMAKE_CURRENT_BINARY_DIR}/genericrepository.rng
COPYONLY
)
