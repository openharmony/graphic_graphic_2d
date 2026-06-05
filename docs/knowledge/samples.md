# 演示工程与示例

## 适用范围

改动涉及 rosen/samples 演示工程、示例代码和 drawing engine sample 时，先读本文，再回到代码确认当前实现。

本文是背景知识和排查路线，不替代代码。修改前仍需读取对应头文件、实现文件和测试。

## 快速代码地图

| 方向 | 关键文件（完整路径） |
| --- | --- |
| 2D 绘制示例 | `rosen/samples/2d_graphics/` |
| 2D 示例构建 | `rosen/samples/2d_graphics/BUILD.gn` |
| 2D 示例入口 | `rosen/samples/2d_graphics/main.cpp` |
| 2D 引擎示例 | `rosen/samples/2d_graphics/drawing_engine_sample.cpp` |
| 渲染后端 | `rosen/samples/2d_graphics/drawing_engine/` |
| Surface 适配 | `rosen/samples/2d_graphics/drawing_engine/drawing_surface/` |
| Benchmark 框架 | `rosen/samples/2d_graphics/benchmarks/benchmark.h`、`benchmark_config.h`、`benchmark_result.h` |
| Benchmark 配置 | `rosen/samples/2d_graphics/benchmarks/benchmark_config.cpp` |
| 单线程 benchmark | `rosen/samples/2d_graphics/benchmarks/benchmark_singlethread/` |
| 多线程 benchmark | `rosen/samples/2d_graphics/benchmarks/benchmark_multithread/` |
| API benchmark | `rosen/samples/2d_graphics/benchmarks/benchmark_api/` |
| DCL benchmark | `rosen/samples/2d_graphics/benchmarks/benchmark_dcl/` |
| C API 示例 | `rosen/samples/2d_graphics/drawing_c_sample.cpp` |
| RS 绘制示例 | `rosen/samples/2d_graphics/drawing_sample_rs.cpp` |
| DCL 回放 | `rosen/samples/2d_graphics/drawing_sample_replayer.cpp` |
| VSync 示例 | `rosen/samples/hello_vsync/hello_vsync.cpp` |
| Rosen 基础示例 | `rosen/samples/hello_rosen/HelloRosen.cpp` |
| NativeBuffer 示例 | `rosen/samples/hello_native_buffer/hello_native_buffer.cpp` |
| NativeImage 示例 | `rosen/samples/hello_native_image/hello_native_image.cpp` |
| NativeWindow 示例 | `rosen/samples/hello_native_window/hello_native_window.cpp` |
| Composer 示例 | `rosen/samples/composer/`（`hello_composer.cpp`、`layer_context.cpp`） |
| OpenGL 测试 | `rosen/samples/opengl/`（`test_eglCreateWindowSurface/`、`test_glCompressApi/` 等） |
| WebGL 示例 | `rosen/samples/webgl/entry/` |
| Text 示例 | `rosen/samples/text/renderservice/` |
| Inner API Demo | `rosen/samples/inner_api_demo/inner_api_demo.h` |

## 核心模型

### 2D 绘制引擎示例（drawing_engine_sample）

这是本仓最完整的示例工程，构建 target 为 `drawing_engine_sample`。它演示了 2D 绘制全链路：

**入口结构**：
- `main.cpp`：程序入口，解析命令行参数。
- `drawing_engine_sample.cpp`：示例主逻辑，协调渲染循环。

**渲染后端**（`drawing_engine/`）：
- `canvas_context.cpp`：Canvas 上下文管理。
- `drawing_proxy.cpp`：绘制代理。
- `software_render_backend.cpp`：CPU 软件渲染后端。
- GPU 渲染后端由 `graphic_config.gni` 中的特性开关控制：
  - `graphic_2d_feature_enable_opengl` → `gles_render_backend.cpp`、`egl_manager.cpp`。
  - `graphic_2d_feature_enable_vulkan` → `vulkan_render_backend.cpp`。

**Surface 适配**（`drawing_engine/drawing_surface/`）：
- `surface_ohos.cpp`、`surface_ohos_raster.cpp`、`surface_frame_ohos_raster.cpp`：OHOS 平台 Surface 基础。
- `surface_ohos_gl.cpp`、`surface_frame_ohos_gl.cpp`：OpenGL ES Surface（条件编译）。
- `surface_ohos_vulkan.cpp`、`surface_frame_ohos_vulkan.cpp`：Vulkan Surface（条件编译）。

