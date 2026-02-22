\page file_versions scene_components.hpp 版本历史
\tableofcontent

# 目录

[TOC]

# scene_components.hpp 版本历史

---

## [0.1.0-2] - 2026-02-22 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Changed(修改)

1. **BREAKING(不向下兼容):** 将 `tnrw::ecs::global_scenes`, `tnrw::ecs::scene` 合并为 `tnrw::ecs::game_scenes`
2. **BREAKING(不向下兼容):** 将 `tnrw::ecs::game_scenes` 和 `tnrw::ecs::father_scenes` 的所有成员公开
3. **BREAKING(不向下兼容):** 将 `tnrw::ecs::game_scenes` 和 `tnrw::ecs::father_scenes` 去模板化

---

## [0.1.0-1] - 2025-08-14 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了几个基本的与场景有关的组件 `tnrw::ecs::global_scenes`, `tnrw::ecs::scene`, `tnrw::ecs::father_scenes`

---
