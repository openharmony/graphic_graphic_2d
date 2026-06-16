# Inner API Surface 与 Composer

## 适用范围

改动涉及 interfaces/inner_api 中 Surface 和 Composer 内部接口、跨进程语义和 buffer 管理时，先读本文，再回到代码确认当前实现。

本文是背景知识和排查路线，不替代代码。修改前仍需读取对应头文件、实现文件和测试。

## 快速代码地图

| 方向 | 关键文件（完整路径） |
| --- | --- |
| Surface inner API 头文件 | `interfaces/inner_api/surface/native_image.h` |
| Surface 实现 | `frameworks/surfaceimage/` |
| Composer inner API 头文件 | `interfaces/inner_api/composer/vsync_receiver.h`、`native_vsync.h`、`vsync_type.h`、`vsync_iconnection_token.h` |
| VSync 实现 | `rosen/modules/composer/vsync/`、`rosen/modules/composer/native_vsync/` |
| Common inner API | `interfaces/inner_api/common/graphic_common.h`、`graphic_common_c.h` |
| Color Manager inner API | `interfaces/inner_api/color_manager/native_color_space_manager.h`、`color_manager_common.h` |
| HGM inner API | `interfaces/inner_api/hyper_graphic_manager/native_display_soloist.h` |
| Bootanimation inner API | `interfaces/inner_api/bootanimation/boot_animation_utils.h` |
| Utils 实现 | `utils/`（`libgraphic_utils`） |

## 核心模型

### Surface — OH_NativeImage

`interfaces/inner_api/surface/native_image.h` 定义了 NativeImage C API，对应 `libnative_image.so`，syscap 为 `SystemCapability.Graphic.Graphic2D.NativeImage`。

核心生命周期：

1. **创建**：`OH_NativeImage_Create(textureId, textureTarget)` 创建与 OpenGL ES 纹理关联的 NativeImage；`OH_ConsumerSurface_Create()` 创建纯消费者模式。
2. **绑定纹理**：`OH_NativeImage_AttachContext(image, textureId)` 将 image 附加到 GL 上下文，绑定 `GL_TEXTURE_EXTERNAL_OES`；`OH_NativeImage_DetachContext` 解绑。
3. **更新纹理**：`OH_NativeImage_UpdateSurfaceImage(image)` 用最新 buffer 更新 GL 纹理。此接口与 `AcquireNativeWindowBuffer` 互斥。
4. **获取 NativeWindow**：`OH_NativeImage_AcquireNativeWindow(image)` 返回关联的 OHNativeWindow，供生产者写入。
5. **查询**：`OH_NativeImage_GetTimestamp`、`OH_NativeImage_GetTransformMatrix`（deprecated，since 12 用 `GetTransformMatrixV2`）、`OH_NativeImage_GetSurfaceId`。
6. **帧可用回调**：`OH_NativeImage_SetOnFrameAvailableListener` 注册 `OH_OnFrameAvailableListener`。
7. **Buffer 消费**（since 12）：`OH_NativeImage_AcquireNativeWindowBuffer` + `OH_NativeImage_ReleaseNativeWindowBuffer` 直接操作 buffer，需注意引用计数管理。
8. **销毁**：`OH_NativeImage_Destroy`。

关键约束：
- `UpdateSurfaceImage` 与 `AcquireNativeWindowBuffer` 不能同时使用。
- `AcquireNativeWindowBuffer` 获取的 buffer 必须通过 `ReleaseNativeWindowBuffer` 归还，否则内存泄漏。
- fenceFd 使用后需手动关闭（Acquire 侧），Release 侧由系统关闭。
- since 13 新增 `OH_ConsumerSurface_SetDefaultUsage`、`OH_ConsumerSurface_SetDefaultSize`。
- since 22 新增单 buffer 模式（`OH_NativeImage_CreateWithSingleBufferMode`、`OH_ConsumerSurface_CreateWithSingleBufferMode`）和 `OH_NativeImage_ReleaseTextImage`。

### Composer — VSyncReceiver

`interfaces/inner_api/composer/vsync_receiver.h` 定义了 C++ 层 VSync 接收器，对应 `libcomposer.so`。

核心类型：

- **VSyncCallBackListener**：继承 `FileDescriptorListener`，通过 EventHandler 监听 fd 可读事件。内部维护 `VSyncCallback`/`VSyncCallbackWithId` 回调、period/timeStamp 缓存、fd 关闭标志。
- **VSyncReceiver**：继承 `RefBase`，封装 `IVSyncConnection` IPC 连接。条件编译 `__OHOS__` 宏区分设备端和预览端实现。

核心流程：

