set(TARGET_NAME tinyobj)
add_library                 (${TARGET_NAME} STATIC "tiny_obj_build.cpp")
target_include_directories  (${TARGET_NAME} INTERFACE ".")