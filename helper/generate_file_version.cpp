/**
 * @file generate_file_version.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 生成文件版本历史的模板文件
 * @version 0.1.0-4
 * @date 2025-10-1
 * 
 * @copyright cpp-love
 * 
 * @details
 * - 这个文件是用来通过命令行参数生成文件版本历史的模板文件。
 * - 使用方法：`.\bin\exes\generate_file_version <workspace_folder> <file> <authors>` 或直接运行 `.\bin\exes\generate_file_version` 后根据提示操作
 *   - `<workspace_folder>` 是工作区目录
 *   - `<file>` 是文件（夹）路径
 *   - `<authors>` 是作者名称
 *   - `<file_relative>` 是文件或文件夹内的子文件相对工作区的路径（运行时根据 `<workspace_folder>` 和 `<file>` 推导出来的）
 *   - 生成的文件将保存在 `file_versions/<file_relative>`
 *   - 生成的文件内容将基于 `<workspace_folder>/helper/file_version.template.md` 模板文件
 */

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

using namespace std::literals::string_literals;

/// @brief 处理大小不合适的参数
void processUnsuitedArgs() {
    fmt::println(":: 错误：参数过多或过少，请输入正确数量的参数，参数的正确顺序为<workspace_folder> "
                 "<file> <authors>");
}

/**
 * @brief 获取文件创建日期
 * @param [in] file 文件位置
 * @return std::string 文件的创建日期，如果获取不到则为空
 * @note 此函数是根据文件头的 `Doxygen` 注释中的 `@date` 推导出来的
 */
std::string getCreateDate(const std::filesystem::path &file) {
    std::ifstream fin(file);
    if (!fin) {
        return std::string();
    }
    std::string date;
    if (fin >> date) {
        if (date == "/**") {
            while (fin >> date) {
                if (date == "@date") {
                    fin >> date;
                    return date;
                } else if (date == "*/") {
                    break;
                }
            }
        }
    }
    return std::string();
}

/**
 * @brief 生成单个文件的版本文件
 * @param [in] workspace_folder 工作区目录
 * @param [in] file 文件位置
 * @param [in] authors 作者（们）
 * @return true 生成成功
 * @return false 生成失败
 */