1. **初始化**：`Init(needAddFd=true)` 创建 socketpair fd，注册到 EventHandler。
2. **请求 VSync**：`RequestNextVSync(callback)` 请求下一帧信号，一帧内多次调用仅最后一次回调生效。`RequestNextVSyncWithMultiCallback` 允许多回调。
3. **持续回调**：`SetVSyncRate(callback, rate)` 按固定频率回调。
4. **查询**：`GetVSyncPeriod`、`GetVSyncPeriodAndLastTimeStamp`（支持线程共享版本）。
5. **DVSync**：`SetUiDvsyncSwitch`/`SetNativeDVSyncSwitch` 开启 DVSync 解耦 VSync，`SetUiDvsyncConfig` 配置参数。

### Composer — OH_NativeVSync

`interfaces/inner_api/composer/native_vsync.h` 定义了 C API，对应 `libnative_vsync.so`，syscap 为 `SystemCapability.Graphic.Graphic2D.NativeVsync`。

核心接口：

- `OH_NativeVSync_Create`：创建实例。
- `OH_NativeVSync_Create_ForAssociatedWindow`（since 14）：创建与指定窗口关联的实例。
- `OH_NativeVSync_RequestFrame`：请求下一帧，一帧内多次调用仅最后一次回调生效。
- `OH_NativeVSync_RequestFrameWithMultiCallback`（since 12）：多回调版本。
- `OH_NativeVSync_GetPeriod`：获取 VSync 周期。
- `OH_NativeVSync_DVSyncSwitch`（since 14）：开启/关闭 DVSync。
- `OH_NativeVSync_SetExpectedFrameRateRange`（since 20）：设置期望帧率范围（min/expected/max，0 <= min <= expected <= max <= 144，expected==0 表示取消投票）。

### VSync 类型定义

`vsync_type.h` 定义：

- `VSyncMode` 枚举：`VSYNC_MODE_LTPS`（固定刷新率）、`VSYNC_MODE_LTPO`（可变刷新率）。
- 刷新率常量：`VSYNC_MAX_REFRESHRATE_RANGE_MIN = 120`、`VSYNC_MAX_REFRESHRATE_RANGE_MAX = 432`、`VSYNC_144_HZ = 144`。

### VSyncIConnectionToken

`vsync_iconnection_token.h` 定义 VSync 连接 token 接口，继承 `IRemoteBroker`，用于 IPC 连接管理。`VSyncIConnectionTokenProxy` 为其 Proxy 实现。

### DisplaySoloist

`interfaces/inner_api/hyper_graphic_manager/native_display_soloist.h` 提供 DisplaySoloist C API，对应 `libnative_display_soloist.so`，syscap 为 `SystemCapability.Graphic.Graphic2D.HyperGraphicManager`。封装 VSync 回调 + 帧率范围设置，简化自绘制应用的帧调度。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| C API + C++ API 并存 | `native_image.h` 为 C API，`vsync_receiver.h` 为 C++ API | C API 面向 NDK/第三方应用，保证 ABI 稳定；C++ API 面向系统内部，可使用 RefBase/EventHandler 等 OHOS 基础设施 |
| UpdateSurfaceImage 与 AcquireNativeWindowBuffer 互斥 | `native_image.h` 中多处注释 "can not be used at the same time" | 同一 NativeImage 只能以一种消费模式运行，避免 buffer 竞争和状态混乱 |
| socketpair 传递 VSync | `VSyncReceiver` 使用 fd + `FileDescriptorListener` | VSync 信号通过 socketpair 从 composer 服务传递到客户端，利用 EventHandler 实现异步回调，避免阻塞 |
| __OHOS__ 条件编译 | `vsync_receiver.h` 中 `#ifdef __OHOS__` 区分完整版和精简版 | 预览环境不需要完整 IPC，提供纯虚基类；设备端使用完整 RefBase 实现 |
| DVSync 独立开关 | `SetUiDvsyncSwitch`/`SetNativeDVSyncSwitch` 分离 | UI 线程和 Native 线程的 DVSync 策略可能不同，独立控制避免相互干扰 |
| fenceFd 生命周期分离 | Acquire 时返回 fenceFd 需手动关闭，Release 时由系统关闭 | 符合 Android 同步 fence 惯例，消费者等 fence 完成后消费，生产者等 Release fence 完成后复用 buffer |

## 待补充背景

这些内容需要模块责任人后续补充，不能仅靠静态扫描完全确定：

- `OH_NativeImage` 内部 `SurfaceImage` 实现与 `graphic_surface` 仓 `Surface`/`BufferQueue` 的交互细节。
- VSyncReceiver 中 `IVSyncConnection` 的完整 IPC 接口定义（在 `graphic_surface` 仓中）。
- DVSync 的完整帧调度策略和与 HGM 的交互方式。
- `OH_NativeVSync_SetExpectedFrameRateRange` 投票机制与 HGM 帧率决策的交互。
- 单 buffer 模式（since 22）的使用场景和约束。
- `interfaces/inner_api/common/graphic_common.h` 中定义的错误码和公共类型。
- `OH_NativeImage_Release`（since 23）的完整语义和与 `Destroy` 的区别。
