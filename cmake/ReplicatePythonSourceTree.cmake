# Note: when executed in the build dir, then CMAKE_CURRENT_SOURCE_DIR is the
# build dir.
file(COPY cli/cycpp.py DESTINATION cyclus)
file(COPY cyclus setup.py README.rst LICENSE.rst DESTINATION "${CMAKE_ARGV3}"
     FILES_MATCHING PATTERN "*.py"
                    PATTERN "*.pyi"
                    PATTERN "*.pyw"
                    PATTERN "*.csv"
                    PATTERN "*.txt"
                    PATTERN "*.rst"
                    PATTERN "*.inp"
                    PATTERN "*.html"
                    PATTERN "*.pxi"
                    PATTERN "*.pxd"
                    )
