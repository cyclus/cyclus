# Add any new cyclus core source files to this list
SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} 
  ${CMAKE_CURRENT_SOURCE_DIR}/QueryEngine.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/XMLFileLoader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/XMLQueryEngine.cpp 
  PARENT_SCOPE)

INSTALL(FILES
  QueryEngine.h 
  XMLFileLoader.h
  XMLQueryEngine.h
  DESTINATION cyclus/include
  COMPONENT core
  )
