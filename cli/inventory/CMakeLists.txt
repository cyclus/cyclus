
FIND_PROGRAM(GO_BIN go)

IF(GO_BIN)
MESSAGE("-- Found Go compiler - adding inventory tool to build.")

ADD_CUSTOM_TARGET(
  inventory ALL
  go get code.google.com/p/go-sqlite/go1/sqlite3 
  COMMAND go build
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  SOURCES util.go walker.go main.go
  )

INSTALL(
  PROGRAMS inventory 
  DESTINATION bin
  COMPONENT analysis
  )

ENDIF()
