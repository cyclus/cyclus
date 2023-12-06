#
# Finds Jinja2 by trying to import it.
#

FIND_PACKAGE( Python3 )

IF( Python3_FOUND )
  EXECUTE_PROCESS( COMMAND ${Python3_EXECUTABLE} "-c" "import jinja2;print(jinja2.__version__)"
    OUTPUT_VARIABLE JINJA2_VERSION_STRING )
ENDIF()

INCLUDE( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Jinja2
  DEFAULT_MSG
  JINJA2_VERSION_STRING
  )
