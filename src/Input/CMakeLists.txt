# Add any new cyclus core source files to this list
SET(CYCLUS_CORE ${CYCLUS_CORE} 
  ${CMAKE_CURRENT_SOURCE_DIR}/InputXML.cpp 
  PARENT_SCOPE)

install(FILES
  InputXML.h
  DESTINATION cyclus/include
  )