bool generateOneFile(const std::filesystem::path &workspace_folder, const std::filesystem::path &file,
                     std::string_view authors, std::string_view file_string_view) {
    std::filesystem::path output_file(
        workspace_folder / "file_versions"s
        / std::filesystem::relative(file, workspace_folder).concat(".md"s)); //< 输出文件目录

    const auto fileExt = file.extension();
    if (std::filesystem::exists(output_file)
        || (fileExt != ".cpp" && fileExt != ".hpp" && fileExt != ".c" && fileExt != ".h"
            && fileExt != ".cc" && fileExt != ".hh" && fileExt != ".cxx" && fileExt != ".hxx")) {
        // 避免覆盖文件和生成无关文件
        return true;
    }

    std::error_code ec; //< 错误码
    std::filesystem::create_directories(output_file.parent_path(),
                                        ec); //< 创建输出文件
    if (ec) {
        fmt::println(stderr, ":: 错误：{}", ec.message());
        return false;
    }

    // 创建输出流
    std::ofstream output(output_file, std::ios::binary); //< 输出文件输出流
    if (!output) {
        fmt::println(stderr, ":: 错误：打开输出文件（位置：{}）失败", output_file.generic_string());
        return false;
    }

    // 获得当前日期（根据文件或当前日期）
    std::string date_string = getCreateDate(file);
    if (date_string.empty()) {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm    *tm = std::localtime(&now);
        date_string =
            fmt::format("{:04d}-{:02d}-{:02d}", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    }

    std::string file_name = file.filename().generic_string(); //< 文件名称
    std::string formatted_string =
        fmt::vformat(file_string_view, fmt::make_format_args(file_name, date_string, authors));
    output.write(formatted_string.data(), formatted_string.size());
    return true;
}

/**
 * @brief 生成单个文件夹的版本文件
 * @param [in] workspace_folder 工作区目录
 * @param [in] file 文件夹位置
 * @param [in] authors 作者（们）
 * @return true 生成成功
 * @return false 生成失败
 */
bool generateOneDirectory(const std::filesystem::path &workspace_folder,
                          const std::filesystem::path &directory, std::string_view authors,
                          std::string_view file_string_view) {
    for (const auto &file : std::filesystem::directory_iterator(directory)) {
        if (std::filesystem::is_directory(file)) {
            if (!generateOneDirectory(workspace_folder, file, authors, file_string_view)) {
                return false;
            }
        } else if (std::filesystem::is_regular_file(file)) {
            if (!generateOneFile(workspace_folder, file, authors, file_string_view)) {
                return false;
            }
        } else {
            fmt::println(":: 错误：{} 不是文件（夹）",
                         static_cast<const std::filesystem::path &>(file).generic_string());
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    // 处理命令行参数或接收输入
    std::filesystem::path workspace_folder; //< 工作区目录
    std::filesystem::path file;             //< 文件路径
    std::string           authors;          //< 作者
    if (argc == 1) {
        fmt::println(":: 请输入工作区目录：");
        std::string tmp; //< 临时输入存储
        std::getline(std::cin, tmp);
        std::error_code ec; //< 错误码
        workspace_folder = std::filesystem::canonical(tmp, ec);
        if (ec) {
            fmt::println(stderr, ":: 错误：{}", ec.message());
            return -1;
        }
        fmt::println(":: 请输入文件路径：");
        std::getline(std::cin, tmp);
        file = std::filesystem::canonical(tmp, ec);
        if (ec) {
            fmt::println(stderr, ":: 错误：{}", ec.message());
            return -1;
        }
        fmt::println(":: 请输入作者：");
        std::getline(std::cin, authors);
    } else if (argc == 4) {
        std::error_code ec; //< 错误码
        workspace_folder = std::filesystem::canonical(argv[1], ec);
        if (ec) {
            fmt::println(stderr, ":: 错误：{}", ec.message());
            return -1;
        }
        file = std::filesystem::canonical(argv[2], ec);
        if (ec) {
            fmt::println(stderr, ":: 错误：{}", ec.message());
            return -1;
        }
        authors = argv[3];
    } else {
        processUnsuitedArgs();
        return -1;
    }
    if (!std::filesystem::is_directory(workspace_folder)) {
        fmt::println(":: 错误：工作区目录指向位置不是一个目录");
        return -1;
    }

    // 判断是否有效
    bool is_file = std::filesystem::is_regular_file(file);
    if (!is_file && !std::filesystem::is_directory(file)) {
        fmt::println("错误：文件路径指向位置不是文件（夹）");
        return -1;
    }

    // 创建输入流
    std::filesystem::path template_file(workspace_folder / "helper"s
                                        / "file_version.template.md"s); //< 模板文件目录
    std::ifstream         input(template_file,
                                std::ios::binary | std::ios::ate); //< 模板文件输入流
    if (!input) {
        fmt::println(stderr, ":: 错误：打开模板文件（位置：{}）失败", template_file.generic_string());
        return false;
    }
    // 文件不大，直接整个读取
    std::streamsize   file_size = input.tellg(); //< 模板文件大小
    std::vector<char> file_data(file_size);      //< 文件存储
    input.seekg(0);                              //< 将输入流重定向到开头
    input.read(file_data.data(), file_size);
    std::string_view file_string_view(file_data.data(), file_data.size()); //< 文件内容

    if (is_file) {
        // 是单个文件
        if (!generateOneFile(workspace_folder, file, authors, file_string_view)) {
            return -1;
        }
    } else {
        // 是目录
        if (!generateOneDirectory(workspace_folder, file, authors, file_string_view)) {
            return -1;
        }
    }

    return 0;
}