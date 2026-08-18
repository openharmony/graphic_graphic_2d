# GLFW Render Context

## 适用范围

- GLFW 跨平台窗口与 OpenGL 上下文管理
- 2D Graphics 跨平台（Windows/macOS/Linux）渲染上下文
  - 注：Windows/macOS 已接入 RSSurface（`render_service_base/src/platform/darwin`、`.../windows`）；
    Linux 在本仓未落地 surface 适配，`render_service_base/src/platform/` 下无 linux 目录
- Sample / Benchmark 工具的窗口创建
  - 注：仓内无 GlfwRenderContext 的 sample/benchmark/单测/fuzz；
    `Init/CreateGlfwWindow/InitFrom/Terminate` 由仓外 preview 宿主调用，
    本仓只消费 `MakeCurrent/SwapBuffers/OnSizeChanged`
- 键盘鼠标输入事件转发
- 窗口大小变更回调
- 剪贴板操作

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| glfw_render_context.h | `rosen/modules/glfw_render_context/export/glfw_render_context.h` | 类声明 |
| glfw_render_context.cpp | `rosen/modules/glfw_render_context/src/glfw_render_context.cpp` | 实现 |
| BUILD.gn | `rosen/modules/glfw_render_context/BUILD.gn` | 构建配置 |

模块 owner：`@aslklw`（`.gitcode/CODEOWNERS:11`）；对外头文件经 `bundle.json:241-246` 导出，
header_base 指向 `.../export`。BUILD.gn 在 `rosen_preview` 下产 `.so`，否则为空 group。

## 核心模型

**GlfwRenderContext** 封装 GLFW 窗口和 OpenGL 上下文管理，
为跨平台 2D Graphics 渲染提供窗口基础：

```
GlfwRenderContext (非单例，但提供全局实例)
  ├─ GetGlobal()  → 获取全局共享实例
  ├─ InitFrom(void* glfwWindow) → 从已有 GLFW 窗口初始化
  │
  ├─ 窗口管理:
  │   ├─ Init() → 初始化 GLFW，返回 glfwInit() 的 GLFW_TRUE/FALSE
  │   ├─ Terminate() → 销毁 GLFW（external_ 为真时 no-op）
  │   ├─ CreateGlfwWindow(width, height, visible) → 0=成功，1=创建失败
  │   ├─ DestroyWindow()（external_ 为真时 no-op）
  │   ├─ WindowShouldClose()
  │   ├─ GetWindowSize() / SetWindowSize()
  │   └─ SetWindowTitle()
  │
  ├─ GL 操作:
  │   ├─ MakeCurrent() → 绑定 OpenGL 上下文
  │   └─ SwapBuffers() → 交换前后缓冲
  │
  ├─ 事件处理:
  │   ├─ WaitForEvents() / PollEvents()
  │   ├─ OnMouseButton(callback)
  │   ├─ OnCursorPos(callback)
  │   ├─ OnKey(callback)
  │   ├─ OnChar(callback)
  │   └─ OnSizeChanged(callback)
  │
  └─ 剪贴板:
      ├─ GetClipboardData()
      └─ SetClipboardData(data)
```

**回调类型定义**：
- `OnMouseButtonFunc`：`void(int button, bool pressed, int mods)`
  （`GLFW_REPEAT` 与 `GLFW_RELEASE` 都映射为 `pressed=false`，不区分重复）
- `OnCursorPosFunc`：`void(double x, double y)`
- `OnKeyFunc`：`void(int key, int scancode, int action, int mods)`
- `OnCharFunc`：`void(unsigned int codepoint)`
- `OnSizeChangedFunc`：`void(int32_t width, int32_t height)`

> 头文件仅这 5 个回调类型，不存在 `OnScrollFunc` / `OnDropFunc` / `OnWindowFocusFunc` /
> HiDPI / VSync 回调；`InitFrom` 内显式调 `glfwSetScrollCallback(window_, nullptr)` 关闭滚动事件，
> 但未对外暴露开关。

**内部实现**：
- `window_`：`GLFWwindow*` 原始指针
- `external_`：是否从外部窗口初始化（不负责销毁，影响 `Terminate`/`DestroyWindow` 行为）
- `global_`：`static inline std::shared_ptr<GlfwRenderContext>`，惰性构造，进程级生命周期
  （见"全局实例的生命周期管理"）
