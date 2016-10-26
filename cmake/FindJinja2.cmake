#
# Finds Jinja2 by trying to import it.
#
execute_process(COMMAND "${PYTHON_EXECUTABLE}" "-c" "import jinja2"
    RESULT_VARIABLE JINJA2_RTN)
if(JINJA2_RTN MATCHES 0)
    set(JINJA2_FOUND 1)
else()
    set(JINJA2_FOUND 0)
endif()
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Jinja2 REQUIRED_VARS JINJA2_FOUND)
