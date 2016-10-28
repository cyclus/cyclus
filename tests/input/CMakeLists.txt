
FILE(GLOB cyclus_infiles "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")

INSTALL(FILES
    ${cyclus_infiles}
    DESTINATION share/cyclus/input
    COMPONENT testing
    )
