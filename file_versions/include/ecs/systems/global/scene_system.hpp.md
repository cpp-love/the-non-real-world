\page file_versions scene_system.hpp 版本历史
\tableofcontent

# 目录

[TOC]

# scene_system.hpp 版本历史

---

## [0.1.0-2] - 2026-02-22 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Changed(修改)

1. **BREAKING(不向下兼容):** 重构了 `tnrw::ecs::scene_system`，将其去模板化，
   并简化了其成员函数，取消了不安全的操作，因为所有操作现在的开销都很小

---

## [0.1.0-1] - 2025-08-14 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了场景管理系统(`tnrw::ecs::basic_scene_system` 模板类及 `tnrw::ecs::scene_system` 别名)及其基本功能

---
