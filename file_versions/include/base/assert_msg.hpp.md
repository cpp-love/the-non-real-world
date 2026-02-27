\page file_versions assert_msg.hpp 版本历史
\tableofcontent

# 目录

[TOC]

# assert_msg.hpp 版本历史

---

## [0.1.0-6] - 2026-02-27 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加 `tnrw::unreachable` 的 `constexpr` 支持

### Changed(修改)

1. **BREAKING(不向下兼容):** 将 `ASSERT_MSG` 改名为 `TNRW_ASSERT_MSG`

---

## [0.1.0-5] - 2026-02-24 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加 `tnrw::unreachable`，提供了类似带消息的 `std::unreachable` 的作用

### Changed(修改)

1. **BREAKING(不向下兼容):** 将 `assert_msg` 改名为 `ASSERT_MSG`（现为 `TNRW_ASSERT_MSG`）

---

## [0.1.0-4] - 2026-02-23 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了输出 **运行时字符串** 的能力

### Changed(修改)

1. 移除了无法编译通过的 `assert_check` 函数的 `wformat_string` 版本，断言函数现在只接受以 **`char`** 类型为单元的字符串

---

## [0.1.0-3] - 2026-02-21 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了 **打印堆栈跟踪** 的能力

---

## [0.1.0-2] - 2025-11-09 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Changed(修改)

1. **BREAKING(不向下兼容):** 取消了对不同类型的格式化字符串的支持

---

## [0.1.0-1] - 2025-08-08 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了带注释的断言(`assert_msg`，现为 `TNRW_ASSERT_MSG`)，与 `<cassert>` 中的 `assert` 功能相近，发布时（定义了 `NDEBUG` 宏时）可以完全消除

---
