# GLFW Render Context

## 适用范围

- GLFW 跨平台窗口与 OpenGL 上下文管理
- 2D Graphics 跨平台（Windows/macOS/Linux）渲染上下文
- Sample / Benchmark 工具的窗口创建
- 键盘鼠标输入事件转发
- 窗口大小变更回调
- 剪贴板操作

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| glfw_render_context.h | `rosen/modules/glfw_render_context/export/glfw_render_context.h` | GlfwRenderContext 类声明 |
| glfw_render_context.cpp | `rosen/modules/glfw_render_context/src/glfw_render_context.cpp` | GlfwRenderContext 实现 |
| BUILD.gn | `rosen/modules/glfw_render_context/BUILD.gn` | 构建配置 |

## 核心模型

**GlfwRenderContext** 封装 GLFW 窗口和 OpenGL 上下文管理，为跨平台 2D Graphics 渲染提供窗口基础：

```
GlfwRenderContext (非单例，但提供全局实例)
  ├─ GetGlobal()  → 获取全局共享实例
  ├─ InitFrom(void* glfwWindow) → 从已有 GLFW 窗口初始化
  │
  ├─ 窗口管理:
  │   ├─ Init() → 初始化 GLFW
  │   ├─ CreateGlfwWindow(width, height, visible)
  │   ├─ DestroyWindow()
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
- `OnCursorPosFunc`：`void(double x, double y)`
- `OnKeyFunc`：`void(int key, int scancode, int action, int mods)`
- `OnCharFunc`：`void(unsigned int codepoint)`
- `OnSizeChangedFunc`：`void(int32_t width, int32_t height)`

**内部实现**：
- `window_`：`GLFWwindow*` 原始指针
- `external_`：是否从外部窗口初始化（不负责销毁）
- GLFW 静态回调 → 转发到用户回调
- `global_`：静态全局实例（`inline` 定义）

数据流：GlfwRenderContext 创建窗口 → MakeCurrent 绑定 GL 上下文 → 2D Graphics 在窗口 Surface 上渲染 → SwapBuffers 提交帧。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 非单例 + 全局实例 | `GetGlobal()` 提供，但注释说明"不是单例" | 默认共享全局实例，但允许创建独立实例 |
| InitFrom 外部窗口 | `InitFrom(void* glfwWindow)` + `external_` 标志 | 支持嵌入已有 GLFW 窗口（如跨平台框架），不接管生命周期 |
- 静态 GLFW 回调 | GLFW 回调必须是静态函数 | GLFW C API 限制，通过成员回调转发到实例 |
| 事件回调模式 | `OnMouseButton` 等注册回调 | 解耦事件处理，上层可自由响应输入事件 |
| visible 参数 | `CreateGlfwWindow(width, height, visible)` | 离屏渲染时可创建不可见窗口 |

## 待补充背景

- GlfwRenderContext 与 RenderContextGL 的协作关系
- 跨平台构建时 GLFW 依赖的处理方式
- 全局实例的生命周期管理
- Sample / Benchmark 中使用 GlfwRenderContext 的完整示例
