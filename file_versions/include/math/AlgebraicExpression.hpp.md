\page file_versions AlgebraicExpression.hpp 版本历史
\tableofcontent

# 目录

[TOC]

# AlgebraicExpression.hpp 版本历史

---

## [0.1.0-3] - 2026-02-14 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了 `AlgebraicExpression` 对自己的加减乘除运算
2. 添加了 `hasVariable` 方法，用于判断代数式是否有指定变量或任意变量
3. 添加了 `toNumericExpression` 方法，用于将代数式转换成无字母的代数式
4. 添加了从 `tnrw::math::NumericExpression` 构造的转换构造函数，用于将无字母的代数式转换成代数式

---

## [0.1.0-2] - 2026-01-17 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. `calculateApproximation` 方法，用于计算近似值
2. `std::formatter` 对 `tnrw::math::AlgebraicExpression` 的特化，
   用于支持 `std::format` 和 `std::print(ln)` 函数的格式化

### Changed(修改)

1. **BREAKING(不向下兼容):** 将 `tnrw::math::VariableType` 别名的类型从 `char` 转为 `std::string`，
   以支持字符串变量，并将 `_v` 与 `_vAlgeExpr` 自定义字面量的声明该为使用字符串字面量为参数
2. **BREAKING(不向下兼容):** 取消方法 `toString` 和 `toWString` 的参数 `loc`，
   可使用 `std::format` 或 `std::ostringstream` 的格式化来达到类似的效果

### Removed(移除)

1. 无用的方法 `isZero` 与 `getValue` 被移除，
   `getValue` 可使用 `calculateApproximation` 代替
2. 多余的模板方法 `toBasicString` 被移除，
   若字符类型是 `char` 或 `wchar_t` 可用 `toString` 和 `toWString` 代替，
   若不是，可用 `std::format` 或 `std::ostringstream` 来达到类似的效果

---

## [0.1.0-1] - 2025-07-05 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了代数式类型 `tnrw::math::AlgebraicExpression`
   和基本的代数式与常量和变量类型的操作
   和代数式类型的自定义字面量

---
