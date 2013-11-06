##############################################################################################
##################################### begin cyclus app #######################################
##############################################################################################

INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS})

# Build the cyclus executable from the CYCLUS_SOURCE source files
SET( CYCLUS_SOURCE ${CYCLUS_SOURCE}
  cyclus.cc
  )

ADD_EXECUTABLE( cyclus ${CYCLUS_SOURCE} )

TARGET_LINK_LIBRARIES( cyclus dl ${LIBS} cycluscore )

INSTALL(
  TARGETS cyclus
  RUNTIME DESTINATION bin
  COMPONENT cyclus
  )

ADD_SUBDIRECTORY(inventory)

##############################################################################################
###################################### end cyclus app ########################################
##############################################################################################
