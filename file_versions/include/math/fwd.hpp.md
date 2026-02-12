\page file_versions fwd.hpp 版本历史
\tableofcontent

# 版本历史记录

## 目录

[TOC]

# fwd.hpp 版本历史

---

## [0.1.0-2] - 2026-01-02 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. `tnrw::math::VariableView` 别名的定义及其自定义字面量

### Changed(修改)

1. **BREAKING(不向下兼容):** 将 `tnrw::math::VariableType` 别名的类型从 `char` 转为 `std::string`，
   以支持字符串变量，并将 `_v` 与 `_vAlgeExpr` 自定义字面量的声明该为使用字符串字面量为参数
2. **BREAKING(不向下兼容):** 将 `tnrw::math::ConstantType` 改名为 `tnrw::math::IntegerConstantType`，
   以更清晰地显示为整型

### Removed(移除)

1. `tnrw::math::gcd` （最大公约数）和 `tnrw::math::lcm` （最小公倍数）的函数的前向声明被移除，
   因为与标准库的 `std::gcd` 与 `std::lcm` 重复

---

## [0.1.0-1] - 2025-07-05 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. `AlgegraicExpression` 类及其自定义字面量的前向声明
2. `NumericExpression` 类及其自定义字面量的前向声明
3. `gcd` （最大公约数）和 `lcm` （最小公倍数）的函数的前向声明
4. `ConstantType` 和 `VariableType` 别名的前向声明

---
