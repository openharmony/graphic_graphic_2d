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

## 已知问题与重构方向

以下问题来源于 `glwrapper.md`（opengl_wrapper 历史债务清单），用于指导后续重构。

### 问题一：GL 函数查表性能问题

`ThreadPrivateDataCtl::GetPrivateData()` 基于 `pthread_key_t` + `pthread_getspecific()` 访问 TLS，
相比 AOSP 在 `frameworks/native/opengl/libs/EGL/getProcAddress.cpp` 中用汇编直接读 `tpidr_el0` 槽位
（`mrs x16, tpidr_el0` + `ldr x16, [x16, #tls]` + `br x16`）的方案，每次 GL 扩展调用多出 2-5 条指令。

关键代码位置：

- `frameworks/opengl_wrapper/src/thread_private_data_ctl.cpp:43-51` — `GetPrivateData()` 当前实现
- `frameworks/opengl_wrapper/include/thread_private_data_ctl.h:48-50` — 静态成员
- `frameworks/opengl_wrapper/src/GLES2/gles2.cpp:22-24` — `CALL_HOOK_API` 取 Hook 表
- `frameworks/opengl_wrapper/src/EGL/egl_wrapper_display.cpp:252` — `MakeCurrent` 写入 Hook 表

预研风险：汇编 TLS + 预分配槽位方案在王者荣耀 Unity so 中必现崩溃，因无符号 so 阻塞定位，
疑似 TLS 初始化时序、Hook 表布局或 Unity 缓存函数指针导致。

### 问题二：EGL Android Frame Timestamps 接口不完整

`egl_wrapper_entry.cpp:1543-1546` 已开放两类查询接口，但缺少对应的获取接口：

| 已开放（查询） | 缺失（获取） |
| --- | --- |
| `eglGetCompositorTimingSupportedANDROID` | `eglGetCompositorTimingANDROID` |
| `eglGetFrameTimestampSupportedANDROID` | `eglGetFrameTimestampsANDROID` |

实现参考位置：`egl_wrapper_entry.cpp:1356-1374`（查询实现）。补充前需先确认底层驱动是否支持。

### 问题三：`eglPresentationTimeANDROID` 与 Android 行为不一致

当前 `egl_wrapper_display.cpp:1252-1256` 使用 `NativeWindowHandleOpt(..., SET_UI_TIMESTAMP, time)`，
必须在 `request buffer` 之前调用；Android 行为允许在 `eglSwapBuffers` 之前调用即对当前帧生效。
建议改用 `SET_DESIRED_PRESENT_TIMESTAMP` 对齐 Android，注意跨版本向后兼容。

### 问题四：帧率统计插桩位置重复且不一致

`EglSwapBuffersImpl`（`egl_wrapper_entry.cpp:468-473`）同时被 APS（`EGL_USE_APS_PLUGIN`）和
GameService（`USE_IGAMESERVICE_PLUGIN`）插桩，存在重复、无法覆盖 Vulkan 路径的问题。
建议将插桩统一下沉到 `NativeWindowFlushBuffer`，由 GL/Vulkan 共用底层入口。

### 问题五：缺乏统一的 LayerManager 管理机制

当前 Hook/Debug Layer 由三套机制独立管理：

| 模块 | 加载方式 | 代码位置 |
| --- | --- | --- |
| GTX（`USE_IGRAPHICS_EXTENDS_HOOKS`） | 编译宏 | `egl_wrapper_display.cpp`、`egl_core.cpp` |
| FF（Frame Filters） | `EglSystemLayersManager` + JSON | `egl_system_layers_manager.cpp` |
| 抓帧工具 `libsquid.so` | 硬编码白名单 | `egl_wrapper_layer.cpp` 中 `EglWrapperLayer::DoLoadLayers` |

设计目标：统一的 Layer 注册/加载/生命周期/配置管理，支持 Layer 间依赖与调用链。

### 问题六：跨 `.so` 重复包含 `thread_private_data_ctl.cpp`，类内静态变量产生多副本

`ThreadPrivateDataCtl` 的三个静态成员在 `frameworks/opengl_wrapper/src/thread_private_data_ctl.cpp:21-23`
定义：

