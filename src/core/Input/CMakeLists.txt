# Add any new cyclus core source files to this list
SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} 
  ${CMAKE_CURRENT_SOURCE_DIR}/InputXML.cpp 
  PARENT_SCOPE)

install(FILES
  InputXML.h
  DESTINATION cyclus/include
  )
