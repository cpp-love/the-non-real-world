\page file_versions expressions_base.hpp 版本历史
\tableofcontent

# 目录

[TOC]

# expressions_base.hpp 版本历史

***

## [0.1.0-2] - 2026-01-17 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. `tnrw::math::VariableView` 别名的定义及其自定义字面量
2. `tnrw::math::details::Node` 及 `tnrw::math::details::NodePtr` 的声明

### Changed(修改)

1. **BREAKING(不向下兼容):** 将 `tnrw::math::VariableType` 别名的类型从 `char` 转为 `std::string`，
   以支持字符串变量，并将 `_v` 自定义字面量的声明该为使用字符串字面量为参数
2. **BREAKING(不向下兼容):** 将 `tnrw::math::ConstantType` 改名为 `tnrw::math::IntegerConstantType`，
   以更清晰地显示为整型

***

## [0.1.0-1] - 2025-07-23 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了为代数式服务的常量类型和变量类型及其自定义字面量

***
