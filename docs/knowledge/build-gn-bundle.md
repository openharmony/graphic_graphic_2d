# 构建、组件与裁剪

## 适用范围

改动涉及 BUILD.gn 构建配置、bundle.json 部件声明、gni 配置、part_name 组件归属和产品裁剪时，先读本文，再回到代码确认当前实现。

本文是背景知识和排查路线，不替代代码。修改前仍需读取对应头文件、实现文件和测试。

## 快速代码地图

| 方向 | 关键文件（完整路径） |
| --- | --- |
| 仓级构建入口 | `BUILD.gn`（仓库根） |
| 部件声明 | `bundle.json`（仓库根） |
| 全局特性开关 | `graphic_config.gni`（仓库根） |
| 平台发现 | `ace_platforms.gni`（仓库根） |
| OHOS 平台配置 | `adapter/ohos/build/platform.gni`、`config.gni`、`config_ng.gni`、`common.gni` |
| Preview 平台配置 | `adapter/preview/build/platform.gni`、`config.gni`、`config_windows.gni`、`config_mac.gni`、`config_linux.gni` |
| 2D 绘制构建 | `rosen/modules/2d_graphics/BUILD.gn` |
| RS Base 构建 | `rosen/modules/render_service_base/BUILD.gn` |
| RS 平台适配 | `rosen/modules/render_service_base/src/platform/BUILD.gn` |
| 通用平台依赖 | `rosen/modules/platform/config.gni`、`rosen/modules/platform/BUILD.gn` |
| 文本配置 | `frameworks/text/config.gni` |

## 核心模型

### 构建分组体系

`bundle.json` 中 `build.group_type` 将本仓产物分为三组：

- **base_group**：基础能力，包括 NAPI/CJ/ANI/Taihe 语言绑定、2D 绘制、Composer/VSync、OpenGL wrapper、Vulkan layer、HGM、DDGR 等。所有产品必须包含。
- **fwk_group**：框架层，包括 `librender_service_base`、`librender_service_proxy`、`librender_service_client`。供应用进程使用。
- **service_group**：服务端，包括 `librender_service`、`render_service`（SA）、`render_process`、init 配置和 graphic.rc。仅设备端部署。

### part_name 与 subsystem_name

本仓所有 target 统一使用：
- `part_name = "graphic_2d"`
- `subsystem_name = "graphic"`

在新增 BUILD.gn target 时，必须复用附近文件的 `part_name` 和 `subsystem_name`，不能引入新的 part 或 subsystem。

### 特性开关体系

`graphic_config.gni` 通过 `declare_args()` 声明约 40 个 `graphic_2d_feature_*` 特性开关。关键机制：

1. **产品形态裁剪**：`graphic_2d_feature_product` 可取 `"phone"`、`"pc"`、`"tablet"`、`"wearable"` 等值。根据该值自动开启 Vulkan、OPINC、stack culling 等特性。例如 phone/pc/tablet/wearable 默认开启 Vulkan 和 OPINC。
2. **条件编译宏**：特性开关转换为 `gpu_defines` 列表中的编译宏（如 `RS_ENABLE_VK`、`RS_ENABLE_UNI_RENDER`、`RS_ENABLE_PARALLEL_RENDER`）。各模块 BUILD.gn 中 `defines += gpu_defines` 引入。
3. **外部扩展检测**：通过 `path_exists()` 或 `exec_script()` 检测 `graphic_2d_ext` 仓是否存在，动态导入 DDGR、Delegator、Broker、HGM、FrameStability、ModifiersDraw 等扩展配置。
4. **系统组件感知**：通过 `global_parts_info` 检测 accessibility、player_framework、video_processing_engine、hdf_drivers_interface_display、memmgr 等组件是否已安装，自动裁剪对应功能。

### 平台发现流程

1. `ace_platforms.gni` 遍历 `adapters` 列表（由 `graphic_config.gni` 动态发现或硬编码为 `["preview", "ohos"]`）。
2. 对每个 adapter 导入 `adapter/$item/build/platform.gni`，获取 `platforms` 列表。
3. OHOS adapter 根据标准系统条件注册 `ohos` 和 `ohos_ng` 平台；Preview adapter 根据 `use_mingw_win`/`use_mac`/`use_linux` 注册 windows/mac/linux 平台。
4. 最终 `ace_platforms` 列表驱动 2D 绘制等模块的 `graphics2d_source_set` 模板实例化，为每个平台生成独立的 source set。

### 测试聚合

根 BUILD.gn 中 `graphic_common_test` group 聚合了主要测试 target。`bundle.json` 的 `build.test` 列表额外包含 `render_service_client:test`、`safuzz:safuzztest`、`2d_graphics:test`、`dtk` 等。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 三层构建分组 | `bundle.json` 中 `base_group`/`fwk_group`/`service_group` | 区分部署范围：基础能力全量部署，框架仅在应用进程加载，服务端仅在系统服务进程部署 |
| 统一 part_name | 所有 BUILD.gn 中 `part_name = "graphic_2d"` | 确保 graphic_2d 部件内所有产物归属一致，支持部件级裁剪 |
| 特性开关 + gpu_defines | `graphic_config.gni` 中 `declare_args()` → `gpu_defines` | 将产品差异收敛到 gni 声明，模块代码通过宏判断特性，避免到处读 GN 变量 |
| 外部扩展动态检测 | `path_exists("//foundation/graphic/graphic_2d_ext/...")` | 允许厂商扩展仓按需存在，不存在时不影响主线构建 |
| ohos 与 ohos_ng 双平台 | `adapter/ohos/build/platform.gni` 注册两个平台 | `ohos_ng` 使用 NG_BUILD 宏和非 ASAN 构建，对应 ACE 引擎 NG 架构适配 |
| fuzz_test_output_path | `graphic_config.gni:391` 定义为 `"graphic_2d/graphic_2d"` | 统一 fuzz 测试产物输出路径，与 part_name 对齐 |

## 待补充背景

这些内容需要模块责任人后续补充，不能仅靠静态扫描完全确定：

- 各 `graphic_2d_feature_*` 开关在不同产品上的实际配置值（当前只能看到默认值和 phone/pc/tablet/wearable 的条件覆盖）。
- `graphic_2d_ext` 扩展仓的完整结构和与主线构建的集成边界。
- `ohos_ng` 平台与 `ohos` 平台的运行时差异（当前只看到 NG_BUILD 宏和 ACE 入口不同）。
- PGO（`graphic_2d_feature_enable_pgo`）和 codemerge 的实际使用场景和构建流程。
- `is_arkui_x` 跨平台编译时 adapter 复制脚本（`copy_arkui_adapters.py`）的工作机制。
