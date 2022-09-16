cmake_minimum_required(VERSION 3.10)
project(ycutils)
set(TARGET_NAME ${PROJECT_NAME})

option(YCU_BUILD_WITH_D3D12 "Build with D3D12" ON)

# this macro adds a directory of source files to a source file list
macro(add_source out_list dir)
    file(GLOB_RECURSE temp_list "${dir}/*.cpp" "${dir}/*.h")
    list(APPEND ${out_list} ${temp_list})
endmacro()

# collect source files
set(SUB_LIBS math win32 opengl mesh log event)
file(GLOB SRC_FILES "ycu/*.h")
foreach(var ${SUB_LIBS})
    add_source(SRC_FILES "ycu/${var}")
endforeach()
if (YCU_BUILD_WITH_D3D12)
    add_source(SRC_FILES "ycu/d3d12")
endif()
source_group(TREE "${PROJECT_SOURCE_DIR}" FILES ${SRC_FILES})

# add library target
add_library(${TARGET_NAME}  STATIC ${SRC_FILES})

# add dependency targets
add_library               (glad STATIC "ycu/deps/glad/src/glad.c")
target_include_directories(glad PRIVATE "ycu/deps/glad/include")
add_subdirectory("ycu/deps/tinyobj")

# configure library target
target_compile_features     (${TARGET_NAME} PUBLIC cxx_std_17)
target_link_directories     (${TARGET_NAME} PUBLIC "ycu/deps/glfw/lib")
target_link_libraries       (${TARGET_NAME} PUBLIC glfw3 glad tinyobj)
target_include_directories  (${TARGET_NAME} PUBLIC
    "."
    "ycu/deps/glad/include"
    "ycu/deps/glfw/include"
    "ycu/deps/spdlog")