- `onMouseBotton_` / `onCursorPos_` / `onKey_` / `onChar_` / `onSizeChanged_`：5 个用户回调成员
  （注意 `onMouseBotton_` 拼写与头文件形参一致）
- `width_` / `height_`：缓存窗口尺寸，参与 `OnSizeChanged` 静态回调的"尺寸回写"逻辑
- GLFW 静态回调 → 通过 `glfwGetWindowUserPointer` 取回 `this` → 转发到用户回调

数据流（preview 路径）：仓外宿主 `Init/CreateGlfwWindow` 或 `InitFrom` 建立窗口 →
`MakeCurrent` 绑定 GL 上下文 → `Drawing::GPUContext::BuildFromGL` 构建 Skia GPUContext →
2D Graphics 在窗口 Surface 上渲染 → `SwapBuffers` 提交帧。
本仓内 `Init/CreateGlfwWindow/InitFrom` 无调用方，数据流不闭环。

## OnSizeChanged 静态回调：尺寸回写与风险

`OnSizeChanged` 静态回调（glfw_render_context.cpp:244-254）行为与其它回调不同，
存在反直觉的"尺寸回写"：

```cpp
const auto &that = reinterpret_cast<GlfwRenderContext *>(glfwGetWindowUserPointer(window));
if (that->width_ != width || that->height_ != height) {
    glfwSetWindowSize(window, that->width_, that->height_);  // 强制改回存储值
}
that->onSizeChanged_(that->width_, that->height_);  // 用存储值回调上层
```

- GLFW 上报的 `width/height` 与 `that->width_/height_` 不一致时，
  回调会调 `glfwSetWindowSize` 把窗口尺寸**强制改回存储值**，
  相当于"抵抗"外部 resize（如宿主拖拽边框）。
- 上层收到的 `OnSizeChangedFunc` 参数也是存储值 `that->width_/height_`，而非 GLFW 上报值。
- 该静态回调**未判 `that` 与 `that->onSizeChanged_` 是否为空**即解引用，
  存在潜在空指针风险（若未注册 `OnSizeChanged` 回调而 GLFW 触发了 size 回调）。
- `width_`/`height_` 由 `InitFrom`/`CreateGlfwWindow`/`SetWindowSize` 维护，
  是本模块对窗口尺寸的"权威缓存"。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 非单例 + 全局实例 | `GetGlobal()`（h:33 注释非单例） | 默认共享全局实例，允许独立实例 |
| InitFrom 外部窗口 | `InitFrom(void*)`+`external_`（cpp:63） | 嵌入已有窗口，不接管生命周期 |
| InitFrom 来源 | cpp:61-63 引用 `flutter_glfw.cc:39` | 外部窗口来自 Flutter 宿主，本仓无调用方 |
| InitFrom 清空回调 | cpp:65-72 置空 Char/Key/Scroll 等 | 清除宿主遗留回调；Scroll 被禁用未开放 |
| 静态 GLFW 回调 | 静态函数 + `glfwSetWindowUserPointer` | C API 限制，经 `reinterpret_cast` 转发 |
| 事件回调模式 | `OnMouseButton` 等注册回调 | 解耦事件处理，上层自由响应输入 |
| visible 参数 | `glfwWindowHint(GLFW_VISIBLE,...)` | 离屏渲染时可创建不可见窗口 |

> InitFrom 来源完整路径：`third_party/flutter/engine/flutter/shell/platform/glfw/flutter_glfw.cc`。

## 全局实例的生命周期管理

- `global_` 声明：`glfw_render_context.h:73`
  `static inline std::shared_ptr<GlfwRenderContext> global_ = nullptr;`
- `GetGlobal()` 实现（glfw_render_context.cpp:29-40）：双重检查锁（DCLP）+
  `static std::mutex mutex` + `std::lock_guard`，惰性 `make_shared`。构造期线程安全。
