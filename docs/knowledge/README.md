# 知识路由详细索引

本目录每个场景均有独立知识文档。
表中 `.md` 文件均位于 `docs/knowledge/`。
先按“领域缩略索引”确定小表，再读取对应行，避免通读全部路由。

## 领域缩略索引

| 触发词 | 先看领域 |
| --- | --- |
| build、GN、bundle、产品裁剪、preview、adapter | 构建/平台/接口 |
| main_thread、render_thread、drawable、modifier、animation | RS 管线/节点 |
| IPC、Parcel、Command、Stub、Proxy、fuzz、安全输入 | IPC/安全 |
| HWC、VSync、屏幕、dirty、HDR、UIFirst、layer、capture | RS 特性/设备 |
| LTPO、fps、DVSync、soft VSync、投票 | HGM |
| Canvas、Path、Bitmap、NDK Drawing、Texgine、WebGL | 2D/API/framework |
| profiler、trace、frame_report、ressched、graphic_test | 工具/测试 |

## 构建/平台/接口

| 场景 | 先读 | 锚点/搜索词 | 验证 |
| --- | --- | --- | --- |
| 构建/组件/裁剪 | `build-gn-bundle.md` | `BUILD.gn`, `bundle.json`, `*.gni`, `part_name` | target |
| 平台/预览适配 | `platform-adapter.md` | `adapter/`, `platform/`, `platform.gni` | 平台构建 |
| inner_api surface/composer | `inner-api-surface-composer.md` | `inner_api/`, `Surface/Composer` | inner API |
| NAPI/CJ/ANI/Taihe | `napi-cj-ani-taihe.md` | `interfaces/kits/`, `Napi`, `Ani`, `CJ` | 语言/XTS |
| Rust 基础绑定 | `rust-binding.md` | `render_service_base/rust/`, `lib.rs` | rust target |
| samples/演示工程 | `samples.md` | `rosen/samples/`, `hello_*` | sample |

## RS 管线/节点

| 场景 | 先读 | 锚点/搜索词 | 验证 |
| --- | --- | --- | --- |
| RS 进程入口 | `rs-process-entry.md` | `render_service/main/`, `core/system/`, `RenderService` | RS 单测 |
| RS 主线程/连接 | `rs-main-thread.md` | `render_service/core/pipeline/main_thread/`, `RSMainThread` | pipeline |
| RS 渲染线程 | `rs-render-thread.md` | `render_thread/`, `RSUniRender` | render_thread |
| 并行渲染/SLR | `rs-parallel-render.md` | `parallel_render/`, `slr_scale/`, `parallel` | pipeline |
| 客户端 UI 节点 | `rs-client-ui-node.md` | `render_service_client/core/ui/`, `RSNode`, `RSSurfaceNode` | UI/fuzz |
| 服务端 RenderNode | `rs-server-render-node.md` | `include/pipeline/`, `RSRenderNode` | pipeline |
| Drawable/绘制执行 | `rs-drawable.md` | `drawable/`, `Drawable` | drawable |
| Modifier/属性同步 | `rs-modifier.md` | `modifier*`, `Modifier` | modifier |
| Animation/窗口动画 | `rs-animation.md` | `animation/`, `window_animation/`, `RSAnimation` | animation |
| UI Effect/RS Effect | `rs-ui-effect.md` | `ui_effect/`, `effect/`, `Filter` | effect/fuzz |
| NGEffect 框架 | `rs-ng-effect-framework.md` | `include/effect/`, `ui_effect/property/`, `RSNGRender*`, `GEVisualEffect` | effect |
| RS ColorPicker 独立线程 | `rs-color-picker-thread.md` | `feature/color_picker/`, `RSColorPickerDrawable`, `RSColorPickerThread` | feature |
| 渲染资源/图片缓存 | `rs-render-resource.md` | `include/render/`, `RSImage`, `Cache` | render |

## IPC/安全

| 场景 | 先读 | 锚点/搜索词 | 验证 |
| --- | --- | --- | --- |
| RS IPC 接口 | `fuzzing.md` | `rs_interfaces.*`, `RSInterfaces` | IPC fuzz |
| Stub/Proxy/权限 | `fuzzing.md` | `render_server/transaction/`, `OnRemoteRequest` | stub fuzz |
| Transaction/Command | `fuzzing.md` | `command/`, `transaction/`, `RSCommand` | marshalling |
| IPC 回调/安全 | `fuzzing.md` | `ipc_callbacks/`, `ipc_security/`, `Callback` | fuzz |
| SAFuzz/畸形输入 | `fuzzing.md` | `safuzz/`, `test_case_config` | `safuzztest` |

