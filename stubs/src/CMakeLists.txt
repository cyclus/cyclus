INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS} ${CYCLUS_CORE_TEST_INCLUDE_DIR})

USE_CYCLUS("@lib_name@" "@agent_name@")

### INSERT NEW ARCHETYPES BELOW HERE ###
INSTALL_CYCLUS_MODULE("@lib_name@" "@lib_path@" "NONE")

SET(TestSource ${@lib_name@_TEST_CC} PARENT_SCOPE)

# install header files
FILE(GLOB h_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${h_files} DESTINATION include/@lib_name@ COMPONENT @lib_name@)
