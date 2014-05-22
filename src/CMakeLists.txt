
#################################################################
############# special file configuration ########################
#################################################################

CONFIGURE_FILE(suffix.h.in "${CMAKE_CURRENT_SOURCE_DIR}/suffix.h" @ONLY)

EXECUTE_PROCESS(COMMAND git describe --tags OUTPUT_VARIABLE core_version OUTPUT_STRIP_TRAILING_WHITESPACE)
CONFIGURE_FILE(version.cc.in "${CMAKE_CURRENT_SOURCE_DIR}/version.cc" @ONLY)

SET(cyclus_install_dir "${CMAKE_INSTALL_PREFIX}")
SET(cyclus_build_dir "${CYCLUS_BINARY_DIR}")
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/env.cc.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/env.cc"
  @ONLY
  )

############################################################
############# end special configuration ####################
############################################################
  
#################################################################
######### build libcyclus #######################################
#################################################################

INCLUDE_DIRECTORIES(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  )

SET(
  CYCLUS_CORE_INCLUDE_DIRS ${CYCLUS_CORE_INCLUDE_DIRS}
  "${CMAKE_CURRENT_SOURCE_DIR}"
  PARENT_SCOPE
  )

FILE(GLOB cc_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
SET(
  CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} 
  ${cc_files}
  "${CMAKE_CURRENT_SOURCE_DIR}/OsiCbcSolverInterface.cpp"
  )

FILE(GLOB h_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
FILE(GLOB hpp_files "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
SET(
  h_files ${h_files} ${hpp_files}
  )
INSTALL(
  FILES ${h_files}
  DESTINATION include/cyclus
  COMPONENT core
  )

ADD_SUBDIRECTORY(toolkit)

ADD_LIBRARY( cyclus ${CYCLUS_CORE_SRC} )

SET_TARGET_PROPERTIES( cyclus
  PROPERTIES 
  INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
  )

TARGET_LINK_LIBRARIES( cyclus dl ${LIBS} )

INSTALL(
  TARGETS cyclus
  LIBRARY DESTINATION lib
  COMPONENT cyclus
  )

#################################################################
######### end build libcyclus ###################################
#################################################################

