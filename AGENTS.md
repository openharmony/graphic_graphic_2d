# graphic_graphic_2d 指引

## 路径基准

本仓对应 OpenHarmony `foundation/graphic/graphic_2d`。
本文中的 `docs/`、`rosen/`、`frameworks/`、`interfaces/` 均为本仓相对路径。
构建命令从 OpenHarmony 源码根目录执行；引用本仓 GN target 时使用
`//foundation/graphic/graphic_2d/...` 前缀。

## 知识路由与快速入口

当前 `docs/knowledge/` 每个场景均有独立知识文档。
表中 `.md` 文件均位于 `docs/knowledge/`。
先按“领域缩略索引”确定小表，再读取对应行，避免通读全部路由。

领域缩略索引：

| 触发词 | 先看领域 |
| --- | --- |
| build、GN、bundle、产品裁剪、preview、adapter | 构建/平台/接口 |
| main_thread、render_thread、drawable、modifier、animation | RS 管线/节点 |
| IPC、Parcel、Command、Stub、Proxy、fuzz、安全输入 | IPC/安全 |
| HWC、VSync、屏幕、dirty、HDR、UIFirst、layer、capture | RS 特性/设备 |
| LTPO、fps、DVSync、soft VSync、投票 | HGM |
| Canvas、Path、Bitmap、NDK Drawing、Texgine、WebGL | 2D/API/framework |
| profiler、trace、frame_report、ressched、graphic_test | 工具/测试 |

### 构建/平台/接口

| 场景 | 先读 | 锚点/搜索词 | 验证 |
| --- | --- | --- | --- |
| 构建/组件/裁剪 | `build-gn-bundle.md` | `BUILD.gn`, `bundle.json`, `*.gni`, `part_name` | target |
| 平台/预览适配 | `platform-adapter.md` | `adapter/`, `platform/`, `platform.gni` | 平台构建 |
| inner_api surface/composer | `inner-api-surface-composer.md` | `inner_api/`, `Surface/Composer` | inner API |
| NAPI/CJ/ANI/Taihe | `napi-cj-ani-taihe.md` | `interfaces/kits/`, `Napi`, `Ani`, `CJ` | 语言/XTS |
| Rust 基础绑定 | `rust-binding.md` | `render_service_base/rust/`, `lib.rs` | rust target |
| samples/演示工程 | `samples.md` | `rosen/samples/`, `hello_*` | sample |

### RS 管线/节点

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
| 渲染资源/图片缓存 | `rs-render-resource.md` | `include/render/`, `RSImage`, `Cache` | render |

### IPC/安全

| 场景 | 先读 | 锚点/搜索词 | 验证 |
| --- | --- | --- | --- |
| RS IPC 接口 | `fuzzing.md` | `rs_interfaces.*`, `RSInterfaces` | IPC fuzz |
| Stub/Proxy/权限 | `fuzzing.md` | `render_server/transaction/`, `OnRemoteRequest` | stub fuzz |
| Transaction/Command | `fuzzing.md` | `command/`, `transaction/`, `RSCommand` | marshalling |
| IPC 回调/安全 | `fuzzing.md` | `ipc_callbacks/`, `ipc_security/`, `Callback` | fuzz |
| SAFuzz/畸形输入 | `fuzzing.md` | `safuzz/`, `test_case_config` | `safuzztest` |

### RS 特性/设备

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
| HDR/TV/图像增强 | `hdr-tv-enhance.md` | `hdr/`, `tv_metadata/`, `image_enhance/` | HDR/XTS |
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

### 2D/API/framework

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
| Color/HDR capability | `color-hdr-capability.md` | `utils/color_manager/`, `*color_manager/`, `ColorSpace` | color |
| DDGR/2D engine | `ddgr-2d-engine.md` | `2d_engine/ddgr/`, `DDGR` | DDGR |
| GLFW 跨平台上下文 | `glfw-render-context.md` | `glfw_render_context/`, `GLFW` | 跨平台 |

