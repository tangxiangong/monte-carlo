cmake_minimum_required(VERSION 4.0)
project(montecarlo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# macOS 特定配置
if(APPLE)
    execute_process(
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE MACOS_SDK_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isysroot ${MACOS_SDK_PATH}")
endif()

# 设置输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# 收集所有的模块文件
file(GLOB_RECURSE MODULE_SOURCES "src/*.cppm")


add_library(montecarlo SHARED)
target_sources(montecarlo
    PUBLIC
        FILE_SET CXX_MODULES FILES
        ${MODULE_SOURCES}
)
set_target_properties(montecarlo PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    CXX_SCAN_FOR_MODULES ON
)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE montecarlo)
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    CXX_SCAN_FOR_MODULES ON
)

add_custom_command(
    TARGET montecarlo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_BINARY_DIR}/compile_commands.json"
    "${PROJECT_SOURCE_DIR}/compile_commands.json"
    COMMENT "Copying compile_commands.json to project root"
)
