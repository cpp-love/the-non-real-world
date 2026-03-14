\page file_versions shape_components.hpp 版本历史
\tableofcontent

# 目录

[TOC]

# shape_components.hpp 版本历史

---

## [0.1.0-3] - 2026-03-14 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 将 `ecs/components/render_shape_components.hpp` 合并到此文件
2. 添加 `tnrw::ecs::collidable_shape` 用于修改后碰撞

### Changed(修改)

1. **BREAKING(不向下兼容):** 取消 `tnrw::ecs::shape` 与 `tnrw::ecs::render_shape` 的相互转换，
   请使用其子类型的相互转换代替

---

## [0.1.0-2] - 2026-02-24 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Changed(修改)

1. **BREAKING(不向下兼容):** 取消 `tnrw::ecs::shape` 的渲染支持，可以使用更全面的 `tnrw::ecs::render_shape` / `tnrw::ecs::collidable_shape` 代替

---

## [0.1.0-1] - 2025-10-18 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加基本的形状组件( `tnrw::ecs::shape` ，包括线、圆形、矩形等组件)、可渲染、可碰撞组件

---
