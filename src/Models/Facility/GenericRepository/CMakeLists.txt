# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( GenericRepository   GenericRepository.cpp Component.cpp StubNuclide.cpp StubThermal.cpp)
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} GenericRepository )
SET(FacilityTestSource ${FacilityTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/GenericRepositoryTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GenericRepository.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Component.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/StubNuclide.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/StubThermal.cpp
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Facility/GenericRepository/GenericRepository.rng
${CMAKE_CURRENT_BINARY_DIR}/GenericRepository.rng
COPYONLY
)
