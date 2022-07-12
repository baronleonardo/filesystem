add_executable(test main.c)
target_link_libraries(test PRIVATE ${PROJECT_NAME})
target_compile_definitions(test PRIVATE "-DTESTS_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}\"")