- **无 Release / Shutdown / Finalize 接口**：头文件与 cpp 均无重置 `global_` 的方法；
  `Terminate()` 只调 `glfwTerminate()`，**不重置 `global_`、不复位 `external_`、不置空 `window_`**。
  `global_` 一旦构造即活到进程结束（静态 `shared_ptr` 持有）。
- 调用方均通过 `GlfwRenderContext::GetGlobal()->Xxx()` 临时使用返回的 `shared_ptr`
  （表达式生命周期），不长期持有。
- 潜在问题：进程内若先 `InitFrom` 接管外部窗口、再期望切换到自建窗口，
  由于 `global_` 不可重置、`external_` 不可复位，全局实例无法切换模式，需新建独立实例。

## 与 RenderContextGL 的关系

二者**互斥**，是两条不同的 GL 上下文建立路径，并非组合关系：

| 路径 | 上下文建立 | 桥接到 Skia | 代表调用点 |
|------|-----------|-------------|-----------|
| Preview | `GetGlobal()->MakeCurrent()` | `GPUContext::BuildFromGL` | `rs_surface_darwin.cpp:174` 等 |
| OHOS/Sample/NDK | `RenderContextGL`（EGL） | EGL → Skia | `render_context_sample.cpp:93` 等 |

- Preview 代表调用点：`rs_surface_darwin.cpp:174`、`rs_surface_windows.cpp:178`。
- OHOS/Sample/NDK 代表调用点：`drawing_surface_utils.cpp:237-296`、`render_context_sample.cpp:93`。
- 隔离点：`rs_render_thread.cpp:333` `CreateAndInitRenderContextIfNeed()` 用
  `#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)) && !defined(ROSEN_PREVIEW)`
  包裹 `RenderContext::Create()`——**preview 下根本不创建 RenderContextGL**。
- 桥接点：两条路径最终都把"当前 GL 上下文"喂给 `Drawing::GPUContext::BuildFromGL()`，
  GlfwRenderContext 只是 preview 路径下"建立当前上下文"的前置步骤。
- RenderContextGL 文件：
  `rosen/modules/2d_graphics/src/render_context/new_render_context/render_context_gl.{h,cpp}`，
  详见 `docs/knowledge/render-context-gl-vk.md`。

## 跨平台构建与 GLFW 依赖

GLFW 源码在仓外 `//third_party/glfw`（OpenHarmony 源码根 `third_party/glfw`），不在本仓内。

- 本模块依赖声明：`rosen/modules/glfw_render_context/BUILD.gn:44`
  `deps = ["//third_party/glfw:glfw"]`（唯一 deps）
- OS 分支全部在 `third_party/glfw/BUILD.gn`，
  本模块 BUILD.gn 内部无 `is_mingw`/`is_mac`/`target_os` 分支：
  - mingw：win32 + wgl + egl，`_GLFW_WIN32`，libs `gdi32`/`opengl32`
  - mac：cocoa + nsgl + egl，`_GLFW_COCOA`，frameworks Cocoa/IOKit/CoreVideo/QuartzCore
  - linux：x11 + glx + egl，`_GLFW_X11`，libs rt/dl/X11/Xcursor/Xinerama/Xrandr
- 平台宏：`third_party/glfw/glfw_ace_config.gni:15-17` 定义
  `ace_windows_defines=["WINDOWS_PLATFORM"]`、`ace_mac_defines=["MAC_PLATFORM"]`、
  `ace_linux_defines=["LINUX_PLATFORM"]`

**本模块的三层构建/宏开关**：

| 宏/开关 | 定义位置 | 作用 |
|---------|---------|------|
| `rosen_preview` | `config.gni:16-28` | 守护 `.so` 产出；非 preview 退化为空 group |
| `enable_glfw_window` | `ace_platforms.gni:16`（默认 false） | 调用方按此注入 `USE_GLFW_WINDOW` |
| `USE_GLFW_WINDOW` | `platform/darwin/BUILD.gn` 等 | 守护 surface 中 SwapBuffers/像素 Y 翻转 |
| `ROSEN_PREVIEW` | render_service_client 侧 | 守护 `rs_render_thread.cpp` 注册 OnSizeChanged |

- `rosen_preview` = `rosen_is_mac || rosen_is_win || rosen_is_linux`。
- `USE_GLFW_WINDOW` 注入点：`render_service_base/src/platform/darwin/BUILD.gn:44-49`、
  `.../windows/BUILD.gn`。