### 工具/测试

| 场景 | 先读 | 锚点/搜索词 | 验证 |
| --- | --- | --- | --- |
| Frame Analyzer/Load/Report | `frame-analyzer.md` | `frame_analyzer/`, `frame_load/`, `FrameReport` | frame |
| Render Frame Trace | `render-frame-trace.md` | `render_frame_trace/`, `scoped_bytrace/`, `Trace` | trace |
| Render Service Profiler | `render-service-profiler.md` | `render_service_profiler/`, `Profiler` | profiler |
| ResSched/资源调度 | `ressched.md` | `ressched/`, `ResSched` | ressched |
| aps/game service | `aps-game-service.md` | `aps_monitor/`, `gameservice_plugin/`, `Game` | 模块构建 |
| utils/log/socketpair | `utils-socketpair.md` | `utils/`, `rosen/modules/utils/`, `SocketPair` | utils/fuzz |
| graphic_test/DTK | `graphic-test-dtk.md` | `graphic_test/`, `rosen/test/dtk/` | 图形测试 |

术语别名：

| 触发词 | 优先读取 |
| --- | --- |
| `LTPO`、`LTPS`、`fps`、frame rate、refresh rate、DVSync | `docs/knowledge/hyper-graphic-manager.md` |
| soft VSync、voting、刷新率拆分、刷新率投票 | `docs/knowledge/hyper-graphic-manager.md` |
| fuzz、sanitize、crash、Parcel、IPC、unmarshal | `docs/knowledge/fuzzing.md` |
| 越界、溢出、畸形输入、不可信输入 | `docs/knowledge/fuzzing.md` |

跨文档补读：

| 当前场景 | 同时检查 |
| --- | --- |
| HGM 与 VSync/DVSync 交互 | `docs/knowledge/vsync-native-vsync.md` |
| HPAE、LPP、RDO、HWC 预校验 | `docs/knowledge/hwc-prevalidate.md`、`docs/knowledge/hpae-lpp-rdo.md` |
| HDR、颜色空间、图像增强 | `docs/knowledge/hdr-tv-enhance.md`、`docs/knowledge/color-hdr-capability.md` |
| Filter、Effect、HPAE 滤镜缓存 | `docs/knowledge/rs-ui-effect.md`、`docs/knowledge/2d-effect-filter.md` |

知识文档沉淀规则：

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

## 项目定位

本仓提供 Rosen 渲染框架、2D 绘制能力、显示刷新率管理和图形栈基础能力。
优先按这些目录定位问题：

- `rosen/modules/render_service_base/`：IPC 接口、节点/动画/属性基础类型、transaction。
- `rosen/modules/render_service/`：服务端渲染、主线程、渲染线程、硬件线程、合成。
- `rosen/modules/render_service_client/`：客户端 API、节点树、动画和 modifier 系统。
- `rosen/modules/2d_graphics/`：`Canvas`、`Paint`、`Path`、文本、图片和后端适配。
- `rosen/modules/hyper_graphic_manager/`：刷新率策略、投票、LTPO/LTPS、软 VSync。
- `frameworks/`：boot animation、text、OpenGL wrapper、Vulkan layers、surface image。
- `interfaces/`：`inner_api`、NAPI、CJ、ANI、Taihe、NDK/C API。
- `rosen/test/`、`test/`、`tools/`、`interfaces/*/test/`：单测、fuzz 和工具。

## 典型工作流

1. 先判断改动场景，按“知识路由与快速入口”读取文档、代码锚点和测试锚点。
2. 定位公开接口和内部实现边界；涉及 API 先看 `interfaces/`，再看实现和测试。
3. 涉及渲染链路时，按客户端节点树、transaction/IPC、服务端节点树、drawable、
   渲染线程、GPU/HWC 合成的顺序追踪数据流。
