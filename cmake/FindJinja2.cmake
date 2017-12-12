#
# Finds Jinja2 by trying to import it.
#

FIND_PACKAGE( PythonInterp )

IF( PYTHONINTERP_FOUND )
  EXECUTE_PROCESS( COMMAND ${PYTHON_EXECUTABLE} "-c" "import jinja2;print(jinja2.__version__)"
    OUTPUT_VARIABLE JINJA2_VERSION_STRING )
ENDIF()

INCLUDE( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Jinja2
  DEFAULT_MSG
  JINJA2_VERSION_STRING
  )
