add_executable(workbench_experiments
    main.cpp
)

target_link_libraries(workbench_experiments
    PRIVATE
        workbench_core
)

target_compile_features(workbench_experiments PRIVATE cxx_std_17)

set_target_properties(workbench_experiments PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