4. 小步修改，就近复用已有日志、错误码、智能指针、锁、线程投递和测试夹具。
5. 先判断是否可定位 OpenHarmony 源码根；单仓环境取消编译环节，只做可执行静态验证。
6. 完整源码环境按验证矩阵选择最近构建或测试目标；
   涉及真实显示效果时补充设备验证结果。
7. 按“完成定义”输出结果。
8. 提交和 push 前按“提交和推送”要求完成检查。

## 常见任务模板

新增 RSNode 属性：

- 实现：从客户端节点或 modifier 定义入手，确认默认值、动画语义和线程边界。
- 同步：检查 `render_service_client`、`render_service_base`、服务端 render node 或 drawable。
- 验证：跑最近 command/marshalling/pipeline 单测；确认旧 transaction 数据有默认兼容行为。
- 关注：`RSRenderNode`、`RSSurfaceNode`、`RSCanvasNode`、`RSRenderModifier`。

新增 RSInterface IPC 接口：

- 实现：先改接口声明和 proxy/stub，再检查权限、参数校验、错误码和日志。
- Parcel：字段顺序必须与 proxy 写入顺序一致，读取失败要有明确返回和日志。
- 验证：补 `rsinterfaces_*` 客户端 fuzzer、SAFuzz 配置和 `rsrenderservice*stub` fuzzer。
- 锚点：`rs_interfaces.h`、`rs_client_to_service_connection_proxy.cpp`、`rs_render_service_stub.cpp`。

新增动画类型：

- 实现：确认客户端 animation、modifier、transaction 和服务端属性更新路径。
- 边界：覆盖 start/end/cancel、重复播放、空节点、跨线程销毁和默认值行为。
- 验证：跑最近 animation、render_service_client 或 render_service_base animation 单测。

新增 NAPI、NDK、CJ、ANI 或 Taihe 公开 API：

- 实现：确认 `interfaces/` 声明、错误码映射、默认值、权限和 XTS 预期。
- 同步：不要只改一层语言绑定改变公开行为；同步检查所有相关语言入口。
- 验证：跑对应语言单测；涉及公开行为时验证或说明 XTS 目标。
- 门禁：改 ABI、枚举、结构体字段或错误码前必须人工确认兼容策略。

新增 `BUILD.gn` target：

- 实现：复用附近 `part_name`、`subsystem_name`、`module_out_path`、deps 和 external_deps。
- 发现：用 `rg -n 'group\\(\"test|group\\(\"unittest|ohos_unittest|ohos_fuzztest' <dir>`。
- 验证：从 OpenHarmony 根执行 `ninja -C out/<product-name> <target>`。
- 聚合：确认 target 被上层 `test`、`unittest`、`fuzztest` group 或 `bundle.json` test 列表引用。

## 构建和验证

构建命令从 OpenHarmony 源码根目录执行，不在本仓子目录直接执行。
产品、out 目录和可用 target 以当前工程配置为准。

先判断仓库形态：

- 若无法在当前路径或父级定位 OpenHarmony 根目录的 `build.sh` 和 `prebuilts/build-tools/`，
  视为单仓环境，取消编译环节；不要伪造构建结论。
- `out/` 只代表已有产品构建输出；干净源码树可能没有 `out/`，
  不能仅凭缺少 `out/` 判定为单仓。
- 单仓环境可自主执行 `git diff --check`、`rg` 路径/符号核对、JSON/YAML 语法检查、
  BUILD target 引用检查和头文件依赖检索。
- 若用户、CI 或远程构建机提供明确命令，按其命令验证；
  未提供时在最终回复记录构建缺口。

```sh
ls out/
./build.sh --product-name <product-name> --build-target graphic_2d --ccache
prebuilts/build-tools/linux-x86/bin/ninja -C out/<product-name> <target>
```

`rk3568` 是常见示例产品，不是默认值。
实际产品名参考 `out/` 已有目录、当前任务说明或 CI 配置。

