# Note: when executed in the build dir, then CMAKE_CURRENT_SOURCE_DIR is the
# build dir.
file(COPY cyclus setup.py README.rst LICENSE.rst DESTINATION "${CMAKE_ARGV3}"
     FILES_MATCHING PATTERN "*.py"
                    PATTERN "*.pyi"
                    PATTERN "*.pyw"
                    PATTERN "*.csv"
                    PATTERN "*.txt"
                    PATTERN "*.inp"
                    PATTERN "*.html"
                    PATTERN "*.pxi"
                    PATTERN "*.pxd"
                    )

#set(PY_SOURCE_DIR "${CMAKE_SOURCE_DIR}/cyclus")
#file(GLOB pyfiles "${PY_SOURCE_DIR}/*.py"
#                  "${PY_SOURCE_DIR}/*.pyw"
#                  "${PY_SOURCE_DIR}/*.pxd"
#                  "${PY_SOURCE_DIR}/*.csv"
#                  "${PY_SOURCE_DIR}/*.txt"
#                  "${PY_SOURCE_DIR}/*.inp"
#                  "${PY_SOURCE_DIR}/*.html"
#                  "${PY_SOURCE_DIR}/*.pxi"
#                  "${PY_SOURCE_DIR}/*.mas12")

#add_custom_target(pysourcecopy)
#foreach(pyfile ${pyfiles})
#  add_custom_command(TARGET copy PRE_BUILD
#                     COMMAND ${CMAKE_COMMAND} -E
#                         copy ${pyfile} $<TARGET_FILE_DIR:${CMAKE_ARGV3}>)
#endforeach()