## RS 特性/设备

| 场景 | 先读 | 锚点/搜索词 | 验证 |
| --- | --- | --- | --- |
| 硬件线程/HWC | `hwc-hardware-thread.md` | `hardware_thread/`, `HardwareThread` | HWC/设备 |
| HWC 预校验 | `hwc-prevalidate.md` | `hwc/`, `Prevalidate`, `Hpae` | HWC |
| Composer/HDI | `composer-hdi.md` | `render_service/composer/`, `composer/hdi_backend/`, `HdiBackend` | composer |
| VSync/native_vsync | `vsync-native-vsync.md` | `composer/vsync/`, `native_vsync/`, `VSync*` | vsync/fuzz |
| 屏幕/虚拟屏 | `screen-manager.md` | `screen_manager/`, `ScreenManager` | screen/fuzz |
| Dirty/遮挡 | `dirty-region.md` | `dirty/`, `dirty_region/`, `Occlusion` | dirty |
| UIFirst/OPINC/组渲染 | `uifirst-opinc.md` | `uifirst/`, `opinc/`, `render_group/` | 性能/图测 |
| layer/special/tunnel | `layer-special-tunnel.md` | `layer/`, `special_layer/`, `tunnel_layer/` | layer |
| overlay/多屏 | `overlay-multi-screen.md` | `overlay_display/`, `multi_screen/`, `Overlay` | 多屏/设备 |
| HDR | `hdr.md` | `hdr/`, `RSHdrUtil` | HDR/XTS |
| HGM/LTPO/DVSync | `hyper-graphic-manager.md` | `hyper_graphic_manager/`, `vrate/`, `Hgm*` | HGM |
| soft VSync/投票 | `hyper-graphic-manager.md` | `frame_rate_vote/`, `soft_vsync_manager/` | HGM/fuzz |
| chipset_vsync/帧稳定 | `chipset-vsync.md` | `chipset_vsync/`, `frame_stability/` | 性能 |
| HPAE/LPP/RDO | `hpae-lpp-rdo.md` | `hpae/`, `lpp/`, `rdo/` | 设备/性能 |
| capture/截图 | `capture.md` | `capture/`, `Capture` | capture/fuzz |
| DRM/水印/安全层 | `drm-watermark.md` | `drm/`, `watermark/`, `Watermark` | 安全/设备 |
| 鼠标/圆角/关键帧 | `pointer-round-corner.md` | `pointer_window_manager/`, `round_corner_display/` | 功能 |
| 后窗滤镜/跳帧 | `behind-window-filter.md` | `behind_window_filter/`, `dynamic_layer_skip/`, `Skip` | feature |
| buffer 回收/内存 | `buffer-reclaim.md` | `buffer_reclaim/`, `memory_info_manager/`, `Memory` | memory |
| 单帧/省电/VCLD | `single-frame-power.md` | `single_frame_composer/`, `power_off_render_skip/`, `vcld/` | feature |

## 2D/API/framework