```cpp
pthread_key_t  ThreadPrivateDataCtl::key_     = PTHREAD_KEY_T_NOT_INITIALIZED;
pthread_key_t  ThreadPrivateDataCtl::tableKey_ = PTHREAD_KEY_T_NOT_INITIALIZED;
pthread_once_t ThreadPrivateDataCtl::onceCtl_ = PTHREAD_ONCE_INIT;
```

该 TU 同时被三个 `ohos_shared_library` 目标编译进各自的产物（`frameworks/opengl_wrapper/BUILD.gn`）：

| target | sources 行 | 输出 |
| --- | --- | --- |
| `EGL` | `BUILD.gn:85` `src/thread_private_data_ctl.cpp` | `libEGL.so` |
| `GLESv1` | `BUILD.gn:148` `src/thread_private_data_ctl.cpp` | `libGLESv1.so` |
| `GLESv2` | `BUILD.gn:191` `src/thread_private_data_ctl.cpp` | `libGLESv2.so` |

由于 `key_` / `tableKey_` / `onceCtl_` 是类内静态（命名空间作用域，非 `extern`），链接器在每个 `.so`
内各自生成一份私有副本：libEGL 的 `key_` 与 libGLESv2 的 `key_` 位于不同地址。后果：

- **写入端与读取端分属不同 `.so`**：libEGL 在 `egl_wrapper_display.cpp:178, 264`、
  `egl_wrapper_hook.cpp:66, 68`、`egl.cpp:85`、`egl_core.cpp:132` 调用 `SetGlHookTable` /
  `SetContext` / `SetError`，但读取端 `GetGlHookTable` / `GetContext` / `GetError` 实际由
  `src/GLES1/gles1.cpp:22, 33` 与 `src/GLES2/gles2.cpp:22, 33` 在 libGLESv1 / libGLESv2 内编译，
  命中的是另一个 `.so` 的静态副本。
- **早返回路径被频繁命中**：`thread_private_data_ctl.cpp:77, 110, 130` 在读到
  `PTHREAD_KEY_T_NOT_INITIALIZED` 时直接返回 `EGL_SUCCESS` / `EGL_NO_CONTEXT` / `nullptr`，
  而该哨兵值是静态初始值，三个 `.so` 互相看不到对方的 `pthread_key_create` 结果，跨 `.so` 调用
  实际上一致落在「未初始化」分支。
- **TLS 槽位与 once 控制完全失效**：`onceCtl_` 各自执行一次 `pthread_key_create`，三套 TLS key
  互不相通，线程私有数据（Hook 表、context、error）无法在 EGL 与 GLES 间传递，问题一中规划的
  TLS 优化路径同样会被此问题放大。

修复方向（按改动面由小到大）：

1. 把 `thread_private_data_ctl.cpp` 从 `GLESv1` / `GLESv2` 的 sources 中移除，改为通过
   `external_deps` / `public_external_deps` 链接 libEGL（或一个独立的 `libglwrapper_common.so`）
   提供的 `ThreadPrivateDataCtl` 符号，确保全进程只有一份定义。
2. 若必须保持三份产物独立，可将三个静态成员改为 `extern` 并集中放到一个公共共享库里，
   由所有 wrapper `.so` 依赖同一份定义。
3. 在重构落地前，至少通过 `nm -D` 在三份 `.so` 中确认 `ThreadPrivateDataCtl::key_` 等符号
   的可见性，作为已知行为登记。

影响范围：所有依赖 EGL 与 GLES 协同的应用（WebGL、游戏、视频），与问题一所述 TLS 性能优化直接耦合。

## 待补充背景

- Hook 表初始化与 GPU 驱动加载的具体流程
- WebGL 调用如何映射到 Wrapper 层
- ThreadPrivateDataCtl 的线程安全保证机制（含问题一所述 TLS 优化路径）
- GL4 支持的产品形态与启用条件
- Wrapper 层与 GPU 驱动的错误处理与 fallback 策略
- 问题二补充接口的驱动侧支持情况
- 问题五统一 LayerManager 的架构选型与迁移路径
- 问题六静态副本拆分到独立公共库的迁移步骤与符号可见性验证方法
