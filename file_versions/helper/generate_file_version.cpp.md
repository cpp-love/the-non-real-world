\page file_versions generate_file_version.cpp 版本历史
\tableofcontent

# 目录

[TOC]

# generate_file_version.cpp 版本历史

***

## [0.1.0-5] -2025-11-8 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Changed(修改)

1. 修改了文件内部的一些代码，使其更加现代与安全

***

## [0.1.0-4] -2025-09-20 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了命令行参数 `<file>` 对文件夹的支持，若是文件夹，则递归生成里面的每一个文件

### Changed(修改)

1. 将文件的默认日期从当前日期改为尝试使用文件 `Doxygen` 注释的日期，如果没有找到，则为默认日期

***

## [0.1.0-3] - 2025-07-27 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Changed(修改)

1. **BREAKING(不向下兼容):** 将模板文件改名为 `helper/file_version.template.md`
2. 重构了整个文件
3. **BREAKING(不向下兼容):** 将原有的命令行参数使用方法 `.\bin\generate_file_version <file_directory> <file_name> [additional_content]`
   修改为 `.\bin\exes\generate_file_version <workspace_folder> <file> <authors>`，其中
   - `<workspace_folder>` 是工作区目录
   - `<file>` 是文件路径
   - `<authors>` 是作者名称
   - `<file_relative>` 是文件相对工作区的路径（运行时根据 `<workspace_folder>` 和 `<file>` 推导出来的）
   - 生成的文件将保存在 `file_versions/<file_relative>`
   - 生成的文件内容将基于 `<workspace_folder>/helper/file_version.template.md` 模板文件

***

## [0.1.0-2] - 2025-07-06 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 新增使用方法：可以直接运行 `.\bin\generate_file_version` 后根据提示操作了

### Changed(修改)

1. **BREAKING(不向下兼容):** 将模板文件改名为`template/file_version.template.md`

***

## [0.1.0-1] - 2025-07-05 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 创建使用方法：`.\bin\generate_file_version <file_directory> <file_name> [additional_content]`
   - `<file_directory>` 是文件所在的目录
   - `<file_name>` 是文件名
   - `[additional_content]` 是可选的附加说明内容
   - 生成的文件将保存在 `file_versions/<file_directory>/<file_name>`
   - 生成的文件内容将基于 `template/file_version_template.md` 模板文件

***
