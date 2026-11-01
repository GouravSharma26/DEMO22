# verify_exit_code.cmake
# Executes an executable and verifies that its exit code matches EXPECTED_CODE exactly.
# Required variables:
#   EXECUTABLE       - Full path or target file of the executable
#   EXPECTED_CODE    - Expected integer exit code (e.g. 1)
# Optional variables:
#   SIMULATE_FAILURE - Boolean flag to pass --simulate-failure
#   OUTPUT_DIR       - Directory path to pass as --output-dir <OUTPUT_DIR>
#   EXTRA_ARGS       - Additional arguments (optional)

if (NOT DEFINED EXECUTABLE)
    message(FATAL_ERROR "EXECUTABLE variable not defined.")
endif()

if (NOT DEFINED EXPECTED_CODE)
    message(FATAL_ERROR "EXPECTED_CODE variable not defined.")
endif()

set(ARG_LIST "")

if (DEFINED SIMULATE_FAILURE AND SIMULATE_FAILURE)
    list(APPEND ARG_LIST "--simulate-failure")
endif()

if (DEFINED OUTPUT_DIR AND NOT "${OUTPUT_DIR}" STREQUAL "")
    list(APPEND ARG_LIST "--output-dir" "${OUTPUT_DIR}")
endif()

if (DEFINED EXTRA_ARGS AND NOT "${EXTRA_ARGS}" STREQUAL "")
    list(APPEND ARG_LIST ${EXTRA_ARGS})
endif()

execute_process(
    COMMAND "${EXECUTABLE}" ${ARG_LIST}
    RESULT_VARIABLE ACTUAL_EXIT_CODE
    OUTPUT_VARIABLE PROCESS_STDOUT
    ERROR_VARIABLE PROCESS_STDERR
)

message(STATUS "Process: ${EXECUTABLE}")
message(STATUS "Arguments: ${ARG_LIST}")
message(STATUS "Actual exit code: ${ACTUAL_EXIT_CODE}")
message(STATUS "Expected exit code: ${EXPECTED_CODE}")

if (NOT ACTUAL_EXIT_CODE EQUAL EXPECTED_CODE)
    message(FATAL_ERROR "Exit code mismatch! Expected ${EXPECTED_CODE}, got ${ACTUAL_EXIT_CODE}.\nSTDOUT:\n${PROCESS_STDOUT}\nSTDERR:\n${PROCESS_STDERR}")
endif()

message(STATUS "Exit code verification PASSED: process returned ${EXPECTED_CODE} as expected.")
