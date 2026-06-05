# 平台适配与预览

## 适用范围

改动涉及平台预览适配、adapter 目录、platform 目录、跨平台编译时，先读本文，再回到代码确认当前实现。

本文是背景知识和排查路线，不替代代码。修改前仍需读取对应头文件、实现文件和测试。

## 快速代码地图

| 方向 | 关键文件（完整路径） |
| --- | --- |
| adapter 入口 | `adapter/ohos/build/platform.gni`、`adapter/preview/build/platform.gni` |
| OHOS 平台配置 | `adapter/ohos/build/config.gni`、`adapter/ohos/build/config_ng.gni`、`adapter/ohos/build/common.gni` |
| Preview 平台配置 | `adapter/preview/build/config.gni`、`adapter/preview/build/config_windows.gni`、`adapter/preview/build/config_mac.gni`、`adapter/preview/build/config_linux.gni`、`adapter/preview/build/preview_common.gni` |
| 平台发现 | `ace_platforms.gni`（仓库根） |
| RS 平台源码 | `rosen/modules/render_service_base/src/platform/ohos/`、`windows/`、`darwin/` |
| RS 平台构建 | `rosen/modules/render_service_base/src/platform/BUILD.gn` |
| 通用平台依赖 | `rosen/modules/platform/config.gni`、`rosen/modules/platform/BUILD.gn` |
| 2D 平台条件 | `rosen/modules/2d_graphics/BUILD.gn` 中 `graphics2d_source_set` 模板 |
| 全局特性开关 | `graphic_config.gni`（仓库根） |

## 核心模型

### 双 adapter 体系

本仓通过 `adapter/` 目录实现 OHOS 设备端和桌面端预览（Preview）两套平台适配：

1. **OHOS adapter**（`adapter/ohos/`）：
   - 仅在 `is_standard_system && !is_arkui_x` 时激活。
   - 注册两个平台：`ohos`（标准）和 `ohos_ng`（NG 架构，ASAN 构建下不启用）。
   - `ohos` 平台开启 `FORM_SUPPORTED` 和 `PLUGIN_COMPONENT_SUPPORTED` 宏，依赖 ACE 引擎标准入口。
   - `ohos_ng` 平台定义 `NG_BUILD` 宏，使用 ACE NG 架构入口。
   - `libace_target` 指向 `//foundation/arkui/ace_engine/build:libace`（ASAN 下改为 `:libace_compatible`）。

2. **Preview adapter**（`adapter/preview/`）：
   - 根据宿主操作系统注册平台：`mingw_x86_64` → windows，`mac_x64`/`mac_arm64` → mac，`linux` → linux。
   - 不依赖 ACE 引擎，用于 IDE 预览场景。
   - `use_external_icu = "shared"` 配置 ICU 为共享库。

### 平台发现流程

`ace_platforms.gni` 驱动平台发现：

1. 从 `graphic_config.gni` 获取 `adapters` 列表。正常构建时通过 `ace_root/build/search.py` 动态发现 adapter 子目录；`ohos_indep_compiler_enable` 时硬编码为 `["preview", "ohos"]`。
2. 对每个 adapter 导入 `adapter/$item/build/platform.gni` 获取 `platforms` 列表。
3. `is_arkui_x` 构建时仅保留设置了 `cross_platform_support` 的平台。
4. 最终 `ace_platforms` 列表驱动各模块的平台条件编译。

### RS 平台分支

`rosen/modules/render_service_base/src/platform/BUILD.gn` 根据 `rosen_is_ohos`/`rosen_is_win`/`rosen_is_mac`/`rosen_is_ios`/`rosen_is_android` 条件选择子目录：

- **ohos**：生产环境，完整 IPC、GPU 后端。
- **windows**（含 linux 宿主构建）：预览和开发环境。
- **darwin**：macOS 预览环境。
- **ios/android**：委托给 `appframework_root/graphic_2d:platform`。

### 2D 绘制平台实例化

`rosen/modules/2d_graphics/BUILD.gn` 使用 `graphics2d_source_set` 模板，通过 `foreach(item, ace_platforms)` 为每个平台生成独立 source set：

- **ohos/ohos_ng**：启用 sanitize（boundary_sanitize、integer_overflow、ubsan、cfi），依赖 `init:libbegetutil`，启用 `ENABLE_OHOS_ENHANCE`、`OHOS_TEXT_ENABLE`、`SUPPORT_OHOS_PIXMAP`、`ROSEN_OHOS` 宏。
- **android/ios**：跨平台编译，ios 额外定义 `ROSEN_IOS`。
- **windows/mac/linux**：预览环境，使用 `-std=c++17` 编译，依赖本地 hilog。

### 通用平台依赖（rosen/modules/platform）

`config.gni` 根据平台切换依赖来源：

- **ohos 平台**：`eventhandler_deps`、`ipc_deps`、`image_deps`、`utils_deps` 指向 OHOS 系统组件。
- **其他平台**：`hilog_deps` 指向本地 hilog 库，不引入系统 IPC/事件处理器。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| adapter 目录分离 | `adapter/ohos/` 与 `adapter/preview/` 并列 | 设备端与预览端的 ACE 依赖、宏定义、编译选项完全隔离，避免交叉污染 |
| ohos + ohos_ng 双平台 | `adapter/ohos/build/platform.gni` 注册两平台，ASAN 下仅保留 ohos | NG 架构需要独立编译路径和入口，ASAN 环境下 NG 架构存在兼容性问题 |
| foreach 平台实例化 | `2d_graphics/BUILD.gn` 中 `graphics2d_source_set` 模板 + `foreach(item, ace_platforms)` | 同一套源码通过模板为每个平台生成独立编译单元，平台差异收敛到 config 和 defines |
| platform 目录按 OS 子目录 | `render_service_base/src/platform/{ohos,windows,darwin}/` | 平台相关代码物理隔离，编译时仅链接对应子目录 |
| is_arkui_x adapter 复制 | `graphic_config.gni` 中 `copy_arkui_adapters.py` 脚本 | arkui-x 跨平台编译时需要从 ace_engine 复制 android/ios adapter 到本仓 adapter 目录 |
| 跨平台平台委托 | `platform/BUILD.gn` 中 ios/android 委托 `appframework_root/graphic_2d:platform` | 移动端平台适配由 appframework 仓维护，graphic_2d 仓不直接包含移动端实现 |

## 待补充背景

这些内容需要模块责任人后续补充，不能仅靠静态扫描完全确定：

- `rosen_is_ohos`、`rosen_is_win`、`rosen_is_mac` 等变量的定义位置和完整取值逻辑（当前只在 BUILD.gn 中看到使用，未找到声明）。
- Preview 模式下 2D 绘制的实际渲染后端（CPU 光栅化？软件渲染？）和功能限制。
- `ohos_ng` 平台的运行时行为差异和部署约束。
- arkui-x 编译时 `copy_arkui_adapters.py` 的实际执行流程和产物布局。
- `rosen/modules/platform/` 下各子模块（eventhandler_impl、image_native、ipc_core、utils）的平台差异实现。
