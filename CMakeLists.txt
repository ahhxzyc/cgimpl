cmake_minimum_required(VERSION 3.10)
project(test)
set(TARGET_NAME ${PROJECT_NAME})

add_executable(${TARGET_NAME} "test/main.cpp")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${TARGET_NAME} ycutils)