#!/usr/bin/env python
"""
输出C/C++代码问题（使用clang-tidy）到文件的脚本
使用方法：
见 `python generate_code_issues.py --help` 的结果
"""

import subprocess
import shutil
import re
import os
import json
from typing import Any
from pathlib import Path


def get_default_compiler_include_directories(compiler: Path) -> list[str]:
    """
    获取编译器默认包含路径

    :param compiler: 编译器路径
    :type compiler: Path
    :return: 包含路径列表
    :rtype: list[str]
    """

    includes: list[str] = []
    if re.match(r"gcc.*|g\+\+.*", compiler.absolute().resolve().name) or re.match(
        r"clang.*", compiler.absolute().resolve().name
    ):
        process: subprocess.Popen = subprocess.Popen(
            [str(compiler.absolute().resolve()), "-v", "-x", "c++", "-E", "-"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            encoding="utf-8",
            text=True,
        )

        try:
            stderr: str = process.communicate(input="", timeout=10)[1]
            capturing: bool = False
            for line in stderr.split("\n"):
                if "#include <...> search starts here:" in line:
                    capturing = True
                    continue
                elif "End of search list" in line:
                    capturing = False
                    continue

                if capturing:
                    path = line.strip()
                    if path and Path(path).exists():
                        includes.append(str(Path(path).absolute().resolve()))
        finally:
            if process.poll() is None:
                process.kill()
    else:
        raise Exception(
            "The compiler is unsupported(is not gcc/clang). If you have a way to solve it, create PR(s) to solve it."
        )
    return includes


def filter_invalid_build_path(build_path: Path) -> Path:
    """
    过滤参数中非法的构建路径

    :param build_path: 构建路径
    :type build_path: Path
    :return: 保证合法的构建路径
    :rtype: Path
    """

    if not build_path.exists():
        raise Exception(
            f"路径 {build_path} 不存在，无法获取 `compile_commands.json` 文件"
        )

    if not build_path.is_file() or build_path.name != "compile_commands.json":
        raise Exception("路径不是 `compile_commands.json` 文件，无法获取")
    return build_path


def process_paths(paths: list[str]) -> list[Path]:
    """
    处理并筛选参数中的路径

    :param paths: 原始路径
    :type paths: list[str]
    :return: 处理后的路径
    :rtype: list[Path]
    """

    files: list[Path] = []
    for raw_file in paths:
        path: Path = Path(raw_file)

        if not path.exists():
            print(f"warning: 路径 {path} 不存在，跳过")
            continue

        if path.is_file():
            if (
                path.suffix == ".cpp"
                or path.suffix == ".hpp"
                or path.suffix == ".c"
                or path.suffix == ".h"
                or path.suffix == ".cc"
                or path.suffix == ".hh"
                or path.suffix == ".cxx"
                or path.suffix == ".hxx"
            ):
                files.append(path)
            else:
                print(f"warning: 路径 {path} 不是C/C++文件，跳过")
        elif path.is_dir():
            extensions: list[str] = ["cpp", "hpp", "c", "h", "cc", "hh", "cxx", "hxx"]
            for extension in extensions:
                files += path.rglob(f"*.{extension}")
        else:
            print(f"warning: 路径 {path} 既不是目录，也不是文件，跳过")

    if not files:
        raise Exception("没有输入文件")
    return files


def add_include_directories_to_new_build_path(
    old_build_path: Path, new_build_path: Path, extra_args: list[str]
):
    """
    复制旧的 `compile_commands.json` 文件到新的中，并添加指定的包含文件

    :param old_build_path: 旧的构建目录
    :type old_build_path: Path
    :param new_build_path: 新的构建目录
    :type new_build_path: Path
    :param extra_args: 额外的参数
    :type extra_args: list[str]
    """

    with open(old_build_path, "r", encoding="utf-8") as input_f:
        data: list[dict[Any, Any]] = json.load(input_f)
        for json_dict in data:
            json_command: str = json_dict["command"]
            command: list[str] = json_command.split(" ")
            new_command = [
                "clang++",
                *extra_args,
                *command[1:],
            ]
            json_dict["command"] = " ".join(new_command)
        with open(new_build_path, "w", encoding="utf-8") as output_f:
            json.dump(data, output_f)


def main():
    """
    主函数
    """

    try:
        import argparse

        # 解析参数
        parser = argparse.ArgumentParser(
            description="输出C/C++代码问题（使用clang-tidy）到文件的脚本"
        )
        parser.add_argument(
            "-p", "--build_path", type=str, help="`compile_commands.json` 的路径位置"
        )
        parser.add_argument(
            "-ea",
            "--extra-arg",
            type=str,
            action="append",
            help="额外的参数",
        )
        parser.add_argument(
            "-fd",
            "--file-or-directory",
            type=str,
            action="append",
            help="要解析的代码文件（夹）位置",
        )
        parser.add_argument(
            "--print-stderr",
            action="store_true",
            default=False,
            help="用于添加clang-tidy的stderr的输出",
        )
        parser.add_argument(
            "-v",
            "--verbose",
            action="store_true",
            default=False,
            help="用于添加详细输出",
        )
        args = parser.parse_args()
        old_build_path: Path = filter_invalid_build_path(Path(args.build_path))
        files: list[Path] = process_paths(args.file_or_directory)
        output_file: Path = Path("code_issues/code_issues.txt")
        new_build_path: Path = Path(os.curdir) / "compile_commands.json"

        # 在json文件中添加编译器目录
        compiler = shutil.which("clang")
        if not compiler:
            print("error: 没有找到clang编译器")
            return
        add_include_directories_to_new_build_path(
            old_build_path,
            new_build_path,
            ["-stdlib=libc++"] + args.extra_arg,
        )

        print(f"检查 {len(files)} 个文件，输出到 {output_file.absolute().resolve()}")

        # 清空输出文件
        with open(output_file, "w", encoding="utf-8"):
            pass

        # 寻找错误并记录
        ansi_color_pattern: re.Pattern = re.compile(
            r"\033\[[0-9;]*m"
        )  # 用于删除ansi转义的颜色
        issue_count: int = 0
        step: int = 5
        for i in range(0, len(files), step):
            try:
                subfiles: list[Path] = files[i : i + step]
                command: list[str] = [
                    "clang-tidy",
                    "-p",
                    str(new_build_path.absolute().resolve()),
                    '-header-filter=".*"',
                ]
                if args.verbose:
                    command.append("-extra-arg=-v")
                for file in subfiles:
                    command.append(str(file.absolute().resolve()))
                print(f"正在检查第{i}至第{min(len(files), i + step)}文件的问题...")
                result = subprocess.run(
                    command,
                    capture_output=True,
                    text=True,
                    encoding="utf-8",
                    timeout=1000,
                )
                issue_count += result.stdout.count("warning: ") + result.stdout.count(
                    "error: "
                )
                with open(output_file, "a", encoding="utf-8") as file:
                    file.write(f"检查文件: {[str(file) for file in subfiles]}\n")
                    file.write("-" * 80 + "\n\n")
                    file.write(ansi_color_pattern.sub("", result.stdout) + "\n\n")
                    if args.print_stderr:
                        if result.stderr:
                            file.write("stderr的输出：\n")
                            file.write(
                                ansi_color_pattern.sub("", result.stderr) + "\n\n"
                            )
                        else:
                            file.write("stderr无输出")
            except Exception as e:
                print(f"error: {e}")
                return

        # 删除临时创建的文件
        os.remove(new_build_path)

        # 输出
        print(f"检查完成，结果已保存到 {output_file}")
        print(f"发现 {issue_count} 个问题")
    except Exception as e:
        print(f"error: {e}")


if __name__ == "__main__":
    main()
