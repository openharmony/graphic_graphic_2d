# Render Service Profiler

## 适用范围

- RS Profiler 录制/回放/保存模式
- 节点树序列化与反序列化（Marshal/Unmarshal Node、SubTree）
- Parcel 数据拦截与打补丁（PatchNodeId、PatchPid、PatchTime、PatchCommand）
- IPC 请求录制与重放（OnRemoteRequest 拦截）
- PixelMap/DrawingImage 序列化拦截
- Beta 录制（后台持续录制，触发时保留前后数据）
- Trace3D 集成（3D 渲染节点可视化）
- 自定义指标采集（RSProfilerCustomMetrics：节点数、模糊操作、动画等）
- 网络命令接口（Socket 通信、命令注册与分发）
- SKP 截图与离屏截图导出
- HRP 服务（文件浏览与加载）

## 快速代码地图

| 模块 | 关键文件 | 相对路径 |
| --- | --- | --- |
| Profiler 主头文件 | `rs_profiler.h` | `rosen/modules/render_service_profiler/` |
| Profiler 主实现 | `rs_profiler.cpp` | `rosen/modules/render_service_profiler/` |
| Profiler 基础 | `rs_profiler_base.cpp` | `rosen/modules/render_service_profiler/` |
| 命令参数 | `rs_profiler_command.h/.cpp` | `rosen/modules/render_service_profiler/` |
| Socket 通信 | `rs_profiler_socket.h/.cpp` | `rosen/modules/render_service_profiler/` |
| 网络通信 | `rs_profiler_network.h/.cpp` | `rosen/modules/render_service_profiler/` |
| 数据包 | `rs_profiler_packet.h/.cpp` | `rosen/modules/render_service_profiler/` |
| 文件 I/O | `rs_profiler_file.h/.cpp` | `rosen/modules/render_service_profiler/` |
| 归档 | `rs_profiler_archive.h/.cpp` | `rosen/modules/render_service_profiler/` |
| 设置/系统参数 | `rs_profiler_settings.h/.cpp` | `rosen/modules/render_service_profiler/` |
| 遥测/设备信息 | `rs_profiler_telemetry.h/.cpp` | `rosen/modules/render_service_profiler/` |
| Capture 数据 | `rs_profiler_capturedata.h/.cpp` | `rosen/modules/render_service_profiler/` |
| Capture 录制 | `rs_profiler_capture_recorder.h/.cpp` | `rosen/modules/render_service_profiler/` |
| PixelMap 处理 | `rs_profiler_pixelmap.h/.cpp` | `rosen/modules/render_service_profiler/` |
| 缓存 | `rs_profiler_cache.h/.cpp` | `rosen/modules/render_service_profiler/` |
| Dump | `rs_profiler_dump.cpp` | `rosen/modules/render_service_profiler/` |
| 日志 | `rs_profiler_log.h` | `rosen/modules/render_service_profiler/` |
| 工具 | `rs_profiler_utils.h/.cpp` | `rosen/modules/render_service_profiler/` |
| Beta 录制 | `rs_profiler_beta_recorder.cpp` | `rosen/modules/render_service_profiler/` |
| HRP 服务 | `rs_profiler_hrp_service.cpp` | `rosen/modules/render_service_profiler/` |
| 测试树 | `rs_profiler_test_tree.h/.cpp` | `rosen/modules/render_service_profiler/` |
| Trace3D API | `trace3d/api_core/trace3d_api_core.h` | `rosen/modules/render_service_profiler/trace3dapi/` |
| 单测 | `rs_profiler_test.cpp` 等 | `rosen/test/render_service/render_service_profiler/unittest/` |

## 核心模型

### 工作模式

Profiler 通过 `RS_PROFILER_ENABLED` 宏条件编译，未启用时所有 `RS_PROFILER_*` 宏为空操作或默认行为。

**Mode 枚举**：`NONE`(0)、`READ`(1)、`WRITE`(2)、`SAVING`(5)；**SubMode**：`NONE`(0)、`READ_EMUL`(1)、`WRITE_EMUL`(2)。

- **录制（WRITE）**：拦截 IPC OnRemoteRequest、Parcel 数据、节点树变化，写入归档文件。
- **回放（READ）**：从归档文件读取 Parcel 数据，替换实时数据，通过 PatchNodeId/PatchPid/PatchTime 映射 ID 和时间，驱动 RS 渲染管线回放。
- **保存（SAVING）**：录制结束后将数据写入文件。
- **Beta 录制**：后台持续录制，仅在特定条件触发时保留数据（前后各若干帧），支持文件分割和指标写入。

### 宏注入点

`RS_PROFILER_*` 宏在渲染管线关键位置注入，覆盖：

