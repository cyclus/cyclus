
#################################################################
############# special file configuration ########################
#################################################################

CONFIGURE_FILE(suffix.h.in "${CMAKE_CURRENT_SOURCE_DIR}/suffix.h" @ONLY)

EXECUTE_PROCESS(COMMAND git describe --tags OUTPUT_VARIABLE core_version OUTPUT_STRIP_TRAILING_WHITESPACE)
CONFIGURE_FILE(version.cc.in "${CMAKE_CURRENT_SOURCE_DIR}/version.cc" @ONLY)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5")
    message("-- Found '${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5'")
else()
    message("-- Downloading http://regtests.fuelcycle.org/cyclus_nuc_data.h5 to "
            "'${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5'...")
    file(DOWNLOAD "http://regtests.fuelcycle.org/cyclus_nuc_data.h5" 
         "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5")
    message("-- ...download complete!")
endif()

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5"
  "${CYCLUS_BINARY_DIR}/share/cyclus_nuc_data.h5"
  COPYONLY
  )

SET(cyclus_install_dir "${CMAKE_INSTALL_PREFIX}")
SET(cyclus_build_dir "${CYCLUS_BINARY_DIR}")
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/env.cc.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/env.cc"
  @ONLY
  )

INSTALL(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus.rng.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus-flat.rng.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5"
  DESTINATION share
  COMPONENT core
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
SET(
  h_files ${h_files}
  any.hpp
  OsiCbcSolverInterface.hpp
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

