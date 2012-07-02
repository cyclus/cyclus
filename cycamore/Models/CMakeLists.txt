# ------------------- Add all Concrete Models ----------------------------
FILE(GLOB all_valid_subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*/CMakeLists.txt")

FOREACH(dir ${all_valid_subdirs})
  IF(${dir} MATCHES "^([^/]*)//CMakeLists.txt")
    STRING(REGEX REPLACE "^([^/]*)//CMakeLists.txt" "\\1" dir_trimmed ${dir})
    ADD_SUBDIRECTORY(${dir_trimmed})
  ENDIF()
ENDFOREACH(dir)

SET( TestSource ${ModelTestSource} PARENT_SCOPE)

# --------------------- Configure the cyclus.rng file -------------------
CONFIGURE_FILE(${PROJECT_BINARY_DIR}/share/cyclus.rng 
  ${PROJECT_BINARY_DIR}/share/cyclus.rng @ONLY)

