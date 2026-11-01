add_executable(workbench_core_tests
    test_main.cpp
)

target_link_libraries(workbench_core_tests
    PRIVATE
        workbench_core
)

target_compile_features(workbench_core_tests PRIVATE cxx_std_17)

add_test(
    NAME workbench_core_tests
    COMMAND workbench_core_tests
)

add_executable(workbench_thermal_tests
    test_thermal.cpp
)

target_link_libraries(workbench_thermal_tests
    PRIVATE
        workbench_core
)

target_compile_features(workbench_thermal_tests PRIVATE cxx_std_17)

add_test(
    NAME workbench_thermal_tests
    COMMAND workbench_thermal_tests
)

add_executable(workbench_parser_tests
    test_parsers.cpp
)

target_link_libraries(workbench_parser_tests
    PRIVATE
        workbench_core
)

target_compile_features(workbench_parser_tests PRIVATE cxx_std_17)

add_test(
    NAME workbench_parser_tests
    COMMAND workbench_parser_tests
)

add_executable(workbench_performance_tests
    test_performance.cpp
)

target_link_libraries(workbench_performance_tests
    PRIVATE
        workbench_core
)

target_compile_features(workbench_performance_tests PRIVATE cxx_std_17)

add_test(
    NAME workbench_performance_tests
    COMMAND workbench_performance_tests
)

add_executable(workbench_embedded_tests
    test_embedded.cpp
)

target_link_libraries(workbench_embedded_tests
    PRIVATE
        workbench_core
)

target_compile_features(workbench_embedded_tests PRIVATE cxx_std_17)

add_test(
    NAME workbench_embedded_tests
    COMMAND workbench_embedded_tests
)

add_executable(workbench_backprop_tests
    test_backprop.cpp
)

target_link_libraries(workbench_backprop_tests
    PRIVATE
        workbench_core
)

target_compile_features(workbench_backprop_tests PRIVATE cxx_std_17)

add_test(
    NAME workbench_backprop_tests
    COMMAND workbench_backprop_tests
)

# Test deliberate validation failure handling:
# 1. Isolates output to a temporary binary directory so project results/ is never touched.
# 2. Verifies that the process returns exit code 1 exactly via CMake script wrapper.
add_test(
    NAME workbench_experiments_failure_exit_code_test
    COMMAND ${CMAKE_COMMAND}
        -DEXECUTABLE=$<TARGET_FILE:workbench_experiments>
        -DEXPECTED_CODE=1
        -DSIMULATE_FAILURE=TRUE
        -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/simulated_failure_test_output
        -P ${CMAKE_CURRENT_SOURCE_DIR}/verify_exit_code.cmake
)

set_tests_properties(
    workbench_core_tests
    workbench_thermal_tests
    workbench_parser_tests
    workbench_performance_tests
    workbench_embedded_tests
    workbench_backprop_tests
    workbench_experiments_failure_exit_code_test
    PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Automated Qt GUI Integration Tests (exercising MainWindow, all 5 views, widgets, and workflows)
find_package(Qt6 COMPONENTS Test Widgets OpenGLWidgets QUIET)
if(Qt6_FOUND)
    add_executable(workbench_gui_tests
        test_gui_integration.cpp
    )

    target_link_libraries(workbench_gui_tests PRIVATE
        workbench_app_lib
        Qt6::Test
    )

    set_target_properties(workbench_gui_tests PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )

    add_test(
        NAME workbench_gui_tests
        COMMAND workbench_gui_tests
    )

    set_tests_properties(workbench_gui_tests PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()