| 类别 | 宏示例 | 埋点位置 |
| --- | --- | --- |
| 帧生命周期 | `RS_PROFILER_ON_FRAME_BEGIN/END`、`ON_RENDER_BEGIN/END` | RSMainThread 帧循环 |
| IPC 拦截 | `RS_PROFILER_ON_REMOTE_REQUEST`、`ON_PARCEL_RECEIVE` | RenderService stub |
| Parcel 打补丁 | `RS_PROFILER_PATCH_NODE_ID/PID/TIME/COMMAND` | Transaction 反序列化 |
| 数据读写 | `RS_PROFILER_WRITE/READ_PARCEL_DATA`、`SKIP_PARCEL_DATA` | Ashmem 读写 |
| PixelMap | `RS_PROFILER_MARSHAL/UNMARSHAL/SKIP_PIXELMAP` | PixelMap 序列化 |
| 节点操作 | `RS_PROFILER_PROCESS_ADD_CHILD`、`SET_DIRTY_REGION` | 节点树操作 |
| 动画 | `RS_PROFILER_ANIME_SET_START_TIME`、`ANIMATION_DURATION_START/STOP` | 动画调度 |
| 指标 | `RS_PROFILER_RENDERNODE_INC/DEC`、`ADD_*_BLUR_METRICS` | 节点/模糊指标 |

### 网络命令接口

通过 `Socket` 类（Unix domain socket）与外部工具（如 DevEco 插件）通信。命令通过 `COMMANDS` 注册表分发，支持 60+ 命令，包括：

- 录制/回放控制：`RecordStart/Stop`、`PlaybackStart/Stop/Pause/Resume/SetSpeed`
- 节点操作：`DumpTree/Node/Properties/Modifiers`、`PatchNode`、`KillNode`、`BlinkNode`
- SKP 导出：`SaveSkp/OffscreenSkp/ComponentSkp`
- 设备信息：`GetDeviceInfo/Frequency`、`FixDeviceEnv`
- 性能分析：`GetPerfTree/CalcPerfNode`
- 系统参数：`SetSystemParameter/GetSystemParameter/DumpSystemParameters`

### 序列化/反序列化

节点树通过 `MarshalNodes/UnmarshalNodes` 进行序列化，支持全树和子树两种粒度。`ProfilerMarshallingJob` 管理异步序列化任务，跟踪未完成节点列表。首帧序列化通过 `FirstFrameMarshalling/Unmarshalling` 处理。

### 自定义指标

`RSProfilerCustomMetrics` 管理 26+ 指标，包括节点数（On-tree/Off-tree）、各类模糊操作计数和面积、动画节点类型和持续时间。指标每帧重置（manualReset 除外），通过 `RsMetricSet/GetList` 输出。

### 遥测

`RSTelemetry` 采集 CPU（16 核频率/负载、温度、电流、电压、RAM）、GPU（频率/负载）信息，通过 sysfs 读取。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 宏注入而非虚函数 | `RS_PROFILER_*` 宏在 `RS_PROFILER_ENABLED` 下展开为函数调用，否则为空 | 零开销抽象，Release 构建完全无性能影响 |
| Parcel 打补丁 | `PatchNodeId/Pid/Time` 替换实时 ID/时间 | 回放时需要映射录制时的 ID 到运行时 ID，保持节点树一致性 |
| Unix domain socket 通信 | `Socket` 类封装 `socketpair`/`accept`/`send`/`recv` | 本地工具连接，无需网络栈，安全可靠 |
| Beta 录制后台持续运行 | `EnableBetaRecord/Start/Stop/Update/Tick` 系列 | 仅在问题触发时保留数据，减少存储和性能开销 |
| 全树/子树两种序列化粒度 | `MarshalTree/SubTree` + `UnmarshalTree/SubTree` | 全树用于完整录制，子树用于增量更新和局部回放 |
| 指标体系可扩展 | `RSPROFILER_METRIC_ENUM` + `RSProfilerCustomMetricsParam` | 新增指标只需添加枚举和注册参数，无需修改采集逻辑 |
| 双标签 AGP + COMMERCIAL | trace 使用 `HITRACE_TAG_GRAPHIC_AGP` | 区分开发 trace 和商用 trace 采集 |
| 条件编译 RS_PROFILER_ENABLED | `#ifdef RS_PROFILER_ENABLED` 大段代码 | 产品可裁剪，非调试构建不包含 profiler 代码 |
| HRP 服务文件浏览 | `HrpServiceOpenFile/PopulateFiles` | 通过 HRP 服务加载设备上的录制文件，无需拷贝 |

## 待补充背景

- Profiler 在 DevEco Studio 中的集成方式和典型使用流程。
- Beta 录制的触发条件（dirty region 阈值、帧率异常等）和保留策略（前后各保留多少帧）。
- Trace3D 集成的具体使用场景和 3D 可视化效果。
- `ProfilerMarshallingJob` 的异步序列化在多线程环境下的调度和同步。
- 节点树序列化的文件格式版本（`fileVersion`）兼容性策略。
- SKP 导出与 Skia Debugger 的配合使用方式。
- 回放时动画时间校准（`AnimeSetStartTime`、`ReplayFixTrIndex`）的具体机制。
- `IsSecureScreen`/`IsPowerOffScreen` 在安全场景下的行为限制。
- 历史线上问题：录制文件过大、回放卡顿、内存泄漏等典型案例。
