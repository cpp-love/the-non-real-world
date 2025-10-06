# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找当前目录下所有 cpp 文件
file(GLOB_RECURSE SRC_FILES *.cpp)

# 生成库
add_library(tnrw ${SRC_FILES})

option(TNRW_USE_FMT_OUTPUT "Use fmt third party as the output.\n
This option is set in order to make it easier for others to use the base/assert_msg.hpp"    OFF)

# 查找第三方库
find_package(EnTT REQUIRED)
if(TNRW_USE_FMT_OUTPUT)
    find_package(fmt REQUIRED)
endif()
# find_package(spdlog REQUIRED)
# find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)
# find_package(TGUI REQUIRED)
# find_package(nlohmann_json REQUIRED)

# 链接编译器标志
if(TNRW_USE_FMT_OUTPUT)
    target_link_libraries(tnrw
        PUBLIC
        tnrw_compiler_flags
        EnTT::EnTT
        fmt::fmt
        # spdlog::spdlog
        # SFML::Graphics
        # SFML::Window
        # SFML::System
        # TGUI
        # nlohmann_json::nlohmann_json
    )
else()
    target_link_libraries(tnrw
        PUBLIC
        tnrw_compiler_flags
        EnTT::EnTT
        # spdlog::spdlog
        # SFML::Graphics
        # SFML::Window
        # SFML::System
        # TGUI
        # nlohmann_json::nlohmann_json
    )
endif()
