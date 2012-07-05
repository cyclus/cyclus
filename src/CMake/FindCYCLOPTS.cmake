
# if installed in a non-standard location,
# the include directory must be added to the PATH variable
FIND_PATH(CYCLOPTS_INCLUDE_DIR cyclopts/Variable.h
  HINTS /usr/
  HINTS /usr/include/
  HINTS /usr/local/
  HINTS /usr/local/include/
  HINTS /usr/cyclopts/
  HINTS /usr/local/cyclopts/
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