增量构建选择中，`$G2D` 表示 `//foundation/graphic/graphic_2d`。
执行前必须替换为完整前缀；不要直接复制 `:lib...` 这类仓内短写法。

| 改动范围 | 优先 target |
| --- | --- |
| 只改文档 | 不构建；跑链接、路径和 `git diff --check` |
| HGM | `$G2D/rosen/modules/hyper_graphic_manager:libhyper_graphic_manager` |
| HGM 测试 | `$G2D/rosen/test/hyper_graphic_manager/unittest:unittest` |
| Render Service 服务端 | `$G2D/rosen/modules/render_service:librender_service` |
| Render Service 测试 | `$G2D/rosen/test/render_service:test` |
| Render Service 客户端 | `$G2D/rosen/modules/render_service_client:librender_service_client` |
| 2D 绘制 | `$G2D/rosen/modules/2d_graphics:2d_graphics` |
| 2D 测试 | `$G2D/rosen/test/2d_graphics:test` |
| IPC 或安全输入 | 最近 stub/proxy fuzz；SAFuzz 用 `$G2D/rosen/modules/safuzz:safuzztest` |
| 多语言 API | 对应 NAPI/NDK/CJ/ANI/Taihe 单测和 XTS |

运行单测时，先在 `BUILD.gn` 中确认 `module_out_path` 和 target 名称。
HGM 示例 target `hgm_frame_voter_test` 的 `module_out_path` 是
`graphic_2d/graphic_2d/hyper_graphic_manager`。

```sh
prebuilts/build-tools/linux-x86/bin/ninja -C out/<product-name> \
  //foundation/graphic/graphic_2d/rosen/test/hyper_graphic_manager/unittest:hgm_frame_voter_test
find out/<product-name>/tests/unittest -name '*hgm_frame_voter_test*'
out/<product-name>/tests/unittest/<path>/hgm_frame_voter_test --gtest_filter='*Vote*'
```

加速和排查：

- 优先构建最近 target；确认 target 存在后再扩大到 `graphic_2d` 或 `graphic_common_test`。
- `--ccache` 只用于 `build.sh`；`ninja -C out/<product-name> <target>` 用于已有 out 的增量构建。
- 构建失败先记录首个真实编译错误，不要被后续级联错误带偏。

macOS 说明：

- 当前本地环境可能是 Darwin；完整 OpenHarmony 构建通常依赖 Linux 构建链。
- macOS 本地适合做文档检查、`rg` 检索、`git diff --check` 和纯脚本校验。
- 需要编译、设备测试或 XTS 时，使用团队 CI、Linux 远程构建机或明确产品构建环境。

## 性能约束

帧预算参考：

| 刷新率 | 单帧预算 |
| --- | --- |
| 60Hz | 16.67ms |
| 90Hz | 11.11ms |
| 120Hz | 8.33ms |

默认复核阈值：

- 同场景 3 次采样，P95 帧耗时或内存峰值上升超过 3%，需要人工确认。
- 任一新增同步等待、无界循环、全量扫描或 per-frame 堆分配，都需要说明必要性和基线。
- O(N) 逻辑必须说明 N 是节点数、surface 数还是屏幕数，并记录典型规模。
- 基线记录写入 PR `Test & Result`：产品、镜像、场景、工具、median、P95、max。

工具和目标：

- 渲染管线优先用 hilog + hitrace/perfetto 对比修改前后；设备命令差异先查 `--help`。
- 2D benchmark 代码在 `rosen/samples/2d_graphics/benchmarks/`。
- 2D benchmark 编进 `$G2D/rosen/samples/2d_graphics:drawing_engine_sample`。
- benchmark 参数见 `benchmarks/benchmark_config.cpp`：`singlethread`、`multithread`、`api`、`dcl`。

最小采样方法：

