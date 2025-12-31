# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找当前目录下所有 cpp 文件
file(GLOB_RECURSE SRC_FILES *.cpp)

# 生成库
add_library(tnrw ${SRC_FILES})

# 查找第三方库
find_package(EnTT CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(SFML 3 COMPONENTS Graphics Window System CONFIG REQUIRED)
# find_package(TGUI CONFIG REQUIRED)
# find_package(nlohmann_json CONFIG REQUIRED)

# 包含头文件目录
target_include_directories(tnrw
    PUBLIC
    ${EnTT_INCLUDE_DIRS}
    ${fmt_INCLUDE_DIRS}
    ${spdlog_INCLUDE_DIRS}
    ${SFML_INCLUDE_DIRS}
    # ${TGUI_INCLUDE_DIRS}
    # ${nlohmann_json_INCLUDE_DIRS}
)

# 链接编译器标志
target_link_libraries(tnrw
    PUBLIC
    tnrw_compiler_flags
    EnTT::EnTT
    fmt::fmt
    spdlog::spdlog
    SFML::Graphics
    SFML::Window
    SFML::System
    # TGUI
    # nlohmann_json::nlohmann_json
)

# 添加安装支持
install(TARGETS tnrw DESTINATION lib)
