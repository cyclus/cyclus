# Add any new cyclus core source files to this list
SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} 
  ${CMAKE_CURRENT_SOURCE_DIR}/InputXML.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/XMLQueryEngine.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/XMLFileLoader.cpp
  PARENT_SCOPE)

INSTALL(FILES
  InputXML.h
  DESTINATION cyclus/include
  COMPONENT core
  )
