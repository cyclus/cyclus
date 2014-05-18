##############################################################################################
##################################### begin cyclus app #######################################
##############################################################################################

INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS})

# Build the cyclus executable from the CYCLUS_SOURCE source files
SET( CYCLUS_SOURCE ${CYCLUS_SOURCE}
  cyclus.cc
  )

ADD_EXECUTABLE( cyclus_cli ${CYCLUS_SOURCE} )
set_target_properties(cyclus_cli PROPERTIES OUTPUT_NAME cyclus)

TARGET_LINK_LIBRARIES( cyclus_cli dl ${LIBS} cyclus )

INSTALL(
  TARGETS cyclus_cli
  RUNTIME DESTINATION bin
  COMPONENT cyclus
  )

INSTALL(
  PROGRAMS cycpp.py
  DESTINATION bin
  COMPONENT core
)

##############################################################################################
###################################### end cyclus app ########################################
##############################################################################################
