# Utils、Log 与 SocketPair

## 适用范围

- 本地 Socket 通信（LocalSocketPair）
- RS Trace 宏定义与 HiTrace 集成
- 作用域 trace 封装（ScopedBytrace、ScopedDebugTrace）
- 帧调度扩展接口（RsFrameReportExt）
- 图形错误码定义（GraphicsErrorCode）
- 字符串工具（StringUtils）
- 沙箱工具（sandbox_utils）

## 快速代码地图

| 模块 | 关键文件 | 相对路径 |
| --- | --- | --- |
| SocketPair 本地通信 | `local_socketpair.h` | `utils/socketpair/export/` |
| SocketPair 实现 | `local_socketpair.cpp` | `utils/socketpair/src/` |
| RS Trace 宏 | `rs_trace.h` | `utils/log/` |
| RS Trace 跨平台 | `rs_trace_crossplatform.h` | `utils/log/` |
| ScopedBytrace | `scoped_bytrace.h/.cpp` | `utils/scoped_bytrace/export/`、`src/` |
| 帧调度扩展 | `rs_frame_report_ext.h` | `utils/rs_frame_report_ext/include/` |
| 错误码 | `error_code.h` | `rosen/modules/utils/` |
| 字符串工具 | `string_utils.h` | `rosen/modules/utils/` |
| 沙箱工具 | sandbox_utils 目录 | `utils/sandbox/` |
| SocketPair 消费方 | `vsync_distributor.h` | `rosen/modules/composer/vsync/include/` |

## 核心模型

### LocalSocketPair：本地 Socket 通信

LocalSocketPair 继承自 RefBase，封装 `socketpair(AF_UNIX, SOCK_STREAM)` 创建本地双端通信通道。

**核心接口**：

| 方法 | 功能 |
| --- | --- |
| `CreateChannel(sendSize, receiveSize)` | 创建 socketpair 并设置缓冲区大小 |
| `GetSendDataFd()` | 获取发送端 fd |
| `GetReceiveDataFd()` | 获取接收端 fd |
| `SendToBinder(data)` | 将发送端 fd 通过 Binder 传递给对端进程 |
| `ReceiveToBinder(data)` | 将接收端 fd 通过 Binder 传递给对端进程 |
| `SendData(vaddr, size)` | 发送数据 |
| `ReceiveData(vaddr, size)` | 接收数据 |
| `CloseReceiveDataFd()` | 关闭接收端 fd |

**使用场景**：VSync 分发器（`VSyncDistributor`）通过 `LocalSocketPair` 在 RS 进程和 App 进程之间建立低延迟通信通道，用于 VSync 信号传递。fd 通过 Binder 传递，数据通过 socket 读写。

**缓冲区配置**：`SetSockopt` 通过 `setsockopt(SO_SNDBUF/SO_RCVBUF)` 设置发送/接收缓冲区大小，确保高帧率下不丢数据。

### RS Trace 体系（rs_trace.h）

统一使用 `HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL` 双标签。详细宏定义见 render-frame-trace.md。

关键特性：
- **分级 trace**：DEBUG / INFO / CRITICAL / COMMERCIAL 四级
- **USER 变体**：带 `[screen_id=X]` 前缀的多屏 trace
- **条件编译**：`ROSEN_TRACE_DISABLE` 下全部为空操作

### ScopedBytrace：作用域 trace 封装

RAII 封装 `StartTrace`/`FinishTrace`。构造调用 `StartTrace(HITRACE_TAG_GRAPHIC_AGP, proc)`，析构自动 `FinishTrace`。支持提前 `End()` 手动结束。

**ScopedDebugTrace**：受 `persist.sys.graphic.openDebugTrace` 系统参数控制，默认关闭，仅开发阶段按需开启。

### RsFrameReportExt：帧调度扩展

通过 dlopen 加载 `libframe_aware_sched.z.so`，获取四个函数指针：

| 函数指针 | 功能 |
| --- | --- |
| `frameGetEnableFunc_` | 查询帧调度是否启用 |
| `initFunc_` | 初始化帧调度 |
| `handleSwapBufferFunc_` | SwapBuffer 回调 |
| `requestNextVSyncFunc_` | 请求下一帧 VSync |
| `receiveVSyncFunc_` | 接收 VSync 回调 |

懒加载：首次调用 `Init()` 时 dlopen，加载失败 `frameSchedSoLoaded_ = false`，后续调用直接返回。

### GraphicsErrorCode

定义图形子系统错误码范围：

| 范围 | 子系统 |
| --- | --- |
| 1-100 | Render Service |
| 101-200 | Animation |
| 201-300 | 2D Graphics（`ERROR_2D_GRAPHICS_OBJ_CREATE_FAILED = 201`、`ERROR_2D_GRAPHICS_DECODE_FILE_INVALID = 202`） |
| 301-400 | Effect |
| 401-500 | Backstore |
| 501-600 | Composer |
| 601-700 | WMS |

### StringUtils

- `AppendFormat(out, fmt, args...)`：安全格式化追加字符串，缓冲区 4096 字节
- `SplitString(src, dest, separator)`：按分隔符拆分字符串到容器
- `MergeToString(srcVec)`：将 vector 合并为逗号分隔字符串

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| SocketPair 用 AF_UNIX SOCK_STREAM | `socketpair()` 创建本地流式 socket | 本地进程间零拷贝通信，比 Binder 更低延迟 |
| fd 通过 Binder 传递 | `SendToBinder`/`ReceiveToBinder` 封装 `WriteFileDescriptor` | App 进程需要获得 socket fd，通过 Binder 传递 fd 是标准方式 |
| 缓冲区大小可配置 | `CreateChannel(sendSize, receiveSize)` 参数化 | 不同刷新率需要不同缓冲区大小，避免数据丢失 |
| RsFrameReportExt dlopen 懒加载 | `LoadLibrary()` 在首次 `Init()` 时调用 | frame_aware_sched 可能不存在（裁剪产品），懒加载避免启动依赖 |
| 错误码分段定义 | `GraphicsErrorCode` 枚举 1-700 分段 | 不同子系统独立使用错误码范围，避免冲突 |
| StringUtils 使用 snprintf_s | `::snprintf_s(buf, ...)` 安全函数 | 防止缓冲区溢出 |
| ScopedDebugTrace 系统参数控制 | `persist.sys.graphic.openDebugTrace` | 开发 trace 默认关闭，避免 Release 版性能开销 |

## 待补充背景

- LocalSocketPair 在 VSync 分发中的完整数据流和性能特征（延迟、吞吐量）。
- `CloseReceiveDataFd()` 的调用时机（是否在 VSync 连接断开时调用）。
- RsFrameReportExt 加载的 `libframe_aware_sched.z.so` 的版本和兼容性。
- `handleSwapBufferFunc_` 和 `receiveVSyncFunc_` 在 RS 渲染管线中的调用位置。
- `persist.sys.graphic.openDebugTrace` 系统参数的典型设置命令和影响范围。
- `GraphicsErrorCode` 各范围的实际使用情况（哪些错误码已被使用）。
- SocketPair 的安全考虑（是否需要 SELinux 策略、fd 泄漏防护）。
- 历史线上问题：socket 缓冲区不足导致 VSync 丢失、fd 泄漏等案例。
