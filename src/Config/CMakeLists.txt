# This allows apple to build the libraries dynamically
IF (WIN32)
  set(suffix ".dll")
  set(dynamicloadlib "WindowHelperFunctions.h")
  CONFIGURE_FILE(suffix.h.in ${CMAKE_CURRENT_SOURCE_DIR}/suffix.h @ONLY)
ELSEIF(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup" PARENT_SCOPE)
  set(suffix ".dylib")
  set(dynamicloadlib "unix_helper_functions.h")
  CONFIGURE_FILE(suffix.h.in ${CMAKE_CURRENT_SOURCE_DIR}/suffix.h @ONLY)
ELSE()
  set(suffix ".so")
  set(dynamicloadlib "unix_helper_functions.h")
  CONFIGURE_FILE(suffix.h.in ${CMAKE_CURRENT_SOURCE_DIR}/suffix.h @ONLY)
ENDIF()

EXECUTE_PROCESS(COMMAND git describe OUTPUT_VARIABLE core_version OUTPUT_STRIP_TRAILING_WHITESPACE)
CONFIGURE_FILE(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h @ONLY)

# Here we set some components for installation with cpack
INSTALL(FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/cyclus.rng.in
  DESTINATION share
  COMPONENT data
  )

INSTALL(FILES
    unix_helper_functions.h
    windows_helper_functions.h
    suffix.h
    version.h
  DESTINATION include/cyclus
  COMPONENT core
  )
