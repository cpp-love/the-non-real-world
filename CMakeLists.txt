cmake_minimum_required(VERSION 3.28)
project(TheNon-realWorld VERSION 0.1.0 DESCRIPTION "The Non-\"real\" World（《不“真实”的世界》）：一款关于不“现实”的数——复数的游戏，通过先前探索者遗留下来的纸条，探索这个复数世界的奥秘。" LANGUAGES CXX)

# 取消编译器扩展
set(CMAKE_CXX_EXTENSIONS OFF "Disable C++ compiler extensions")

# 取消模块
set(CMAKE_CXX_SCAN_FOR_MODULES OFF "Disable CMake C++ modules support")

# 添加测试
enable_testing()

# 添加版本
configure_file(${PROJECT_SOURCE_DIR}/include/base/config.hpp.in ${PROJECT_SOURCE_DIR}/include/base/config.hpp)
configure_file(${PROJECT_SOURCE_DIR}/Doxyfile.internal.in ${PROJECT_SOURCE_DIR}/Doxyfile.internal)
configure_file(${PROJECT_SOURCE_DIR}/Doxyfile.in ${PROJECT_SOURCE_DIR}/Doxyfile)

# 添加编译器标志
add_library(tnrw_compiler_flags INTERFACE)

# 添加编译器版本
target_compile_features(tnrw_compiler_flags INTERFACE cxx_std_23)

# 添加编译器警告标志
set(gcc_like_cxx $<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>)
set(msvc_cxx $<COMPILE_LANG_AND_ID:CXX,MSVC>)
target_compile_options(tnrw_compiler_flags
    INTERFACE
    $<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wpedantic>>
    $<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>
)

# 添加优化
target_compile_options(tnrw_compiler_flags
    INTERFACE
    $<${gcc_like_cxx}:$<BUILD_INTERFACE:-O2>>
)

# 添加gcc对c++23 `print` / `println` 函数的 `Windows` 版本的支持
set(gcc_cxx $<COMPILE_LANG_AND_ID:CXX,GNU>)
if(gcc_cxx)
    target_link_libraries(tnrw_compiler_flags
        INTERFACE
        $<BUILD_INTERFACE:-lstdc++exp>
    )
endif()

# 包含头文件目录
target_include_directories(tnrw_compiler_flags
    INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

# 添加调试/发行宏
target_compile_definitions(tnrw_compiler_flags
    INTERFACE
    $<BUILD_INTERFACE:$<$<CONFIG:Debug>:DEBUG=1>>
    $<BUILD_INTERFACE:$<$<CONFIG:Debug>:_DEBUG=1>>
    $<BUILD_INTERFACE:$<$<CONFIG:Release>:NDEBUG=1>>
)

# 添加子目录
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(helper)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 设置链接文件
# add_executable(TheNon-realWorld ${PROJECT_SOURCE_DIR}/tests/base/test_string_convert.cpp)

# 链接第三方库
# target_link_libraries(TheNon-realWorld
# PUBLIC
# tnrw
# tnrw_compiler_flags
# )

# 添加安装支持
# install(TARGETS TheNon-realWorld DESTINATION bin)

# 添加测试支持
# add_test(NAME Runs COMMAND TheNon-realWorld)
# add_test(NAME StandardUse COMMAND TheNon-realWorld)
# set_tests_properties(StandardUse
# PROPERTIES PASS_REGULAR_EXPRESSION "这是一个类型为 const char8_t \\* 类型的字符串 这是一个类型为 std::u8string 类型的字符串\\s*"
# )
