cmake_minimum_required(VERSION 3.15)
project(NakuyaMonoRepo)
set(CMAKE_C_STANDARD 23)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/game-architecture.cmake)

include_directories(include)
add_subdirectory(external)
add_subdirectory(apps)
