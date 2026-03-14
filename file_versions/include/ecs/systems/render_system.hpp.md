\page file_versions render_system.hpp 版本历史
\tableofcontent

# 目录

[TOC]

# render_system.hpp 版本历史

---

## [0.1.0-3] - 2026-03-14 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Changed(修改)

1. **BREAKING(不向下兼容):** 从 `tnrw::ecs::render_shape` 迁移到 `tnrw::ecs::collidable_shape`

---

## [0.1.0-2] - 2026-02-27 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 为 `tnrw::ecs::render_system::draw` 添加可选参数 `sf::RenderState`

### Changed(修改)

1. **BREAKING(不向下兼容):** 从 `tnrw::ecs::shape` 迁移到 `tnrw::ecs::render_shape`

---

## [0.1.0-1] - 2025-10-18 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 声明了渲染系统(`tnrw::ecs::render_system`)及其基本功能

---
