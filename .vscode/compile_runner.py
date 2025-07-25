#!/usr/bin/env python3
"""
VSCode 集成编译运行控制器
支持文件组选择、通配符扩展和跨平台编译
"""
import os
import sys
import glob
import platform
import subprocess
import argparse
import json
import re


def load_json_with_comments(content):
    """
    加载带注释的JSON文件
    支持 // 和 /* */ 注释
    """

    # 移除单行注释
    content = re.sub(r"//.*?$", "", content, flags=re.MULTILINE)

    # 移除多行注释
    content = re.sub(r"/\*.*?\*/", "", content, flags=re.DOTALL)

    # 解析JSON
    return json.loads(content)


# 从 VSCode 设置中获取配置
def get_vscode_config():
    """从 VSCode 设置中获取配置"""
    try:
        # 尝试从 VSCode 环境变量获取设置
        config_path = os.path.join(
            os.getenv("WORKSPACE_FOLDER", ""), ".vscode", "settings.json"
        )
        if os.path.exists(config_path):
            with open(config_path, "r", encoding="utf-8") as f:
                # 读取文件内容
                content = f.read()
                settings = load_json_with_comments(content)
                return {
                    "file_groups": settings.get("compile-runner.fileGroups", {}),
                    "output_path": settings.get(
                        "compile-runner.outputPath", "${workspaceFolder}/build/output"
                    ),
                    "front_compiler_flags": settings.get(
                        "compile-runner.FrontCompilerArgs",
                        ["-O2", "-g", "-Wall", "-Wextra"],
                    ),
                    "back_compiler_flags": settings.get(
                        "compile-runner.BackCompilerArgs",
                        ["-O2", "-g", "-Wall", "-Wextra"],
                    ),
                }
    except Exception as e:
        print(f"抛出错误: {e}")
        pass

    # 默认配置
    return {
        "file_groups": {
            "核心模块": ["src/main.cpp", "src/core/**/*.cpp"],
            "测试模块": ["tests/**/*.cpp", "src/utils.cpp"],
            "完整编译": ["src/**/*.cpp", "include/**/*.hpp"],
        },
        "output_path": os.path.join(
            os.getenv("WORKSPACE_FOLDER", ""), "build", "output"
        ),
        "front_compiler_flags": ["-O2", "-g", "-Wall", "-Wextra"],
        "back_compiler_flags": [],
    }


def expand_vscode_vars(path):
    """替换 VSCode 变量"""
    workspace = os.getenv("workspaceFolder", "")
    file = os.getenv("file", "")

    replacements = {
        "${workspaceFolder}": workspace,
        "${file}": file,
        "${fileDirname}": os.path.dirname(file),
        "${fileBasename}": os.path.basename(file),
        "${fileBasenameNoExtension}": os.path.splitext(os.path.basename(file))[0],
    }

    for var, value in replacements.items():
        path = path.replace(var, value)

    return os.path.normpath(path)


def expand_patterns(patterns, base_path=None):
    """展开通配符模式为实际文件列表"""
    expanded = []
    for pattern in patterns:
        # 处理 VSCode 变量
        pattern = expand_vscode_vars(pattern)

        # 确保绝对路径
        if not os.path.isabs(pattern):
            pattern = os.path.join(base_path or os.getcwd(), pattern)

        # 展开通配符
        files = glob.glob(pattern, recursive=True)

        # 过滤出文件（排除目录）
        files = [f for f in files if os.path.isfile(f)]

        expanded.extend(files)

    # 去重并保持顺序
    return list(dict.fromkeys(expanded))


def select_files_interactively():
    """交互式选择文件"""
    print("请选择文件（输入多个文件路径，用空格分隔）:")
    print("支持通配符（如：src/**/*.cpp），完成后按回车两次开始")

    selected = []
    while True:
        try:
            line = input()
            if not line:
                break
            selected.extend(line.split())
        except EOFError:
            break

    return selected


