option(TNRW_BUILD_HELPER "Build The Non-\"real\" World helper programs" OFF)

# 添加测试
enable_testing()

if(TNRW_BUILD_HELPER)
    # 设置输出目录
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    # 构建帮助程序
    add_executable(generate_file_version ${CMAKE_CURRENT_SOURCE_DIR}/generate_file_version.cpp)
    target_link_libraries(generate_file_version
        PUBLIC
        tnrw_compiler_flags
    )

    # 添加安装支持
    install(TARGETS generate_file_version DESTINATION bin)
    install(PROGRAMS generate_code_issues.py generate_test_expressions_data.py DESTINATION bin)

    # 添加测试支持
    add_test(NAME RunHelper COMMAND generate_file_version ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/helper author)
endif()