- 同产品、同镜像、同场景采集修改前后各 3 轮；每轮至少 300 帧或 10 秒。
- 丢弃启动和预热阶段的前 30 帧；记录 median、P95、max 和内存峰值。
- 优先使用 `frame_analyzer/`、`frame_report/` 或性能平台输出；没有工具输出时保留原始 trace。
- 只有同工具、同场景、同统计口径下，P95 上升超过 3% 才作为性能回退信号。

常用设备采集示例：

```sh
hdc shell "hilog -w start -f /data/local/tmp/graphic.log"
hdc shell "hitrace --trace_begin graphic sched freq idle"
# 手动或脚本触发待测场景，保持至少 300 帧或 10 秒。
hdc shell "hitrace --trace_finish > /data/local/tmp/graphic.ftrace"
hdc shell "hilog -w stop"
hdc file recv /data/local/tmp/graphic.log ./graphic.log
hdc file recv /data/local/tmp/graphic.ftrace ./graphic.ftrace
```

2D benchmark 构建示例：

```sh
prebuilts/build-tools/linux-x86/bin/ninja -C out/<product-name> \
  $G2D/rosen/samples/2d_graphics:drawing_engine_sample
```

## 依赖和接口边界

常见跨子系统边界包括：

- 上游图形和硬件：Skia、OpenGL、Vulkan、EGL、HDI、HWC、`drivers_interface_display`。
- 上游运行时和系统能力：IPC、FFRT、event handler、system ability、init、hitrace、hilog。
- Buffer 和显示：graphic surface、SurfaceBuffer、NativeBuffer、fence、screen manager。
- 下游消费方：ACE/UI 框架、窗口管理、多语言 API 调用方、应用侧 NDK/NAPI 用户。
- 平台和产品裁剪：`graphic_config.gni`、产品形态、feature flag、vendor extension。

改动触达依赖接口、枚举、buffer 语义、错误码、线程模型或能力查询时，
不要只在本仓闭环。
需要检查依赖方公开头文件、调用方假设和运行时能力，并人工确认通知范围和评审人。

## 项目约束

不要做：

- 不要绕过既有客户端、服务端和渲染端边界直接访问对侧对象。
- 不要只改某一层语言绑定改变公开行为；同步检查 JS、NDK、CJ、ANI、Taihe 和 Native 入口。
- 不要把缺失真实设备验证的刷新率、HWC、GPU、surface、HDR 或显示效果改动描述为
  完整验证。
- 没有设备不默认阻塞文档、知识路由、静态验证或可本地验证的小修；
  阻塞的是“已完成设备行为验证”这类结论。
- 不要修改公开 API/ABI、枚举值、结构体字段、错误码或默认行为而不说明兼容性影响。
- 不要修改生成代码、OAT 扫描豁免对应产物或第三方依赖副本，除非任务明确要求。
- 不要把 OpenHarmony 源码树 `third_party/` 下的 Skia、EGL 等外部依赖当成本仓文件修改。
- 不要执行破坏性 git/文件操作或大范围机械重构，除非用户明确要求。

必须人工确认：

- 改公开 API/ABI、错误码、默认值、权限、XTS 预期或跨语言接口前。
- 改显示硬件接口、HWC、SurfaceBuffer、NativeBuffer、fence、刷新率模式或产品裁剪策略前。
- 改 Render Service 跨进程 IPC、transaction 编码、Parcel/TLV 格式或旧数据兼容前。
- 改第三方库、license、编译宏、feature flag、vendor extension 或跨仓接口前。
- 需要真实设备验证但当前没有设备，且任务涉及设备行为、显示效果或性能功耗结论时。
  如果只是继续推进实现或文档，可以先记录设备验证缺口，等待人工或 CI/设备补测。

Agent 自主边界：

- “5 个文件”是同一用户任务或同一会话累计阈值，不是单个 commit 阈值；
  不得拆提交绕过。
- 可自主完成：文档、知识路由、注释、局部测试补充和单模块小修。
- 可自主推进但需说明风险：同一模块累计不超过 5 个文件，行为边界清晰且可验证。
- 单仓或缺少 OpenHarmony 根环境时，不因无法编译而默认阻塞；
  编译环节按“构建和验证”取消。
