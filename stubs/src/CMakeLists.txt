INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS} ${CYCLUS_CORE_TEST_INCLUDE_DIR})

### DO NOT DELETE THIS COMMENT: INSERT_ARCHETYPES_HERE ###
USE_CYCLUS("@lib_name@" "@agent_name@")

INSTALL_CYCLUS_MODULE("@lib_name@" "@lib_path@")

SET(TestSource ${@lib_name@_TEST_CC} PARENT_SCOPE)

# install header files
FILE(GLOB h_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${h_files} DESTINATION include/@lib_name@ COMPONENT @lib_name@)
