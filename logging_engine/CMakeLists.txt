add_library(logging_engine
    src/logger.cpp
)

target_include_directories(logging_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(logging_engine PUBLIC shared)