**构建依赖**：
- `rosen/modules/composer:libcomposer`
- `frameworks/opengl_wrapper:EGL`、`GLESv3`
- `rosen/modules/render_service_base:librender_service_base`
- 外部：`skia:skia_canvaskit`、`openssl:libcrypto_shared`、`graphic_surface:sync_fence`

### Benchmark 框架

`benchmarks/` 目录提供了 2D 绘制性能基准测试框架，通过命令行参数选择测试类型：

**BenchmarkConfig**（`benchmark_config.cpp`）支持四种模式：
- `singlethread`：单线程绘制性能测试。
- `multithread`：多线程绘制性能测试。
- `api`：API 级别性能测试。
- `dcl`：DCL（Drawing Command List）回放性能测试。

**DCL 子系统**（`benchmark_dcl/`）：
- `drawing_command.cpp`：绘制命令录制。
- `drawing_playback.cpp`：DCL 回放。
- `skia_recording.cpp`：Skia 录制适配。

### 其他示例

| 示例 | 功能 | 关键依赖 |
| --- | --- | --- |
| `drawing_sample_rs` | 通过 RS Client C++ API 绘制 | `2d_graphics`、`native_drawing_ndk`、`librender_service_client` |
| `drawing_sample_replayer` | DCL 回放器 | `librender_service_client`、`librender_service_base` |
| `hello_vsync` | VSync 接收演示 | `libcomposer` |
| `hello_rosen` | Rosen 渲染框架基础 | `librender_service_client` |
| `hello_native_buffer` | NativeBuffer API 使用 | `graphic_surface` |
| `hello_native_image` | NativeImage API 使用 | `libnative_image` |
| `hello_native_window` | NativeWindow API 使用 | `graphic_surface` |
| `composer` | HWC 合成演示 | `libcomposer` |
| `opengl/*` | OpenGL ES API 测试 | `EGL`、`GLESv2`/`GLESv3` |
| `webgl/entry` | WebGL 示例 | WebGL NAPI |
| `text/renderservice` | 文本渲染服务示例 | `rosen_text` |

### 构建约定

所有示例 target 的 `part_name = "graphic_2d"`、`subsystem_name = "graphic"`。主要示例默认 `install_enable = true`，可部署到设备上运行。

2D 绘制示例的构建 target 对应 AGENTS.md 中的路径：
```
$G2D/rosen/samples/2d_graphics:drawing_engine_sample
```

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 多渲染后端条件编译 | `BUILD.gn` 中 `rs_enable_gpu` + `enable_vulkan`/`enable_opengl` 控制 sources | 同一示例可运行在软件渲染、OpenGL、Vulkan 三种后端，按产品配置自动选择 |
| Benchmark 命令行模式选择 | `BenchmarkConfig::SetBenchMarkType` 解析字符串参数 | 同一可执行文件覆盖多种性能场景，避免维护多个 benchmark 二进制 |
| hello_* 系列最小示例 | 每个目录仅一个 .cpp + BUILD.gn | 演示单一 API 的最小用法，降低学习门槛 |
| C API 示例独立 | `drawing_c_sample.cpp` 使用 NDK C API | 证明 NDK C API 可独立于 C++ API 使用，面向第三方应用 |
| DCL 回放独立示例 | `drawing_sample_replayer.cpp` 仅依赖 `librender_service_client` | DCL 回放不需要完整渲染引擎，仅需 RS Client 提交录制数据 |
| 内部 API demo 仅头文件 | `inner_api_demo/inner_api_demo.h` 无 BUILD.gn | inner_api_demo 可能由其他构建系统集成，本仓仅提供头文件定义 |
| sanitize 配置 | `drawing_engine_sample` 启用 cfi + cfi_cross_dso | 示例代码也遵循安全编译要求，与生产代码一致 |

## 待补充背景

这些内容需要模块责任人后续补充，不能仅靠静态扫描完全确定：

- Benchmark 的运行参数、数据采集方法和性能基线。
- `inner_api_demo` 的实际使用方式和构建入口。
- `opengl/` 下各测试用例的运行前提和预期结果。
- `webgl/entry` 的完整部署方式和依赖的 Node.js 工具链。
- 各示例在设备上的实际运行方式和输出格式。
- `text/renderservice` 示例与 Texgine 模块的关系。
- `drawing_engine_sample` 与 2D benchmark 在 CI 中的使用方式和回归检测策略。
