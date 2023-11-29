#
# Finds Pandas by trying to import it.
#

FIND_PACKAGE( Python3 )

IF( Python3_FOUND )
  EXECUTE_PROCESS( COMMAND ${Python3_EXECUTABLE} "-c" "import pandas;print(pandas.__version__)"
    OUTPUT_VARIABLE PANDAS_VERSION_STRING )
ENDIF()

INCLUDE( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Pandas
  DEFAULT_MSG
  PANDAS_VERSION_STRING
  )
