# Add any new cyclus core source files to this list
SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} 
  ${CMAKE_CURRENT_SOURCE_DIR}/CompMap.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/IsoVector.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GenericResource.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Material.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MatBuff.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Resource.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ResourceBuff.cpp
  PARENT_SCOPE)

install(FILES
  CompMap.h
  IsoVector.h
  GenericResource.h
  Material.h
  MatBuff.h
  Resource.h
  ResourceBuff.h
  DESTINATION cyclus/include
  )

