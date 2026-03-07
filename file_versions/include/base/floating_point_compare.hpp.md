\page file_versions floating_point_compare.hpp 版本历史
\tableofcontent

# 目录

[TOC]

# floating_point_compare.hpp 版本历史

---

## [0.1.0-2] - 2026-03-07 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Changed(修改)

1. **BREAKING(不向下兼容):** 将 `tnrw::fast_float` 改名为 `tnrw::no_nan_inf`，`tnrw::safe_float` 改名为 `tnrw::has_nan_inf`
2. **BREAKING(不向下兼容):** 将 `tnrw::no_nan_inf` 的构造函数从不检查是否是 `NaN` 或 `Inf` 改为可选的检查（使用 `TNRW_ASSERT_MSG`）

---

## [0.1.0-1] - 2025-11-15 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了 `tnrw::fast_float` 和 `tnrw::safe_float` 两个类模板，用于浮点数的近似比较

---
