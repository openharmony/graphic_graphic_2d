# graphic_test 与 DTK

## 适用范围

- 图形测试框架（RSGraphicTest、RSGraphicTestDirector）
- 设备测试套件 DTK（TestBase、DtkTestRegister、TestUtils）
- 像素级截图比对与测试用例注册
- Profiler 测试（NodeTree 加载、Playback 回放）
- 子窗口测试与截图范围控制
- 2D Drawing 功能测试（Bitmap、Pixmap、Font 等）
- Render Service 功能测试（drawing_engine、render_service_render_profiler_test 等）

## 快速代码地图

| 模块 | 关键文件 | 相对路径 |
| --- | --- | --- |
| 测试框架入口 | `BUILD.gn` | `graphic_test/` |
| 测试框架 - GTest 基类 | `rs_graphic_test.h/.cpp` | `graphic_test/graphic_test_framework/include/`、`src/` |
| 测试框架 - Director | `rs_graphic_test_director.h/.cpp` | `graphic_test/graphic_test_framework/include/`、`src/` |
| 测试框架 - 截图与图像 | `rs_graphic_test_img.h/.cpp` | `graphic_test/graphic_test_framework/include/`、`src/` |
| 测试框架 - Profiler | `rs_graphic_test_profiler.h/.cpp` | `graphic_test/graphic_test_framework/include/`、`src/` |
| 测试框架 - Profiler 线程 | `rs_graphic_test_profiler_thread.h/.cpp` | `graphic_test/graphic_test_framework/include/`、`src/` |
| 测试框架 - 扩展 | `rs_graphic_test_ext.h` | `graphic_test/graphic_test_framework/include/` |
| 测试框架 - RootNode | `rs_graphic_rootnode.h/.cpp` | `graphic_test/graphic_test_framework/include/`、`src/` |
| 测试框架 - 工具 | `rs_graphic_test_utils.h/.cpp` | `graphic_test/graphic_test_framework/include/`、`src/` |
| 测试框架 - 参数解析 | `rs_parameter_parse.h/.cpp` | `graphic_test/graphic_test_framework/include/`、`src/` |
| 测试框架 - 错误码 | `rs_graphic_errors.h` | `graphic_test/graphic_test_framework/include/` |
| 测试框架 - 日志 | `rs_graphic_log.h` | `graphic_test/graphic_test_framework/include/` |
| 测试框架 - 文本 | `rs_graphic_test_text.h/.cpp` | `graphic_test/graphic_test_framework/include/`、`src/` |
| 测试框架 - main | `rs_graphic_test_main.cpp` | `graphic_test/graphic_test_framework/src/` |
| 测试框架 - 构建 | `BUILD.gn` | `graphic_test/graphic_test_framework/` |
| DTK - 测试基类 | `dtk_test_base.h/.cpp` | `rosen/test/dtk/` |
| DTK - 用例注册 | `dtk_test_register.h/.cpp` | `rosen/test/dtk/` |
| DTK - 工具函数 | `dtk_test_utils.h/.cpp` | `rosen/test/dtk/` |
| DTK - 扩展宏 | `dtk_test_ext.h` | `rosen/test/dtk/` |
| DTK - 常量 | `dtk_constants.h` | `rosen/test/dtk/` |
| DTK - 入口 | `dtk.cpp` | `rosen/test/dtk/` |
| DTK - 构建 | `BUILD.gn` | `rosen/test/dtk/` |
| DTK - Bitmap 测试 | `function/Bitmap/dtk_Bitmap_Scene*.cpp` | `rosen/test/dtk/function/` |
| DTK - Pixmap 测试 | `function/Pixmap/dtk_Pixmap_Scene*.cpp` | `rosen/test/dtk/function/` |
| DTK - Font 测试 | `function/font/dtk_Font_Scene_Transform_*.cpp` | `rosen/test/dtk/function/` |
| 测试用例 - Drawing Engine | `drawing_engine/` | `graphic_test/test/` |
| 测试用例 - Effect Kit | `effect_kit/` | `graphic_test/test/` |
| 测试用例 - Render Service Profiler | `render_service_render_profiler_test.cpp` | `graphic_test/test/` |
| 测试用例 - RS 功能 | `rs_func_feature/`、`rs_framework/` 等 | `graphic_test/test/` |

## 核心模型

### graphic_test 框架

**RSGraphicTest**：继承 `testing::Test`，是所有图形测试的 GTest 基类。提供：

- `GetRootNode()`：获取根节点
- `GetScreenSize()`：获取屏幕尺寸
- `SetSurfaceBounds/SetScreenSurfaceBounds`：设置 Surface 边界
- `RegisterNode(node)`：注册待测试节点
- `CreateSubWindow(options)`：创建子窗口
- `SetCaptureScope(scope)`：设置截图范围（`MAIN_SURFACE` 或 `FULL_DISPLAY`）
- `BeforeEach/AfterEach`：用户覆盖的初始化/清理
- `AddFileRenderNodeTreeToNode`：从文件加载节点树
- `PlaybackRecover/PlaybackStop`：回放控制

**生命周期**：`SetUpTestCase` → `SetUp` → `BeforeEach` → [测试执行] → `AfterEach` → `TearDown` → `TearDownTestCase`

