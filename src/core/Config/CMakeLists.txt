# This allows apple to build the libraries dynamically
IF (WIN32)
  set(suffix ".dll")
  set(dynamicloadlib "WinLoadConstr.h")
  CONFIGURE_FILE(suffix.h.in suffix.h @ONLY)
ELSEIF(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup" PARENT_SCOPE)
  set(suffix ".dylib")
  set(dynamicloadlib "UnixLoadConstr.h")
  CONFIGURE_FILE(suffix.h.in suffix.h @ONLY)
ELSE()
  set(suffix ".so")
  set(dynamicloadlib "UnixLoadConstr.h")
  CONFIGURE_FILE(suffix.h.in suffix.h @ONLY)
ENDIF()

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/cyclus.rng.in
  ${PROJECT_BINARY_DIR}/share/cyclus.rng
  COPYONLY
  )

set(input_path ${CYCLUS_SOURCE_DIR}/../input)
set(cyclus_path ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/run_inputs.py.in 
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/run_inputs.py @ONLY)

set(input_path ${CYCLUS_SOURCE_DIR}/../input)
set(cyclus_path ${CMAKE_INSTALL_PREFIX}/cyclus/bin)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/run_inputs.py.in 
  ${CMAKE_CURRENT_SOURCE_DIR}/run_inputs.py @ONLY)

install(FILES run_inputs.py 
  DESTINATION cyclus/bin
  COMPONENT testing
  )

install(FILES
  UnixLoadConstr.h
  WinLoadConstr.h
  suffix.h
  DESTINATION cyclus/include
  )
