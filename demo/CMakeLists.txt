add_executable(demo main.cpp)

target_link_libraries(demo
    tracing_sdk
    logging_engine
)