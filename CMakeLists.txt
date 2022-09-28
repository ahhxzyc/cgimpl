cmake_minimum_required(VERSION 3.10)
project(cgimpl)
add_subdirectory(lib)

set(CHAPTERS ssao ibl)

foreach(chapter ${CHAPTERS})
    file(GLOB src_files "src/${chapter}/*.cpp")
    foreach(file ${src_files})
        get_filename_component(filename ${file} NAME_WE)
        # create a target for each source file
        set(target_name "${chapter}_${filename}")
        add_executable       (${target_name} ${file})
        target_link_libraries(${target_name} PRIVATE ycutils)
        set_target_properties(${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${chapter}")
        set_target_properties(${target_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${chapter}/Debug")
    endforeach()
endforeach()