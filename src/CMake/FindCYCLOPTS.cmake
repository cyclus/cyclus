
IF(NOT DEFINED CYCLOPTS_ROOT_DIR)
    SET(CYCLOPTS_ROOT_DIR "$ENV{CYCLOPTS_ROOT_DIR}")
ENDIF(NOT DEFINED CYCLOPTS_ROOT_DIR)

MESSAGE(STATUS "CYCLOPTS_ROOT_DIR hint is : ${CYCLOPTS_ROOT_DIR}")

# if installed in a non-standard location,
# the include directory must be added to the PATH variable
FIND_PATH(CYCLOPTS_INCLUDE_DIR cyclopts/Variable.h
  HINTS "${CYCLOPTS_INCLUDE_DIR}"
  HINTS "${CYCLOPTS_ROOT_DIR}"
  HINTS "${CYCLOPTS_ROOT_DIR}/include"
  HINTS /usr/
  HINTS /usr/include/
  HINTS /usr/local/
  HINTS /usr/local/include/
  HINTS /usr/cyclopts/
  HINTS /usr/local/cyclopts/
  HINTS ${CYCLOPTS_HINT}/
)

set(CYCLOPTS_INCLUDE_DIR ${CYCLOPTS_INCLUDE_DIR}/cyclopts)
#MESSAGE("\tCYCLOPTS Include Dir: ${CYCLOPTS_INCLUDE_DIR}")

FIND_LIBRARY(CYCLOPTS_LIBRARY
  NAMES cyclopts libcyclopts
  HINTS ${CYCLOPTS_INCLUDE_DIR}/../../lib
)

# let us know if we found it
IF(CYCLOPTS_INCLUDE_DIR)
  MESSAGE("\tFound CYCLOPTS Include Dir: ${CYCLOPTS_INCLUDE_DIR}")
ELSE(CYCLOPTS_INCLUDE_DIR)
  MESSAGE("\tCYCLOPTS Include Dir NOT FOUND")
ENDIF(CYCLOPTS_INCLUDE_DIR)