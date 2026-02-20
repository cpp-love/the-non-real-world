\page file_versions game_state_components.hpp 版本历史
\tableofcontent

# 目录

[TOC]

# game_state_components.hpp 版本历史

---

## [0.1.0-2] - 2026-02-20 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Changed(修改)

1. **BREAKING(不向下兼容):** 重构了整个文件
   1. 声明了游戏状态基类 `game_state_base`，并定义了四个继承类
   2. 定义了游戏状态枚举 `game_state_enum`，分别对应四个继承类
   3. 定义了函数 `to_string_view`，用于将游戏状态枚举转为字符串
   4. 定义了工厂函数 `make_corresponding_state`，用于使用游戏状态枚举构造游戏状态类

---

## [0.1.0-1] - 2025-07-26 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了游戏状态组件 `tnrw::ecs::GameState` ，采用 `std::vector` 存储

---
