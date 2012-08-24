# This allows apple to build the libraries dynamically
IF (WIN32)
  set(suffix ".dll")
  set(dynamicloadlib "WinLoadConstr.h")
  CONFIGURE_FILE(suffix.h.in ${CMAKE_CURRENT_SOURCE_DIR}/suffix.h @ONLY)
ELSEIF(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup" PARENT_SCOPE)
  set(suffix ".dylib")
  set(dynamicloadlib "UnixLoadConstr.h")
  CONFIGURE_FILE(suffix.h.in ${CMAKE_CURRENT_SOURCE_DIR}/suffix.h @ONLY)
ELSE()
  set(suffix ".so")
  set(dynamicloadlib "UnixLoadConstr.h")
  CONFIGURE_FILE(suffix.h.in ${CMAKE_CURRENT_SOURCE_DIR}/suffix.h @ONLY)
ENDIF()

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/cyclus.rng.in
  ${PROJECT_BINARY_DIR}/share/cyclus.rng.in
  COPYONLY
  )

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/cyclus.rng
  ${PROJECT_BINARY_DIR}/share/cyclus.rng
  COPYONLY
  )

INSTALL(FILES
    UnixLoadConstr.h
    WinLoadConstr.h
    suffix.h
  DESTINATION cyclus/include
  COMPONENT core
  )
