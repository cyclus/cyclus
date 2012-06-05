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
  cyclus.rng.in
  ${PROJECT_BINARY_DIR}/share/cyclus.rng
  COPYONLY
  )

set(input_path ${CYCLUS_SOURCE_DIR}/../input)
set(cyclus_path ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
CONFIGURE_FILE(${CYCLUS_SRC_DIR}/Config/run_inputs.py.in 
  ${PROJECT_BINARY_DIR}/Testing/run_inputs.py @ONLY)

