# Render Frame Trace

## 适用范围

- 渲染帧追踪（RenderFrameTrace、RenderFrameTraceImpl）
- 作用域 trace 封装（ScopedBytrace、ScopedDebugTrace）
- RS trace 宏定义与 HiTrace 集成（rs_trace.h）
- 可选 trace 与分级 trace（RSOptionalTrace、RSProcessTrace、rs_optional_trace.h）
- 帧感知 trace（FrameAwareTrace）与 frame_aware_sched 子系统集成
- trace 开关控制与系统参数联动

## 快速代码地图

| 模块 | 关键文件 | 相对路径 |
| --- | --- | --- |
| Frame Trace 接口 | `render_frame_trace.h` | `rosen/modules/render_frame_trace/include/` |
| Frame Trace 实现 | `render_frame_trace_impl.h/.cpp` | `rosen/modules/render_frame_trace/include/`、`src/` |
| Frame Trace 默认实现 | `render_frame_trace.cpp` | `rosen/modules/render_frame_trace/src/` |
| ScopedBytrace | `scoped_bytrace.h/.cpp` | `utils/scoped_bytrace/export/`、`src/` |
| RS Trace 宏定义 | `rs_trace.h` | `utils/log/` |
| RS Trace 跨平台 | `rs_trace_crossplatform.h` | `utils/log/` |
| 可选/分级 Trace | `rs_optional_trace.h` | `rosen/modules/render_service_base/include/common/` |
| 单测 | `render_frame_trace_test.cpp`、`render_frame_trace_impl_test.cpp` | `rosen/test/render_frame_trace/unittest/` |

## 核心模型

### RenderFrameTrace：帧感知 trace 桥接

RenderFrameTrace 是抽象基类，提供 `RenderStartFrameTrace`/`RenderEndFrameTrace` 接口，通过 `RegisterImpl()` 静态方法实现运行时多态。默认实现所有方法为空操作。

**注册机制**：`RenderFrameTraceImpl` 在全局构造时自动调用 `RegisterImpl(this)`，将自身注册为唯一实现。`GetInstance()` 返回 `implInstance_` 指针，如果未注册则 new 一个默认实例。

**RenderFrameTraceImpl**：在 `FRAME_TRACE_ENABLE` 宏保护下，将调用桥接到 `frame_aware_sched` 子系统的 `QuickStartFrameTrace`/`QuickEndFrameTrace` 和 `FrameAwareTraceOpen/IsOpen/Close`。

**数据流**：RS 渲染管线 → `RenderFrameTrace::GetInstance().RenderStartFrameTrace(tag)` → `RenderFrameTraceImpl` → `QuickStartFrameTrace(tag)` → frame_aware_sched

### ScopedBytrace：作用域 trace 封装

利用 RAII 自动管理 `StartTrace`/`FinishTrace` 对。构造时调用 `StartTrace(HITRACE_TAG_GRAPHIC_AGP, proc)`，析构时自动 `FinishTrace`。支持提前调用 `End()` 手动结束。

**ScopedDebugTrace**：受 `persist.sys.graphic.openDebugTrace` 系统参数控制，仅在开启时输出 trace。用于开发阶段高频调试 trace，避免 Release 版 trace 开销。

### RS Trace 宏体系（rs_trace.h）

统一使用 `HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL` 双标签。核心宏：

| 宏 | 功能 | HiTrace 对应 |
| --- | --- | --- |
| `RS_TRACE_BEGIN(name)` | 开始同步 trace | `StartTrace(tag, name)` |
| `RS_TRACE_END()` | 结束同步 trace | `FinishTrace(tag)` |
| `RS_TRACE_NAME(name)` | 作用域同步 trace | `HITRACE_METER_NAME(tag, name)` |
| `RS_TRACE_NAME_FMT(fmt, ...)` | 格式化作用域 trace | `HITRACE_METER_FMT(tag, fmt, ...)` |
| `RS_ASYNC_TRACE_BEGIN(name, value)` | 开始异步 trace | `StartAsyncTrace(tag, name, value)` |
| `RS_ASYNC_TRACE_END(name, value)` | 结束异步 trace | `FinishAsyncTrace(tag, name, value)` |
| `RS_TRACE_INT(name, value)` | 计数 trace | `CountTrace(tag, name, value)` |
| `RS_TRACE_FUNC()` | 函数名 trace | `RS_TRACE_NAME(__func__)` |

