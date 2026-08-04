# 构建、组件与裁剪

## 适用范围

改动涉及 `BUILD.gn`、`bundle.json`、`*.gni`、`part_name`、产品特性开关、平台发现或
部件裁剪时，先读本文，再回到目标文件确认当前实现。

本文回答四类问题：产物从哪里进入部件、开关如何生效、平台 target 如何生成、
优化配置如何接入。它不替代产品侧 `args.gn`、扩展仓或实际构建结果。

## 快速代码地图

| 方向 | 关键文件 |
| --- | --- |
| 仓级构建入口 | `BUILD.gn` |
| 部件声明和聚合 | `bundle.json` |
| 全局特性开关 | `graphic_config.gni` |
| 平台发现 | `ace_platforms.gni` |
| OHOS 平台 | `adapter/ohos/build/platform.gni`、`config.gni`、`config_ng.gni` |
| Preview 平台 | `adapter/preview/build/platform.gni`、各主机 `config_*.gni` |
| ArkUI-X adapter 同步 | `utils/build/copy_arkui_adapters.py` |
| 2D 绘制构建 | `rosen/modules/2d_graphics/BUILD.gn` |
| RS Base 构建 | `rosen/modules/render_service_base/BUILD.gn` |
| RS 服务端构建 | `rosen/modules/render_service/BUILD.gn` |
| RS 平台适配 | `rosen/modules/render_service_base/src/platform/BUILD.gn` |
| 通用平台依赖 | `rosen/modules/platform/config.gni`、`rosen/modules/platform/BUILD.gn` |
| 文本配置 | `frameworks/text/config.gni` |

## 先建立构建链路

定位构建问题时按以下顺序检查：

1. `bundle.json` 是否把目标放入正确的 `group_type`，或被其他 target 间接依赖。
2. 目标 `BUILD.gn` 是否声明正确的模板、依赖、`part_name` 和 `subsystem_name`。
3. `graphic_config.gni` 中开关的默认值，是否又被产品类型、系统组件或扩展仓覆盖。
4. 平台 target 是否由 `adapters` → `ace_platforms` → 模块模板实际展开。
5. 已生成的构建目录中，`args.gn` 和 `gn desc` 的结果是否符合预期。

不能只根据 `declare_args()` 的默认值判断产品行为。产品侧传入的 GN 参数不在本仓，
最终值应以对应产品的生成目录为准。检查生成目录的 `args.gn`，并使用
`gn desc <out_dir> <target>` 查看目标最终配置。

## 部件聚合与 target 归属

### `bundle.json` 的三类聚合

| 分组 | 当前主要内容 | 使用边界 |
| --- | --- | --- |
| `base_group` | 多语言绑定、2D、Composer、VSync、OpenGL、Vulkan、HGM、DDGR | 基础图形能力 |
| `fwk_group` | RS Base、RS Proxy、RS Client | 应用或框架进程使用的 RS 库 |
| `service_group` | RS 服务、进程、参数与配置 | 标准系统服务端部署 |

这三组描述部件产物的部署层次，不等于“任意产品必然安装全部 target”。
产品选择哪些部件和 group，仍由产品配置及上层构建系统决定。

### `part_name` 与 `subsystem_name`

本仓正式产物通常使用：

```gn
part_name = "graphic_2d"
subsystem_name = "graphic"
```

新增 target 时应复用邻近同类 target 的模板和归属。不要仅为解决依赖可见性而新建 part，
也不要把 `public_deps` 当作普通 `deps` 使用；公开传播依赖会扩大编译边界。

target 要进入镜像，还必须被上层 target 依赖、进入正确的 bundle group，并由产品实际选择
该 group。`install_enable` 或安装目录只控制安装方式，不能替代依赖和部件聚合。

## 特性开关取值链路

`graphic_config.gni` 的处理顺序可概括为：

1. `declare_args()` 提供默认值，产品可从外部覆盖。
2. `graphic_2d_feature_product` 对部分开关做仓内二次赋值。
3. `global_parts_info` 根据系统已安装部件关闭或开启集成能力。
4. `path_exists()` 检测 `graphic_2d_ext` 中的可选配置。
5. 布尔值被转换为 `gpu_defines` 等宏列表，再由模块 target 引入。

### 仓内可确认的产品差异

| 产品值 | 仓内附加行为 |
| --- | --- |
| `phone` | 开启 Vulkan、stack culling、OPINC 和 HVE blur |
| `pc` | 开启 Vulkan、stack culling 和 OPINC |
| `tablet` | 开启 Vulkan、stack culling、OPINC 和全屏识别 |
| `wearable` | 开启 Vulkan、stack culling 和 OPINC |
| `default` 或其他值 | 不触发上述产品分支，保留参数默认值或产品侧覆盖值 |

当 unified render 开启时，`phone` 和 `tablet` 还会开启 chipset VSync。表中只描述本仓的
条件赋值，不代表所有产品的最终配置；真实值必须检查产品侧 `args.gn`。

### 系统组件感知

`global_parts_info` 当前用于感知 accessibility、player framework、video processing engine、
display HDI、memory manager 和 access token 等组件。缺少组件时，相关功能可能被关闭，
因此“宏已声明但代码未进入”应同时检查部件是否实际安装。

