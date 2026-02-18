#!/usr/bin/env python3
"""
一些有用的帮助组件

:version: 0.1.0-1
"""

from pathlib import Path
from enum import IntFlag
from typing import Final


class PathType(IntFlag):
    NOT_EXIST = -1  # 不存在
    DIRECTORY = 1 << 0  # 文件夹
    FILE = 1 << 1  # 文件
    SYMLINK = 1 << 2  # 链接
    ALL = DIRECTORY | FILE | SYMLINK | NOT_EXIST  # 都可能


def filter_invalid_path(
    path: Path, path_type_should_be: PathType = PathType.ALL
) -> Path:
    """
    过滤非法的路径

    :param path: 路径
    :type path: Path
    :return: 保证合法的路径
    :rtype: Path
    """

    if (
        ((PathType.NOT_EXIST in path_type_should_be) and (not path.exists()))
        or ((PathType.DIRECTORY in path_type_should_be) and path.is_dir())
        or ((PathType.FILE in path_type_should_be) and path.is_file())
        or ((PathType.SYMLINK in path_type_should_be) and path.is_symlink())
    ):
        return path.resolve()
    raise Exception(f"路径 {path} 不符合 {path_type_should_be} 的要求")


C_CPP_FILE_EXTENSION: Final[list[str]] = [
    ".cpp",
    ".hpp",
    ".c",
    ".h",
    ".cc",
    ".hh",
    ".cxx",
    ".hxx",
]
