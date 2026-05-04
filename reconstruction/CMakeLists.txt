add_library(reconstruction_lib
    src/span_builder.cpp
    src/printer.cpp
)

target_include_directories(reconstruction_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(reconstruction_lib PUBLIC shared)

add_executable(reconstruction
    src/main.cpp
)

target_link_libraries(reconstruction
    reconstruction_lib
)