option(TNRW_BUILD_HELPER "Build The Non-\"real\" World helper programs" OFF)

# 添加测试
enable_testing()

if(TNRW_BUILD_HELPER)
    # 设置输出目录
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    # 构建帮助程序
    find_package(fmt REQUIRED)
    add_executable(generate_file_version ${PROJECT_SOURCE_DIR}/helper/generate_file_version.cpp)
    target_link_libraries(generate_file_version
        PUBLIC
        tnrw_compiler_flags
        fmt::fmt)

    # 添加安装支持
    install(TARGETS generate_file_version DESTINATION bin)

    # 添加测试支持
    add_test(NAME RunHelper COMMAND generate_file_version ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/helper author)
endif()