**USER 变体**：`RS_USER_TRACE_*` 系列宏自动附加 `[screen_id=%llu]` 前缀，用于多屏幕场景区分。

**分级 trace**：DEBUG / INFO / CRITICAL / COMMERCIAL 四级，对应 `HiTraceOutputLevel`。通过 `*_DEBUG`/`*_INFO`/`*_CRITICAL`/`*_COMMERCIAL` 后缀宏控制输出级别。

**条件编译**：`ROSEN_TRACE_DISABLE` 宏下所有 trace 宏为空操作；`ROSEN_ANDROID` 下使用跨平台实现 `rs_trace_crossplatform.h`。

### RSOptionalTrace（rs_optional_trace.h）

基于 `RSSystemProperties::GetDebugTraceEnabled()` 动态控制 trace 输出。提供：

- `RS_OPTIONAL_TRACE_BEGIN/END`：受 debug 开关控制
- `RS_OPTIONAL_TRACE_NAME_FMT`：格式化可选 trace
- `RS_OPTIONAL_TRACE_FMT`：格式化可选 trace（unique_ptr 管理）
- `RS_APPOINTED_TRACE_BEGIN/END`：针对指定节点的 trace（`FindNodeInTargetList`）
- `RS_OPTIONAL_TRACE_BEGIN_LEVEL(Level)`：基于 trace level 的分级控制
- `RS_PROCESS_TRACE(forceEnable, name)`：可强制启用的 process trace

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| RenderFrameTrace 用桥接模式 | 抽象基类 + `RegisterImpl` + Impl 子类 | 解耦 RS 与 frame_aware_sched 的编译依赖，可选加载 |
| Impl 全局构造自注册 | `RenderFrameTraceImpl::instance_ = new RenderFrameTraceImpl()` 在静态初始化时注册 | 无需显式初始化调用，链接即生效 |
| FRAME_TRACE_ENABLE 宏保护 | `AccessFrameTrace()` 检查宏 | 产品可裁剪 frame_aware_sched 依赖 |
| ScopedBytrace RAII | 构造 StartTrace，析构 FinishTrace | 避免手动配对遗漏，异常路径也能正确结束 trace |
| 双标签 AGP + COMMERCIAL | `HITRACE_TAG_GRAPHIC_AGP \| HITRACE_TAG_COMMERCIAL` | AGP 标签用于开发 trace，COMMERCIAL 标签用于商用 trace 采集 |
| USER 变体带 screen_id | `RS_USER_TRACE_*` 自动附加 `[screen_id=X]` | 多屏场景下区分各屏幕的 trace 数据 |
| 分级 trace 四级 | DEBUG/INFO/CRITICAL/COMMERCIAL | 不同场景采集不同级别的 trace，减少无用数据 |
| ScopedDebugTrace 受系统参数控制 | `persist.sys.graphic.openDebugTrace` | 开发 trace 默认关闭，按需开启避免性能影响 |
| RSOptionalTrace 运行时开关 | `RSSystemProperties::GetDebugTraceEnabled()` | 全局 trace 开关，运行时动态生效 |
| ROSEN_TRACE_DISABLE 空操作 | `#else` 分支全部宏为空 | Release 构建完全消除 trace 开销 |

## 待补充背景

- `RenderFrameTraceImpl` 对 `QuickStartFrameTrace`/`QuickEndFrameTrace` 的调用时机和 RS 渲染管线中的埋点位置。
- `FRAME_TRACE_ENABLE` 宏在哪些产品配置中开启/关闭。
- `ScopedDebugTrace` 的 `persist.sys.graphic.openDebugTrace` 参数在哪些场景下需要手动设置。
- `RSSystemProperties::GetDebugTraceLevel()` 的取值范围和各级别含义。
- `RS_APPOINTED_TRACE_*` 的 `FindNodeInTargetList` 节点列表如何配置。
- 多屏 USER trace 的实际使用场景和典型采集命令。
- `HITRACE_TAG_COMMERCIAL` 与商用 trace 采集平台的集成方式。
- 历史线上问题：trace 不配对导致 perfetto 解析异常、高频 trace 导致性能回退等案例。
