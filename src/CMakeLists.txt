# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找当前目录下所有 cpp 文件
file(GLOB_RECURSE SRC_FILES *.cpp)

# 生成库
add_library(tnrw ${SRC_FILES})

# 查找第三方库
include(FetchContent)

FetchContent_Declare(
    ENTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG master
    FIND_PACKAGE_ARGS 3.15.0 CONFIG REQUIRED
)
FetchContent_MakeAvailable(ENTT)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG master
    FIND_PACKAGE_ARGS 1.15.3 CONFIG REQUIRED
)
FetchContent_MakeAvailable(spdlog)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG master
    FIND_PACKAGE_ARGS 3.0.1 COMPONENTS Graphics Window System CONFIG REQUIRED
)
FetchContent_MakeAvailable(SFML)

# find_package(TGUI CONFIG REQUIRED)
# find_package(nlohmann_json CONFIG REQUIRED)

# 包含头文件目录
target_include_directories(tnrw
    PUBLIC
    ${EnTT_INCLUDE_DIRS}
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
    spdlog::spdlog
    SFML::Graphics
    SFML::Window
    SFML::System
    # TGUI
    # nlohmann_json::nlohmann_json
)

# 添加安装支持
install(TARGETS tnrw DESTINATION lib)
