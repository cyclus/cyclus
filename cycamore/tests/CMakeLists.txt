# Find tests generated in the binary directory
message("GENERATED TESTS DIR ${CMAKE_CURRENT_BINARY_DIR}")
message("GENERATED TESTS ${GENERATED_FILTER_LIST}")

MESSAGE("CYClUS EXECUTABLE PATH = ${CYCLUS_ROOT_DIR}/bin")

# Configure the input running script
SET(input_path ${PROJECT_SOURCE_DIR}/../input)
SET(cyclus_path ${CYCLUS_ROOT_DIR}/bin)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/run_inputs.py.in
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/run_inputs.py @ONLY)

SET(input_path ${PROJECT_SOURCE_DIR}/input)
SET(cyclus_path ${CYCLUS_ROOT_DIR}/bin)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/run_inputs.py.in
    ${CMAKE_CURRENT_SOURCE_DIR}/run_inputs.py @ONLY)

INSTALL(FILES run_inputs.py
    DESTINATION bin
    COMPONENT testing
    )