### `graphic_2d_ext` 的边界

主仓只定义可选接入点，并不包含扩展实现。`graphic_config.gni` 当前探测的入口包括：

- `dvsync/BUILD.gn`
- `ddgr/config.gni`
- `delegator/config.gni`
- `ohcore/build/broker_config.gni`
- `hgm_manager/build/hgm_config.gni`
- `math_tools/build/math_tools_config.gni`
- `frame_stability/build/frame_stability_config.gni`
- `platform/config.gni`
- `subtree`、`hetero_hdr`、`mhc`、`car_features` 的构建配置

因此可以从主仓确认入口名称、导入条件和主仓消费方式，不能确认扩展仓的完整目录、
默认参数或产品部署结论。修改这些入口属于跨仓接口变化，应先确认。

### 特性开关改动检查

新增或修改特性开关时，应同步检查：

1. `declare_args()` 默认值及 `bundle.json` 的 `features`。
2. 产品分支、`global_parts_info` 和扩展仓是否会覆盖它。
3. 宏列表、source/deps 条件和所有消费点。
4. 开启与关闭两种配置下是否仍能解析 target。
5. 是否改变产品裁剪、公开 ABI、跨仓接口或设备行为；若改变，先人工确认。

## 平台发现与 `ohos_ng`

### 平台 target 的生成过程

1. `graphic_config.gni` 搜索本仓 `adapter/`，得到 `adapters`。
2. 独立编译器场景不执行搜索，直接使用 `["preview", "ohos"]`。
3. `ace_platforms.gni` 导入每个 `adapter/<name>/build/platform.gni`。
4. 每个 adapter 返回 `platforms`，ArkUI-X 场景还会过滤不支持跨平台的项。
5. 2D 等模块遍历 `ace_platforms`，为每个平台实例化独立 target。

平台 target 缺失时，应依次检查 adapter 是否被搜索到、`platform.gni` 是否返回
`platforms`、ArkUI-X 过滤条件、标准系统条件和 ASAN 条件。`ohos_ng` 在 ASAN 下不会注册。

### `ohos` 与 `ohos_ng` 的已知差异

| 项目 | `ohos` | `ohos_ng` |
| --- | --- | --- |
| ACE 入口与 OSAL | `*_ohos` target | `*_ohos_ng` target |
| 编译宏 | `FORM_SUPPORTED`、`PLUGIN_COMPONENT_SUPPORTED` | `NG_BUILD` |
| form/plugin | 开启 | 关闭 |
| ASAN | 使用兼容版 `libace` | 不注册该平台 |

两者都使用 `libace` 和共享的 `common.gni`。仓内只能确认编译入口、宏和依赖差异，不能据此
推导完整运行时行为；显示效果和设备行为仍需对应产品验证。

### ArkUI-X adapter 复制流程

非独立编译器且 `is_arkui_x=true` 时，`graphic_config.gni` 调用
`utils/build/copy_arkui_adapters.py`：

1. 源目录是 ACE Engine 的 `adapter/`，目标目录是本仓 `adapter/`。
2. 脚本跳过 `ohos` 和 `preview`。
3. 只处理含 `build/platform.gni` 的 adapter。
4. 目标 `build/` 不存在时才复制；已有目录不会被覆盖或增量更新。
5. 复制完成后，再由 ACE 的 `search.py` 搜索本仓 adapter。

这是一段配置阶段的目录补齐逻辑，不是运行时适配。若 ACE 侧 adapter 已变化，
而本仓目标目录已存在，脚本不会刷新它。
此时应先检查工作区来源和生成流程，不能直接手工覆盖。

## PGO 与 codemerge

两个开关默认均为 `false`，`graphic_2d_feature_pgo_path` 默认为空。本仓负责消费优化产物，
没有提供 profile 采集和生成流程。

- PGO 在 OHOS、Clang、ARM/ARM64 且满足 `enhanced_opt` 等 target 条件时生效。
- RS Base 和 RS 服务读取 `librender_service_base.profdata` 或
  `librender_service.profdata`。
- VSync 仅在产品为 `phone` 的对应分支读取 `libvsync.profdata`。
- codemerge 是 PGO 优化链路中的附加开关，会增加 `CM_FEATURE_ENABLE`、机器函数拆分参数，
  并读取 `librender_service_base.txt` 等符号排序文件。
- 缺少 `graphic_2d_feature_pgo_path` 下的输入文件时，相关 target 无法完成构建。

所以“打开开关即可生成 PGO 数据”是不成立的。profile、符号排序文件及其版本匹配
由仓外流程提供；本仓能验证的是条件、文件名和编译链接参数。

## 验证边界

文档或构建配置变更至少执行路径和符号核对、`git diff --check`，并在已有生成目录上使用
`gn desc` 验证 target。完整产品构建必须从 OpenHarmony 源码根目录执行，target 使用
`//foundation/graphic/graphic_2d/...` 前缀。

若没有产品源码根目录、扩展仓或构建机，只能确认主仓静态链路。
不能声称产品裁剪、PGO、ArkUI-X、`ohos_ng` 或设备行为已完成验证。
