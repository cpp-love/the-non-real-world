\page file_versions overload.hpp 版本历史
\tableofcontent

# 目录

[TOC]

# overload.hpp 版本历史

---

## [0.1.0-1] - 2026-02-16 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 对应了生成对于可调用类型的重载的函数对象的工厂函数 `make_overloaded`
   - 支持 **绝大部分** 可调用类型：
     1. **非模板的** 普通函数（函数引用）
     2. **非模板的** 函数指针
     3. **非模板的** 成员函数指针（但是包装后成为模板函数对象，调用优先级比非模板的可调用对象低）
     4. 函数对象（包括 `std::function` 等标准库定义的函数对象）
     5. lambda 表达式
   - 不支持类型：
     1. 普通模板函数，若要使其参与重载，请使用 泛型 lambda 表达式包装 或 手动定义函数对象包装
     2. C-style 可变参数函数，若要使其参与重载，请使用 泛型 lambda 表达式包装 或 手动定义函数对象包装

---
