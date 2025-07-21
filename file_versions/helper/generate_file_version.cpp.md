\page file_versions generate_file_version.cpp 版本历史
\tableofcontent

# 目录

[TOC]

# generate_file_version.cpp 版本历史

***

## [0.1.0-2] - 2025-07-06 - [cpp-love(15865418+cpp-love@user.noreply.gitee.com)](15865418+cpp-love@user.noreply.gitee.com)

### Added(新增)

1. 新增使用方法：可以直接运行 `.\bin\generate_file_version` 后根据提示操作了

### Changed(修改)

1. **BREAKING(不向下兼容):** 将模板文件改名为`template/file_version.template.md`

***

## [0.1.0-1] - 2025-07-05 - [cpp-love(15865418+cpp-love@user.noreply.gitee.com)](15865418+cpp-love@user.noreply.gitee.com)

### Added(新增)

1. 创建使用方法：`.\bin\generate_file_version <file_directory> <file_name> [additional_content]`
   - `<file_directory>` 是文件所在的目录
   - `<file_name>` 是文件名
   - `[additional_content]` 是可选的附加说明内容
   - 生成的文件将保存在 `file_versions/<file_directory>/<file_name>`
   - 生成的文件内容将基于 `template/file_version_template.md` 模板文件

***
