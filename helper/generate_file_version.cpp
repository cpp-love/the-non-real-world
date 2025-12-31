/**
 * @file generate_file_version.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 生成文件版本历史的模板文件
 * @version 0.1.0-5
 * @date 2025-11-8
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

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

using namespace std::string_literals;

/// @brief 处理大小不合适的参数
void processUnsuitedArgs() {
    std::println(":: 错误：参数过多或过少，请输入正确数量的参数，参数的正确顺序为<workspace_folder> "
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
        return {};
    }
    std::string date;
    if (fin >> date) {
        if (date == "/**") {
            while (fin >> date) {
                if (date == "@date") {
                    fin >> date;
                    return date;
                }
                if (date == "*/") {
                    break;
                }
            }
        }
    }
    return {};
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

    const auto file_ext = file.extension();
    if (std::filesystem::exists(output_file)
        || (file_ext != ".cpp" && file_ext != ".hpp" && file_ext != ".c" && file_ext != ".h"
            && file_ext != ".cc" && file_ext != ".hh" && file_ext != ".cxx" && file_ext != ".hxx")) {
        // 避免覆盖文件和生成无关文件
        return true;
    }

    std::error_code err_code; //< 错误码
    std::filesystem::create_directories(output_file.parent_path(),
                                        err_code); //< 创建输出文件
    if (err_code) {
        std::println(stderr, ":: 错误：{}", err_code.message());
        return false;
    }

    // 创建输出流
    std::ofstream output(output_file, std::ios::binary); //< 输出文件输出流
    if (!output) {
        std::println(stderr, ":: 错误：打开输出文件（位置：{}）失败", output_file.generic_string());
        return false;
    }

    // 获得当前日期（根据文件或当前日期）
    std::string date_string = getCreateDate(file);
    if (date_string.empty()) {
        std::chrono::zoned_time time(std::chrono::current_zone(), std::chrono::system_clock::now());
        date_string = std::format("{:%F}", time);
    }

    std::string file_name = file.filename().generic_string(); //< 文件名称
    std::string formatted_string =
        std::vformat(file_string_view, std::make_format_args(file_name, date_string, authors));
    output.write(formatted_string.data(), static_cast<std::streamsize>(formatted_string.size()));
    return true;
}

/**
 * @brief 生成单个文件夹的版本文件
 * @param [in] workspace_folder 工作区目录
 * @param [in] directory 文件夹位置
 * @param [in] authors 作者（们）
 * @return true 生成成功
 * @return false 生成失败
 */
bool generateOneDirectory(
    const std::filesystem::path &workspace_folder, // NOLINT(bugprone-easily-swappable-parameters)
    const std::filesystem::path &directory, std::string_view authors,
    std::string_view file_string_view) {
    auto process_entry = [&](const std::filesystem::directory_entry &entry) -> bool {
        if (entry.is_directory()) {
            return generateOneDirectory(workspace_folder, entry.path(), authors, file_string_view);
        }
        if (entry.is_regular_file()) {
            return generateOneFile(workspace_folder, entry.path(), authors, file_string_view);
        }
        std::println(":: 错误：{} 不是文件（夹）", entry.path().generic_string());
        return false;
    };

    return std::ranges::all_of(std::filesystem::directory_iterator(directory), process_entry);
}

int main(int argc, char *argv[]) {

    std::vector<char *>   args(argv,
                               argv + argc); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    // 处理命令行参数或接收输入
    std::filesystem::path workspace_folder; //< 工作区目录
    std::filesystem::path file;             //< 文件路径
    std::string           authors;          //< 作者
    if (argc == 1) {
        std::println(":: 请输入工作区目录：");
        std::string tmp; //< 临时输入存储
        std::getline(std::cin, tmp);
        std::error_code err_code; //< 错误码
        workspace_folder = std::filesystem::canonical(tmp, err_code);
        if (err_code) {
            std::println(stderr, ":: 错误：{}", err_code.message());
            return -1;
        }
        std::println(":: 请输入文件路径：");
        std::getline(std::cin, tmp);
        file = std::filesystem::canonical(tmp, err_code);
        if (err_code) {
            std::println(stderr, ":: 错误：{}", err_code.message());
            return -1;
        }
        std::println(":: 请输入作者：");
        std::getline(std::cin, authors);
    } else if (argc == 4) {
        std::error_code err_code; //< 错误码
        workspace_folder = std::filesystem::canonical(args[1], err_code);
        if (err_code) {
            std::println(stderr, ":: 错误：{}", err_code.message());
            return -1;
        }
        file = std::filesystem::canonical(args[2], err_code);
        if (err_code) {
            std::println(stderr, ":: 错误：{}", err_code.message());
            return -1;
        }
        authors = args[3];
    } else {
        processUnsuitedArgs();
        return -1;
    }
    if (!std::filesystem::is_directory(workspace_folder)) {
        std::println(":: 错误：工作区目录指向位置不是一个目录");
        return -1;
    }

    // 判断是否有效
    bool is_file = std::filesystem::is_regular_file(file);
    if (!is_file && !std::filesystem::is_directory(file)) {
        std::println("错误：文件路径指向位置不是文件（夹）");
        return -1;
    }

    // 创建输入流
    std::filesystem::path template_file(workspace_folder / "helper"s
                                        / "file_version.template.md"s); //< 模板文件目录
    std::ifstream         input(template_file,
                                std::ios::binary      // NOLINT(hicpp-signed-bitwise)
                                    | std::ios::ate); //< 模板文件输入流
    if (!input) {
        std::println(stderr, ":: 错误：打开模板文件（位置：{}）失败", template_file.generic_string());
        return -1;
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