\page file_versions expressions.cpp 版本历史
\tableofcontent

# 目录

[TOC]

# expressions.cpp 版本历史

***

## [0.1.0-2] - 2026-01-17 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 两个代数式 `calculateApproximation` 方法的实现

### Changed(修改)

1. 重构了整个文件，
   将 `tnrw::math::details::Node` 的不同节点分开为不同的结构体，
   并采用更现代化的方式处理其他函数

***

## [0.1.0-1] - 2025-07-05 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了 `math/AlgebraicExpression.hpp` 和 `math/NumericExpression.hpp` 的实现
2. 采用 **树** 作为其实现方式，两个类的树的节点均为 `tnrw::math::details::Node`

***
