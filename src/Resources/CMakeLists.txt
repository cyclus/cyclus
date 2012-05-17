# Add any new cyclus core source files to this list
SET(CYCLUS_CORE ${CYCLUS_CORE} 
  ${CMAKE_CURRENT_SOURCE_DIR}/IsoVector.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GenericResource.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Material.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MatBuff.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Resource.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ResourceBuff.cpp
  PARENT_SCOPE)