- 单仓可自主完成的静态验证包括 `git diff --check`、`rg` 路径/符号核对、JSON/YAML 校验、
  BUILD 引用检查和头文件依赖检索。
- 若远程 CI 或构建机入口已给出，按该入口验证并记录结果；未给出时不要臆造 CI 结论。
- 跨模块或跨仓变化、设备行为、性能功耗结论、公开 API/ABI 变化，必须先确认。

## 代码风格

优先使用仓库根目录 `.clang-format`。其当前 C++ 配置包含 `ColumnLimit: 120`、
`IndentWidth: 4`、`UseTab: Never` 和 include regroup/sort 规则。
NBNC、魔法数字等规则以本指引、评审和团队工具共同约束。

- 单行不得超过 120 字符；长字符串、函数调用和表格行都要拆分或压缩。
- 单个函数不超过 50 NBNC 行；NBNC 指非空白、非注释行。
- 除 0、1 和必要的 -1 错误返回外，不直接写魔法数字；使用 `constexpr` 或具名常量。
- 命名：类名使用项目既有 PascalCase 风格；其它命名复用附近文件风格。
- 成员变量：新代码优先使用尾部 `_`；遇到历史 `m` 前缀时保持局部一致，不做批量改名。
- 头文件：按 `.clang-format` 排序；能 forward declaration 的地方不要引入重依赖。
- 智能指针：IPC/RefBase 对象常用 `sptr<>`/`wptr<>`；内部所有权使用标准智能指针。
- 错误码：复用附近 `SUCCESS`、`ERR_*`、`HgmErrCode` 或模块既有返回约定。
- 线程异步：FFRT task、EventHandler 投递和回调生命周期必须说明线程归属，避免悬空捕获。
- 注释：只解释不显然的约束、生命周期、并发原因或兼容性原因，不写空泛描述。

## 提交和推送

所有提交在 push 之前，必须完成 `stability-code-review` 检视，并确认没有遗留问题。
`stability-code-review` 是团队外部检查工具/skill，不是 Agent 内置能力；
当前环境不可用时，先执行：

```sh
npm i @ohos-graphics/stability-code-review
```

安装后按工具说明执行检视。如果安装或执行失败，不要继续 push；
需要在回复或 PR 说明中记录失败原因，并等待人工确认。
以下为 Agent 提交约定，可能与历史人工提交风格不同；人工提交按团队现有规范执行。
提交建议使用 `git commit -s` 自动生成 `Signed-off-by`，
其姓名和邮箱来自 `git config user.name` 与 `git config user.email`，
格式类似 `Signed-off-by: zhangsan <zhangsan@huawei.com>`。
同时在 commit message 末尾额外空一行写入 `Co-Authored-By: Agent`：

```text
<type>(<scope>): <summary>

<body，可选>

Signed-off-by: <name> <email>

Co-Authored-By: Agent
```

没有明确项目要求时，`type` 优先使用 `fix`、`feat`、`refactor`、`test`、`docs`、`build`，
`scope` 使用模块名或目录名。
若关联 issue、缺陷单或需求单，在 body 中写清编号和影响范围。

## 完成定义

Agent 最终回复必须包含：

- 读取过的知识文档和对应场景。
- 修改的文件、行为影响面和明确未修改的关键文件。
- 已执行的构建、单测、fuzz、XTS 或真实设备验证命令；未执行时说明原因。
- XTS 目标或真实设备验证无法确认时，列出缺口和需要人工确认的问题。
- 本次自主边界级别：自主完成、自主推进但说明风险、必须先确认；
  同时说明累计改动文件数。
- 当前是否为单仓或完整 OpenHarmony 源码根；
  若取消编译环节，说明取消原因和替代静态检查。
- 若涉及提交或 push，说明 `stability-code-review` 结果和 commit message 是否符合提交约定。
