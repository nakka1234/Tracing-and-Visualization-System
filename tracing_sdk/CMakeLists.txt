add_library(tracing_sdk
    src/span.cpp
    src/context.cpp
)

target_include_directories(tracing_sdk PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(tracing_sdk PUBLIC
    shared
    logging_engine
)