def expand_for_compiler_command(flags):
    """展开编译命令的通配符"""
    flags_str = ""
    for flag in flags:
        if flag[0] == "$":
            flags_str += expand_vscode_vars(flag)
        else:
            flags_str += flag
        flags_str += " "
    return flags_str


def get_compiler_commands(files, output_path, front_flags, back_flags):
    """根据平台获取编译命令"""
    front_flags_str = expand_for_compiler_command(front_flags)
    back_flags_str = expand_for_compiler_command(back_flags)

    # 处理 Windows 路径
    if platform.system() == "Windows":
        output_path = output_path.replace("/", "\\")

    commands = list()
    file_objs = ""

    for file in files:
        file_obj = (
            expand_vscode_vars("${workspaceFolder}\\bin\\objs")
            + "\\"
            + os.path.splitext(os.path.basename(file))[0]
            + ".o"
        )
        # 创建输出目录
        os.makedirs(os.path.dirname(file_obj), exist_ok=True)
        file_objs += file_obj
        file_objs += " "
        commands.append(f"g++ {front_flags_str}{file} -o {file_obj} {back_flags_str}-c")
    commands.append(
        f"g++ {front_flags_str}{file_objs}-o {output_path} {back_flags_str}"
    )

    return commands


def main():
    config = get_vscode_config()

    # 处理输出路径中的变量
    output_path = expand_vscode_vars(config["output_path"])

    parser = argparse.ArgumentParser(description="VSCode 编译运行控制器")
    parser.add_argument("--group", help="指定文件组名称")
    parser.add_argument("--interactive", action="store_true", help="交互式选择文件")
    args = parser.parse_args()

    # 确定要编译的文件
    if args.group:
        if args.group not in config["file_groups"]:
            print(f"错误: 未知文件组 {args.group}")
            print("可用文件组:")
            for group in config["file_groups"]:
                print(f"  - {group}")
            sys.exit(1)

        patterns = config["file_groups"][args.group]
        files = expand_patterns(patterns, os.getenv("WORKSPACE_FOLDER"))
    elif args.interactive:
        patterns = select_files_interactively()
        files = expand_patterns(patterns)
    else:
        print("错误: 需要指定 --group 或 --interactive")
        sys.exit(1)

    if not files:
        print("错误: 未选择任何文件")
        sys.exit(1)

    # 创建输出目录
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    # 获取未改变的文件
    unchanged_index = list()
    print(":: 以下输出的文件哪些未改变？未改变输入1，改变输入其他")
    for i in range(len(files)):
        print(f":: {i + 1}. {files[i]}")
        if input() == "1":
            unchanged_index.append(i)

    # 获取编译命令
    compile_cmds = get_compiler_commands(
        files,
        output_path,
        config["front_compiler_flags"],
        config["back_compiler_flags"],
    )

    for i in range(len(compile_cmds)):

        # 在 VSCode 问题面板中显示编译信息
        if i in unchanged_index:
            print(f"{files[i]}未改变，跳过此文件")
            continue
        elif i == len(compile_cmds) - 1:
            print(f":: 链接全部文件")
            print(f":: 输出: {output_path}")
        else:
            print(f":: 编译第 {i + 1} 个文件")

        print(f":: 命令: {compile_cmds[i]}")

        # 执行编译
        process = subprocess.Popen(
            compile_cmds[i],
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            universal_newlines=True,
            encoding="utf-8",
        )

        # 实时输出编译结果
        while True:
            output = process.stdout.readline()
            if output == "" and process.poll() is not None:
                break
            if output:
                # 格式化输出以便问题匹配器工作
                print(output.strip())

        return_code = process.poll()

        if return_code != 0:
            print(f"\n编译失败! 退出代码: {return_code}")
            sys.exit(return_code)

    print("\n编译成功!")


if __name__ == "__main__":
    main()
