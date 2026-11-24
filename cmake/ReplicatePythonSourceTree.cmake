# Run from the source directory, with the build directory as the first
# argument after the script (CMAKE_ARGV3).
file(COPY cli/cycpp.py DESTINATION "${CMAKE_ARGV3}/cyclus")
file(COPY cyclus setup.py pyproject.toml README.rst LICENSE.rst DESTINATION "${CMAKE_ARGV3}"
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
                    PATTERN "*.toml"
                    )
