
FIND_PROGRAM(GO_BIN go)
SET(GOPATH $ENV{GOPATH})

IF(GO_BIN)
  MESSAGE("-- Found Go build tool: ${GO_BIN}")
  IF(GOPATH)
    MESSAGE("-- GOPATH is ${GOPATH}")
    MESSAGE("-- Adding inventory tool to build.")
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
  ELSE()
    MESSAGE(SEND_ERROR "-- GOPATH env var must be set to build inventory tool.")
  ENDIF()
ENDIF()