| 场景 | 先读 | 锚点/搜索词 | 验证 |
| --- | --- | --- | --- |
| 2D Canvas/Paint/Path | `2d-canvas-paint-path.md` | `2d_graphics/include/draw/`, `Canvas`, `Path` | 2D/fuzz |
| 2D 图片/位图/Pixmap | `2d-image-bitmap-pixmap.md` | `include/image/`, `Bitmap`, `Pixmap` | image/fuzz |
| 2D 文本/字体 | `2d-text-font.md` | `2d_graphics/include/text/`, `frameworks/text/`, `Font` | text/fuzz |
| 2D effect/filter | `2d-effect-filter.md` | `2d_graphics/include/effect/`, `rosen/modules/effect/` | effect/fuzz |
| Recording/DrawCmd | `recording-drawcmd.md` | `recording/`, `CmdList`, `DrawCmd` | recording/fuzz |
| Drawing NDK | `drawing-ndk.md` | `2d_graphics/drawing_ndk/`, `OH_Drawing_*` | NDK/fuzz |
| RenderContext GL/VK | `render-context-gl-vk.md` | `render_context/`, `backend/`, `RenderContext` | backend |
| OpenGL/WebGL | `opengl-webgl.md` | `opengl_wrapper/`, `graphic/webgl/`, `EGL`, `GLES` | wrapper/fuzz |
| Vulkan/Trace3D | `vulkan-trace3d.md` | `vulkan_layers/`, `trace3dapi/`, `Trace3D` | vulkan |
| SurfaceImage | `surface-image.md` | `frameworks/surfaceimage/`, `NativeImage` | surfaceimage |
| PixelMapFromSurface | `pixelmap-from-surface.md` | `create_pixelmap_surface/`, `PixelMap` | pixelmap/fuzz |
| Text Framework | `text-framework.md` | `frameworks/text/service/`, `interface/mlb/`, `Texgine` | `text_test` |
| BootAnimation | `boot-animation.md` | `frameworks/bootanimation/`, `BootAnimation` | boot 测试 |
| effect_common/NDK | `effect-common-ndk.md` | `rosen/modules/effect/`, `Effect`, `ColorPicker` | effect |
| ColorSpace | `colorspace.md` | `utils/color_manager/`, `*color_manager/`, `ColorSpace` | color |
| DDGR/2D engine | `ddgr-2d-engine.md` | `2d_engine/ddgr/`, `DDGR` | DDGR |
| GLFW 跨平台上下文 | `glfw-render-context.md` | `glfw_render_context/`, `GLFW` | 跨平台 |

## 工具/测试

| 场景 | 先读 | 锚点/搜索词 | 验证 |
| --- | --- | --- | --- |
| Frame Analyzer/Load/Report | `frame-analyzer.md` | `frame_analyzer/`, `frame_load/`, `FrameReport` | frame |
| Render Frame Trace | `render-frame-trace.md` | `render_frame_trace/`, `scoped_bytrace/`, `Trace` | trace |
| Render Service Profiler | `render-service-profiler.md` | `render_service_profiler/`, `Profiler` | profiler |
| ResSched/资源调度 | `ressched.md` | `ressched/`, `ResSched` | ressched |
| aps/game service | `aps-game-service.md` | `aps_monitor/`, `gameservice_plugin/`, `Game` | 模块构建 |
| utils/log/socketpair | `utils-socketpair.md` | `utils/`, `rosen/modules/utils/`, `SocketPair` | utils/fuzz |
| graphic_test/DTK | `graphic-test-dtk.md` | `graphic_test/`, `rosen/test/dtk/` | 图形测试 |

## 术语别名

| 触发词 | 优先读取 |
| --- | --- |
| `LTPO`、`LTPS`、`fps`、frame rate、refresh rate、DVSync | `hyper-graphic-manager.md` |
| soft VSync、voting、刷新率拆分、刷新率投票 | `hyper-graphic-manager.md` |
| fuzz、sanitize、crash、Parcel、IPC、unmarshal | `fuzzing.md` |
| 越界、溢出、畸形输入、不可信输入 | `fuzzing.md` |

## 跨文档补读

| 当前场景 | 同时检查 |
| --- | --- |
| HGM 与 VSync/DVSync 交互 | `vsync-native-vsync.md` |
| HPAE、LPP、RDO、HWC 预校验 | `hwc-prevalidate.md`、`hpae-lpp-rdo.md` |
| HDR、颜色空间 | `hdr.md`、`colorspace.md` |
| Filter、Effect、HPAE 滤镜缓存、NGEffect、ColorPicker | `rs-ui-effect.md`、`2d-effect-filter.md`、`rs-ng-effect-framework.md`、`rs-color-picker-thread.md` |

## 知识文档沉淀规则

| 场景 | 需要新增或更新知识文档 |
| --- | --- |
| 无文档场景反复出现且已有稳定结论 | 新增 `docs/knowledge/<topic>.md` 并更新本表 |
| 改动跨客户端、服务端和硬件合成 | 补渲染管线或合成知识文档 |
| 新增公开 API、IPC 协议或安全输入面 | 补 API/IPC/fuzz 知识文档 |
| 调试步骤需要团队经验或设备结论 | 补排障知识文档，记录产品和镜像信息 |

Agent 不维护跨会话次数状态；
“反复出现”由当前任务上下文、仓内已有记录或人工反馈判断。
如果只是一次性问题，优先在最终回复记录结论，不强制新增知识文档。
需要跨会话跟踪时，把依据写入任务记录、PR 说明或新知识文档草稿，不依赖 Agent 记忆。