**RSGraphicTestDirector**：全局单例，管理测试执行环境：

- `Run()`：启动渲染循环
- `FlushMessage/FlushMessageAndWait`：刷新 RS 消息并等待
- `TakeScreenCaptureAndWait`：截图并等待
- `SendProfilerCommand`：发送 Profiler 命令
- VSync 等待与 UI 动画控制
- 内部使用 `EventRunner`/`EventHandler` 驱动消息循环

**RSGraphicTestProfiler**：Profiler 测试支持：

- `RunNodeTreeTest(path)`：从指定路径加载节点树文件进行测试
- `RunPlaybackTest(filePath)`：回放测试
- `PlaybackInfo`：回放配置（文件名、OHR 类型、起止时间、时间间隔）
- 支持 Buffer Dump 模式（`SetUseBufferDump`）和中断标志（`SetInterruptFlag`）

**RSGraphicTestImg**：图像工具：

- `DecodePixelMap`：解码图片为 PixelMap
- `SetUpNodeBgImage`：创建带背景图的节点
- `ImageCustomModifier`：自定义图片绘制 Modifier

**CaptureScope**：控制截图范围

- `MAIN_SURFACE`：仅截主 Surface
- `FULL_DISPLAY`：截全屏

### DTK 测试套件

**TestBase**：DTK 测试基类，提供 Drawing Canvas 操作的辅助方法：

- `Recording()`：执行录制
- `AddTestBrush/Pen`：创建测试画刷/画笔
- `ClipPath/Rect/RoundRect`：裁剪操作
- `GetEffectTestImage`：获取测试图片
- `MakeImage`：创建 Drawing Image
- 子类覆盖 `OnRecording()` 实现具体绘制逻辑

**DtkTestRegister**：测试用例注册中心，维护 `testCaseMap`（名称 → 创建函数）。支持 `TestLevel`（L0-L9、PERF=100）和用例索引。

**TestUtils**：Drawing 操作的工具集合，提供 18 种绘制函数（Point、Line、Rect、Oval、Path、ImageRect、TextBlob、Vertices 等），每种函数接受 Pen、Brush 和 Image 参数。

**dtk_constants.h**：测试常量定义，包括绘制坐标、BlendMode 数组（29 种）、BlurType 数组（4 种）、CMS 矩阵、PQ 传输函数等。

### 测试用例组织

`graphic_test/test/` 下按功能分类：

| 目录 | 测试内容 |
| --- | --- |
| `drawing_engine/` | 2D 绘制引擎功能测试 |
| `effect_kit/` | 效果套件测试 |
| `hardware_manager/` | 硬件管理测试 |
| `open_capability/` | 开放能力测试 |
| `rs_display_effect/` | 显示效果测试 |
| `rs_effect_feature/` | 特效功能测试 |
| `rs_framework/` | RS 框架测试 |
| `rs_func_feature/` | RS 功能特性测试 |
| `rs_perform_feature/` | RS 性能测试 |
| `test_template/` | 测试模板 |

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| RSGraphicTest 继承 GTest | `class RSGraphicTest : public testing::Test` | 复用 GTest 基础设施（过滤、并行、报告），降低学习成本 |
| BeforeEach/AfterEach 虚函数 | 覆盖 GTest 的 SetUp/TearDown 为 final | 统一生命周期管理，防止子类误覆盖 |
| Director 单例 | `RSGraphicTestDirector::Instance()` | 全局唯一渲染环境，避免多实例冲突 |
| CaptureScope 区分截图范围 | `MAIN_SURFACE` vs `FULL_DISPLAY` | 主 Surface 截图速度快，全屏截图用于验证合成结果 |
| DtkTestRegister 用例注册 | `regist(testName, testCase, testLevel, index)` | 动态注册测试用例，支持按名称/级别筛选 |
| TestLevel 分级 | L0-L9 + PERF=100 | 区分基础功能和性能测试，按需执行 |
| Profiler 测试支持 Buffer Dump | `SetUseBufferDump(true)` | 直接读取 GPU Buffer 数据，精度高于屏幕截图 |
| 资源编译为字节码 | `build_resource_to_bytecode.py` | 测试资源嵌入二进制，避免运行时文件依赖 |
| RS_PROFILER_ENABLED 条件编译 | `BUILD.gn` 中 `defines += [ "RS_PROFILER_ENABLED" ]` | Profiler 测试仅在完整环境可用 |
| DTK 可执行目标 | `ohos_executable("dtk")` | 独立可执行文件，可直接在设备上运行 |

## 待补充背景

- RSGraphicTest 的截图比对算法和像素差异阈值。
- `TestDefManager`（在 director 中作为 friend 引用）的用例管理和执行调度逻辑。
- DTK 在设备上的典型执行命令和输出格式。
- `rs_graphic_test_main.cpp` 中 main 函数的 GTest 初始化流程。
- Profiler 测试的 Playback 文件格式和 JSON 配置结构。
- Buffer Dump 模式的实现细节（如何从 GPU Buffer 读取数据）。
- 子窗口测试（`CreateSubWindow`）的使用场景和验证方式。
- 各 `rs_*_feature/` 测试目录下的具体用例清单和覆盖范围。
- 历史线上问题：截图比对误判、VSync 等待超时、Profiler 回放不一致等案例。
