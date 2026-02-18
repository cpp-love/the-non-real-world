#!/usr/bin/env python3
"""
移动C/C++文件的脚本

使用方法：
见 `python3 move_source_file.py --help` 的结果

:version: 0.1.0-1
"""

import re
import subprocess
import helper_base
from pathlib import Path
from itertools import chain
from typing import Iterator


def move_file(source: Path, destination: Path):
    """
    使用git移动文件

    :param source: 要移动的文件
    :type source: Path
    :param destination: 文件移动后的位置
    :type destination: Path
    """
    result = subprocess.run(
        ["git", "mv", str(source), str(destination)],
        capture_output=True,
        text=True,
        encoding="utf-8",
        timeout=1000,
    )

    print(f"移动文件{source}到{destination}")

    message: str = ""
    if result.stdout:
        message += result.stdout
        message += "\n"
    if result.stderr:
        message += result.stderr
        message += "\n"

    if message:
        raise Exception(message)


def get_folder_files(folder: Path) -> Iterator[Path]:
    """
    获取文件夹中需要修改的文件

    :param folder: 文件夹
    :type folder: Path
    :return: 文件夹中需要修改的文件
    :rtype: Iterator[Path]
    """

    extensions: list[str] = [*helper_base.C_CPP_FILE_EXTENSION, ".md"]
    files: Iterator[Path] = chain(
        chain.from_iterable(folder.rglob(f"*{extension}") for extension in extensions),
        folder.rglob("CMakeLists.txt"),
    )

    return files


def main():
    """
    主函数
    """
    try:
        import argparse

        # 解析参数
        parser = argparse.ArgumentParser(description="移动C/C++文件的脚本")
        parser.add_argument(
            "source",
            type=str,
            help="要移动的文件",
        )
        parser.add_argument(
            "destination",
            type=str,
            help="文件移动后的位置",
        )
        parser.add_argument(
            "-w",
            "--workspacefolder",
            type=str,
            help="文件所属的工作区",
        )
        args = parser.parse_args()
        source: Path = helper_base.filter_invalid_path(
            Path(args.source), helper_base.PathType.FILE
        )
        if not source.suffix in helper_base.C_CPP_FILE_EXTENSION:
            raise Exception("要移动的文件不是C/C++文件")
        destination: Path = helper_base.filter_invalid_path(
            Path(args.destination), helper_base.PathType.NOT_EXIST
        )
        if not destination.suffix in helper_base.C_CPP_FILE_EXTENSION:
            raise Exception("要移动的文件不是C/C++文件")
        workspace_folder: Path = helper_base.filter_invalid_path(
            Path(args.workspacefolder), helper_base.PathType.DIRECTORY
        )
        source_md: Path = Path(
            str(
                workspace_folder
                / "file_versions"
                / source.relative_to(workspace_folder)
            )
            + ".md"
        )
        destination_md: Path = Path(
            str(
                workspace_folder
                / "file_versions"
                / destination.relative_to(workspace_folder)
            )
            + ".md"
        )

        # 改名
        move_file(source, destination)
        move_file(source_md, destination_md)

        # 更改相对应的名称
        all_files: Iterator[Path] = chain.from_iterable(
            get_folder_files(workspace_folder / subfolder)
            for subfolder in ["file_versions", "helper", "include", "src", "tests"]
        )
        for file in all_files:
            file_content: str = ""
            with open(file, "r", encoding="utf-8") as f:
                line_number: int = 0
                while True:
                    line_number += 1
                    line: str = f.readline()
                    if not line:
                        break
                    line_unchanged: str = line
                    if source.is_relative_to(workspace_folder / "include"):
                        line = re.sub(
                            r"\b"
                            + str(source.relative_to(workspace_folder / "include"))
                            + r"\b",
                            str(destination.relative_to(workspace_folder / "include")),
                            line,
                        )
                    line = re.sub(r"\b" + source.name + r"\b", destination.name, line)
                    if line_unchanged != line:
                        print(f"{file}文件的第{line_number}行发生变化：")
                        print(f"{line_unchanged.rstrip('\n')}")
                        print(f"->")
                        print(f"{line.rstrip('\n')}")
                    file_content += line
            with open(file, "w", encoding="utf-8") as f:
                f.write(file_content)

    except Exception as e:
        print(f"error: {e}")


if __name__ == "__main__":
    main()
