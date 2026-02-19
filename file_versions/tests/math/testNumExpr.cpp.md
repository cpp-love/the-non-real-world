\page file_versions testNumExpr.cpp 版本历史
\tableofcontent

# 目录

[TOC]

# testNumExpr.cpp 版本历史

---

## [0.1.0-3] - 2026-02-12 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Removed(移除)

1. 移除 **整个文件**，请使用 `test_expressions.cpp` 的 `num ...` 部分命令代替

---

## [0.1.0-2] - 2026-01-17 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. `calculate_approximation` 用于计算无字母的代数式的近似值

### Deprecated(废弃)

1. 废弃 **整个文件**，因为两个代数式重复内容过多不方便，
   目前此文件剩余的作用仅用于命令帮助的查看，其余的可用 `test_expressions.cpp` 来代替

### Removed(移除)

1. 移除无用命令 `isZero` 和 `getValue`，`getValue` 可用 `calculate_approximation` 代替

---

## [0.1.0-1] - 2025-07-26 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了 `tnrw::math::numeric_expression` 的使用样例和测试用例，
   具体使用方式请使用此程序键入 `help` 来获取帮助

---
