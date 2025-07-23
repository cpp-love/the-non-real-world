/**
 * @file generate_file_version.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 生成文件版本历史的模板文件
 * @version 0.1.0-2
 * @date 2025-07-05
 * 
 * @copyright Copyright 2025 cpp-love
 * 
 * @details
 * - 这个文件是用来通过命令行参数生成文件版本历史的模板文件。
 * - 使用方法：`.\bin\generate_file_version <file_directory> <file_name> [additional_content]` 或直接运行 `.\bin\generate_file_version` 后根据提示操作
 *   - `<file_directory>` 是文件所在的目录
 *   - `<file_name>` 是文件名
 *   - `[additional_content]` 是可选的附加说明内容
 *   - 生成的文件将保存在 `file_versions/<file_directory>/<file_name>` 
 *   - 生成的文件内容将基于 `template/file_version.template.md` 模板文件
 */

#include <chrono>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <spdlog/fmt/bundled/format.h>
#include <spdlog/fmt/bundled/ostream.h>
#include <string>

using namespace std::literals::string_literals; //< 使用字符串字面量

constexpr int buffer_size = 1024; //< 缓冲区大小
constexpr int offset_size = 2;    //< 偏移量大小
/**
 * @brief 用于标记格式化字符串需处理的类型
 */
enum class BufferKind { Normal, required1FollowingChar, required2FollowingChars };

/**
 * @brief 处理缓冲区的格式化字符串
 * @details 根据缓冲区的格式化字符串的内容，判断其类型并返回相应的枚举值
 * @param [in] buffer 缓冲区的格式化字符串
 * @param [in] bytes_read 有效字符串
 * @return FmtStringKind 返回处理后的格式化字符串类型
 */
inline BufferKind processBuffer(const char *buffer, std::streamsize bytes_read) {
    if (buffer[bytes_read - 2] != '{')
        return BufferKind::Normal;
    if (buffer[bytes_read - 3] == '{')
        return BufferKind::required2FollowingChars;
    return BufferKind::required1FollowingChar;
}

int main(int argc, char *argv[]) {
    // 处理命令行参数或接收输入
    std::string input_dir;          //< 输入目录
    std::string file_name;          //< 文件名
    std::string additional_content; //< 附加内容
    std::string authors =
        "[cpp-love(15865418+cpp-love@user.noreply.gitee.com)](15865418+cpp-love@user.noreply.gitee.com)"s; //< 作者信息
    if (argc < 3) {
        fmt::println("请输入文件目录（不以/结尾）：");
        std::cin >> input_dir;
        fmt::println("请输入文件名：");
        std::cin >> file_name;
        fmt::println("请输入附加内容：");
        std::cin >> additional_content;
    } else {
        input_dir = argv[1];
        file_name = argv[2];
        if (argc > 3) {
            additional_content = argv[3];
        }
    }
    std::filesystem::path file_directory("../file_versions/"s
                                         + input_dir); //< 文件所在目录

    // 定义缓冲区和打开文件
    char                  buffer[buffer_size + offset_size]; //< 缓冲区及偏移量
    std::filesystem::path template_file_path(
        "../helper/file_version.template.md"s);                        //< 模板文件路径
    std::ifstream template_file(template_file_path, std::ios::binary); //< 模板文件流
    if (!template_file) {
        fmt::println(stderr, "error: Failed to open template file(at: {})!",
                     template_file_path.generic_string());
        return -1;
    }
    std::filesystem::path output_file_path(file_directory
                                           / (file_name + ".md")); //< 输出文件路径
    std::ofstream         output_file(output_file_path, std::ios::binary); //< 输出文件流
    if (!output_file) {
        fmt::println(stderr, "error: Failed to open output file(at: {})!",
                     output_file_path.generic_string());
        return -1;
    }

    // 获得当前日期
    std::time_t now =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm    *tm = std::localtime(&now);
    std::string date_string = fmt::format("{:04d}-{:02d}-{:02d}", tm->tm_year + 1900,
                                          tm->tm_mon + 1, tm->tm_mday);

    // 读取模板文件并输出到输出文件
    std::streamsize bytes_read = 0; //< 读取的字节数
    while (true) {
        template_file.read(buffer, buffer_size); //< 从模板文件中读取数据到缓冲区
        bytes_read = template_file.gcount();     //< 获取实际读取的字节数

        // 处理读取错误
        if (template_file.bad()) {
            fmt::println(stderr, "error: Failed to read template file(at: {})!",
                         template_file_path.generic_string());
            fmt::println(stderr, "errorinfo: Read {} bytes, but expected {} bytes!",
                         bytes_read, buffer_size);
            fmt::println(stderr,
                         "errorinfo: the file is bad:{}, fail:{}, eof:{}, good:{}",
                         template_file.bad(), template_file.fail(), template_file.eof(),
                         template_file.good());
            fmt::println(stderr, "errorinfo: the buffer now is : \n{}",
                         std::string_view(buffer, bytes_read));
            return -1;
        }

        // 如果没有读取到数据，则退出循环
        if (bytes_read == 0) {
            break;
        }

        // 处理缓冲区的格式化字符串并根据缓冲区的内容进行格式化输出
        BufferKind kind = processBuffer(buffer, bytes_read);
        switch (kind) {
            case BufferKind::required1FollowingChar:
                template_file.get(buffer[bytes_read]);
                ++bytes_read;
                break;
            case BufferKind::required2FollowingChars:
                template_file.get(buffer[bytes_read]);
                template_file.get(buffer[bytes_read + 1]);
                bytes_read += 2;
                break;
            case BufferKind::Normal:
                break;
        }

        std::string_view fmtStringView(buffer, bytes_read); //< 格式化字符串视图
        try {
            fmt::vprint(output_file, fmtStringView,
                        fmt::make_format_args(file_name, date_string, authors,
                                              additional_content));
        } catch (const fmt::format_error &e) {
            fmt::println(stderr, "error: Failed to format output file(at: {})!",
                         output_file_path.generic_string());
            fmt::println(stderr, "errorinfo: {}", e.what());
            return -1;
        }

        // 如果读到的数据小于缓冲区大小，则说明已经读到文件末尾，退出循环
        if (bytes_read < buffer_size) {
            break;
        }
    }

    return 0;
}