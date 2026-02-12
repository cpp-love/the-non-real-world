\page file_versions expressions.cpp 版本历史
\tableofcontent

# 目录

[TOC]

# expressions.cpp 版本历史

---

## [0.1.0-3] - 2026-02-12 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. `Polynomial` 多项式别名及其一些处理函数，用于辅助 `Division` 节点的简化

### Changed(修改)

1. 将 `IntegerConstant`、`Variable`、`Multiplication` 节点合并为一个节点 `Monomial`，表示单项式（但是没有分数），
   但是没有删除 `Mutiplication` 节点，为了方便代数式的计算与简化
2. 删除了 `Negation` 节点，逻辑分散到其他节点中
3. 修改并完善了 `Division` 节点的化简策略

---

## [0.1.0-2] - 2026-01-17 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 两个代数式 `calculateApproximation` 方法的实现

### Changed(修改)

1. 重构了整个文件，
   将 `tnrw::math::details::Node` 的不同节点分开为6个不同的结构体，
   并采用更现代化的方式处理其他函数

---

## [0.1.0-1] - 2025-07-05 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了 `math/AlgebraicExpression.hpp` 和 `math/NumericExpression.hpp` 的实现
2. 采用 **树** 作为其实现方式，两个类的树的节点均为 `tnrw::math::details::Node`

---
