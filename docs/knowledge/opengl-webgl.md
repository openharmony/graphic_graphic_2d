# OpenGL / WebGL Wrapper

## 适用范围

- OpenGL ES / EGL 函数动态加载与 Hook
- WebGL 应用通过 OpenGL Wrapper 使用 GPU 能力
- GL API 版本兼容（GLES1/GLES2/GLES3/GL4）
- 线程私有数据管理（多 EGL 上下文）
- GL 扩展函数查询与加载
- 符号版本控制与导出管理

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| hook.h | `frameworks/opengl_wrapper/include/hook.h` | Hook 表结构：WrapperHookTable / EglHookTable / GlHookTable1-4 |
| thread_private_data_ctl.h | `frameworks/opengl_wrapper/include/thread_private_data_ctl.h` | 线程私有数据控制 |
| wrapper_log.h | `frameworks/opengl_wrapper/include/wrapper_log.h` | Wrapper 日志 |
| egl_hook_entries.in | `frameworks/opengl_wrapper/include/egl_hook_entries.in` | EGL Hook 函数入口列表 |
| gl1_hook_entries.in | `frameworks/opengl_wrapper/include/gl1_hook_entries.in` | GLES1 Hook 函数入口列表 |
| gl2_hook_entries.in | `frameworks/opengl_wrapper/include/gl2_hook_entries.in` | GLES2 Hook 函数入口列表 |
| gl3_hook_entries.in | `frameworks/opengl_wrapper/include/gl3_hook_entries.in` | GLES3 Hook 函数入口列表 |
| gl4_hook_entries.in | `frameworks/opengl_wrapper/include/gl4_hook_entries.in` | GL4 Hook 函数入口列表 |
| gl1_entries.in | `frameworks/opengl_wrapper/include/gl1_entries.in` | GLES1 函数定义列表 |
| gl2_entries.in | `frameworks/opengl_wrapper/include/gl2_entries.in` | GLES2 函数定义列表 |
| gl2ext_entries.in | `frameworks/opengl_wrapper/include/gl2ext_entries.in` | GLES2 扩展函数列表 |
| gl32_entries.in | `frameworks/opengl_wrapper/include/gl32_entries.in` | GLES3.2 函数定义列表 |
| wrapper_hook_entries.in | `frameworks/opengl_wrapper/include/wrapper_hook_entries.in` | Wrapper 层 Hook 函数列表 |
| EGL/ | `frameworks/opengl_wrapper/src/EGL/` | EGL 函数 Wrapper 实现 |
| GLES1/ | `frameworks/opengl_wrapper/src/GLES1/` | GLES1 函数 Wrapper 实现 |
| GLES2/ | `frameworks/opengl_wrapper/src/GLES2/` | GLES2 函数 Wrapper 实现 |
| GLES3/ | `frameworks/opengl_wrapper/src/GLES3/` | GLES3 函数 Wrapper 实现 |
| GL4/ | `frameworks/opengl_wrapper/src/GL4/` | GL4 函数 Wrapper 实现（条件编译） |
| EGL.versionscript | `frameworks/opengl_wrapper/EGL.versionscript` | EGL 符号版本脚本 |

## 核心模型

OpenGL Wrapper 采用 **函数指针 Hook 表** 架构：

```
应用层调用 egl* / gl* 函数
  → Wrapper 层导出同名符号
  → 通过 Hook 表函数指针分发
  → 实际调用 GPU 驱动实现

Hook 表结构:
  WrapperHookTable  → Wrapper 层额外函数
  EglHookTable      → EGL 函数指针（约 100 个）
  GlHookTable1      → GLES1 函数指针
  GlHookTable2      → GLES2 函数指针
  GlHookTable3      → GLES3 函数指针
  GlHookTable4      → GL4 函数指针（条件编译 OPENGL_WRAPPER_ENABLE_GL4）
  GlHookTable       → 聚合 table1 + table2 + table3 [+ table4]
```

`.in` 文件模式：使用 `#include` 包含的 `.in` 文件定义函数列表，通过宏展开生成 Hook 表结构体和分发函数。

**线程私有数据**：`ThreadPrivateDataCtl` 管理每个线程的 EGL 上下文关联，确保多线程场景下 GL 调用正确路由到对应的 EGL Context。

**版本兼容**：`ENTRIES_FILES_VERSION` 常量标记 Hook 表版本，修改 `.in` 文件时需递增，确保 ABI 兼容。

数据流：应用 GL 调用 → Wrapper 导出符号 → Hook 表查找函数指针 → GPU 驱动执行。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 函数指针 Hook 表 | `EglHookTable` / `GlHookTable1-4` 结构体 | 运行时动态加载 GPU 驱动，无需编译期绑定 |
| .in 文件 + 宏展开 | `*_entries.in` + `HOOK_API_ENTRY` 宏 | 集中管理函数列表，避免手工维护多份列表 |
| 线程私有数据控制 | `ThreadPrivateDataCtl` | 多线程多 EGL 上下文场景下的正确路由 |
| GL4 条件编译 | `#ifdef OPENGL_WRAPPER_ENABLE_GL4` | 桌面 OpenGL 4.x 仅在特定产品启用 |
| 版本号机制 | `ENTRIES_FILES_VERSION` | 修改 Hook 表时强制更新，防止 ABI 不匹配 |
| 符号版本脚本 | `EGL.versionscript` | 控制动态库导出符号，防止符号冲突 |

## 待补充背景

- Hook 表初始化与 GPU 驱动加载的具体流程
- WebGL 调用如何映射到 Wrapper 层
- ThreadPrivateDataCtl 的线程安全保证机制
- GL4 支持的产品形态与启用条件
- Wrapper 层与 GPU 驱动的错误处理与 fallback 策略