- `ROSEN_PREVIEW` 同时在 `rs_render_thread.cpp:333` 隔离 `RenderContext::Create()`，
  在 `:395-402` 守护 `OnSizeChanged` 注册。
- `libglfw_render_context` 的真实消费者：`render_service_client/BUILD.gn:368-373`、
  `render_service_base/src/platform/darwin/BUILD.gn:60`、`.../windows/BUILD.gn:57`，
  `bundle.json:241-246` 声明为对外 so + 头文件。

## 真实使用点与示例缺口

**仓内无 GlfwRenderContext 的 sample / benchmark / 单测 / fuzz**：
- `rosen/samples/` 下 grep `GlfwRenderContext|GetGlobal|InitFrom|CreateGlfwWindow` 无匹配；
  `rosen/samples/2d_graphics/render_context_sample.cpp:93` 用的是 `RenderContextGL`，与本模块无关。
- `rosen/test/`、`test/`、`tools/`、`graphic_test/`、`rosen/test/dtk/` 下均无引用
  GlfwRenderContext 的用例。
- `rosen/test/2d_graphics/unittest/render_context/render_context_gl_test.cpp` 是 RenderContextGL 的单测。

**仓内真实消费点**（grep `GlfwRenderContext::GetGlobal` 精确命中 5 处 + 1 处定义）：

| 路径:行 | 调用 | 宏保护 |
|---------|------|--------|
| `rs_surface_darwin.cpp:127` | `GetGlobal()->SwapBuffers()` | `#ifdef USE_GLFW_WINDOW` |
| `rs_surface_darwin.cpp:174` | `GetGlobal()->MakeCurrent()` | 无（`SetupGrContext` 内） |
| `rs_surface_windows.cpp:131` | `GetGlobal()->SwapBuffers()` | `#ifdef USE_GLFW_WINDOW` |
| `rs_surface_windows.cpp:178` | `GetGlobal()->MakeCurrent()` | 无（`SetupGrContext` 内） |
| `rs_render_thread.cpp:401` | `GetGlobal()->OnSizeChanged(...)` | `#ifdef ROSEN_PREVIEW` |

**最完整用法（darwin/windows 预览渲染流，以 darwin 为例）**：
1. 仓外宿主 `GetGlobal()->Init()` → `CreateGlfwWindow(...)` 或 `InitFrom(hostWindow)`
   （本仓不可见，来源 Flutter glfw 宿主）；
2. `RSSurfaceDarwin::RequestFrame`（rs_surface_darwin.cpp:60-84）→ `SetupGrContext()`；
3. `SetupGrContext()`（:168-183）→ `GetGlobal()->MakeCurrent()` →
   `GPUContext::BuildFromGL(options)` → 得到 `grContext_`；
4. `FlushFrame`（:87-130）→ 读取像素 + Y 翻转（`#ifdef USE_GLFW_WINDOW`）→
   `onRender_(...)` → `#ifdef USE_GLFW_WINDOW` `GetGlobal()->SwapBuffers()`；
5. `RSRenderThread::RenderLoop`（rs_render_thread.cpp:395-402）→
   `#ifdef ROSEN_PREVIEW` 注册 `OnSizeChanged` 回调，回调内触发 `RequestNextVSync()`。

## 待补充背景

- `OnSizeChanged` 静态回调未对 `that` 与 `that->onSizeChanged_` 做空判即解引用
  （glfw_render_context.cpp:248-253），若未注册回调而 GLFW 触发 size 回调存在空指针风险。
- `DestroyWindow()`（cpp:119-128）未置 `window_ = nullptr`、未复位 `external_`，
  重复调用或后续 `Terminate` 行为依赖 GLFW 自身容错。
- Linux preview 在本仓未落地 surface 适配
  （`render_service_base/src/platform/` 无 linux 目录），三平台描述与实际落地不一致。
- 仓内对 GlfwRenderContext 完全无单测/fuzz/集成测试覆盖。
- `global_` 不可重置，进程内切换"外部窗口模式"与"自建窗口模式"需新建独立实例，
  无统一接口。
