# 平台适配与预览

## 适用范围

改动涉及平台预览适配、adapter 目录、platform 目录、跨平台编译时，先读本文，
再回到代码确认当前实现。

本文是背景知识和排查路线，不替代代码。修改前仍需读取对应头文件、实现文件和测试。

## 快速代码地图

| 方向 | 关键文件（完整路径） |
| --- | --- |
| adapter 入口 | `adapter/ohos/build/platform.gni`、`adapter/preview/build/platform.gni` |
| OHOS 平台配置 | `adapter/ohos/build/config.gni`、`config_ng.gni`、`common.gni` |
| Preview 平台配置 | `adapter/preview/build/` 下 `config.gni`、`config_{w,m,l}.gni`、`preview_common.gni` |
| 平台发现 | `ace_platforms.gni`（仓库根） |
| RS 平台变量定义 | `rosen/modules/render_service_base/config.gni`（`rosen_is_*` 声明） |
| RS 平台源码 | `rosen/modules/render_service_base/src/platform/ohos/`、`windows/`、`darwin/` |
| RS 平台构建 | `rosen/modules/render_service_base/src/platform/BUILD.gn` |
| 通用平台依赖 | `rosen/modules/platform/config.gni`、`rosen/modules/platform/BUILD.gn` |
| 2D 平台条件 | `rosen/modules/2d_graphics/BUILD.gn` 中 `graphics2d_source_set` 模板 |
| 全局特性开关 | `graphic_config.gni`（仓库根） |
| arkui-x adapter 复制 | `utils/build/copy_arkui_adapters.py` |

## 核心模型

### 双 adapter 体系

本仓通过 `adapter/` 目录实现 OHOS 设备端和桌面端预览（Preview）两套平台适配：

1. **OHOS adapter**（`adapter/ohos/`）：
   - 仅在 `is_standard_system && !is_arkui_x` 时激活（platform.gni:18）。
   - 注册两个平台：`ohos`（标准）和 `ohos_ng`（NG 架构）。
   - `ohos` 平台（config.gni:20-23）开启 `FORM_SUPPORTED`、`PLUGIN_COMPONENT_SUPPORTED` 宏，
     `libace_target = //foundation/arkui/ace_engine/build:libace`；
     **ASAN 下改为 `:libace_compatible`**（config.gni:32-34）——此 ASAN override **仅 ohos 有**。
   - `ohos_ng` 平台（config_ng.gni:19）定义 `NG_BUILD` 宏，form/plugin 组件支持均为 false，
     `libace_target` 始终用 `:libace`（无 ASAN override）。
   - ASAN 下 `ohos_ng` 不注册（platform.gni:35-37 `if (!is_asan)` 才加入）。

2. **Preview adapter**（`adapter/preview/`）：
   - 根据宿主操作系统注册平台（platform.gni）：`mingw_x86_64` → windows，
     `mac_x64`/`mac_arm64` → mac，`linux` → linux。
   - 不依赖 ACE 引擎，用于 IDE 预览场景。
   - `use_external_icu = "shared"`（common.gni:14、preview_common.gni:16）。
   - linux preview 额外定义 `GPU_DISABLED`（config_linux.gni:20），**显式禁用 GPU**。

### 平台发现流程

`ace_platforms.gni` 驱动平台发现：

1. `ace_platforms.gni:14` 从 `graphic_config.gni` 获取 `adapters` 列表。
   `graphic_config.gni:386-400`：`!ohos_indep_compiler_enable` 时由
   `ace_root/build/search.py` 动态发现 adapter 子目录（search.py 在 ace_engine 仓，不在本仓）；
   `ohos_indep_compiler_enable` 时硬编码为 `["preview", "ohos"]`。
2. `ace_platforms.gni:21-38` 对每个 adapter import `adapter/$item/build/platform.gni` 获取 `platforms`。
3. `is_arkui_x` 构建时仅保留设置了 `cross_platform_support` 的平台（:30-32）。
4. 最终 `ace_platforms` 列表驱动各模块的平台条件编译。
5. `ace_platforms.gni:16` 定义 `enable_glfw_window = false`（默认关），
   由 darwin/windows 的 BUILD.gn 条件注入 `USE_GLFW_WINDOW` 宏。

### rosen_is_* 变量定义

`rosen/modules/render_service_base/config.gni:16-21` 完整声明 RS 平台判断变量
（原文档"待补充背景"称"未找到声明"，实际声明在此）：

| 变量 | 行号 | 取值逻辑 |
| --- | --- | --- |
| `rosen_is_ohos` | `:16` | `current_os == "ohos"` |
| `rosen_is_android` | `:17` | `current_os == "android"` |
| `rosen_is_ios` | `:18` | `current_os == "ios" \|\| current_os == "tvos"`（含 tvos） |
| `rosen_is_mac` | `:19` | `current_os == "mac"` |
| `rosen_is_win` | `:20` | `current_os == "win" \|\| current_os == "mingw"` |
| `rosen_is_linux` | `:21` | `current_os == "linux"` |

派生变量：
- `rosen_cross_platform`（:25-27）= mac/mingw/linux/android/ios 之或
- `rosen_preview`（:28）= `rosen_is_mac || rosen_is_win || rosen_is_linux`

`config.gni:30-79` 还按平台注入 `ROSEN_*` 宏：
- `rosen_cross_platform` → `ROSEN_CROSS_PLATFORM`、`ROSEN_DISABLE_DEBUGLOG`、`ROSEN_TRACE_DISABLE`
- `is_emulator` → `ROSEN_EMULATOR`（:45-47）
- `rosen_preview` → `ROSEN_PREVIEW`（:49-51）
- `rosen_is_ohos` → `ROSEN_OHOS`（:53-55）
- `rosen_is_android` → `ROSEN_ANDROID`、`USE_SURFACE_TEXTURE`、`ROSEN_ARKUI_X`（:57-61）
- `rosen_is_ios` → `ROSEN_IOS`、`USE_SURFACE_TEXTURE`、`ROSEN_ARKUI_X`（:63-67）
- `rosen_is_mac/win/linux` → `ROSEN_MAC`/`ROSEN_WIN`/`ROSEN_LINUX`

### RS 平台分支

`rosen/modules/render_service_base/src/platform/BUILD.gn:17-29`（group("platform")）：

- `rosen_is_ohos` → `public_deps = ["ohos"]`（生产环境，完整 IPC、GPU 后端）
- `rosen_is_win || current_os == "linux"` → `["windows"]`
  （预览/开发；**linux 宿主构建复用 windows 子目录**）
- `rosen_is_mac` → `["darwin"]`（macOS 预览）
- `rosen_is_ios || rosen_is_android` → `["$appframework_root/graphic_2d:platform"]`（委托 appframework 仓）
- `rosen_is_linux` → **空分支（不可达死代码）**，因 linux 已被上面的 `current_os == "linux"` 捕获
- else → `assert(false, "Unsupported platform")`

> windows/darwin 子目录的 BUILD.gn 复用 `../ohos/rs_log.cpp`（windows/BUILD.gn:29、darwin/BUILD.gn:29），
> 跨目录共享日志实现。子目录实际只有 `ohos/`、`windows/`、`darwin/` 三个，无 ios/android。

### 2D 绘制平台实例化

`rosen/modules/2d_graphics/BUILD.gn` 使用 `graphics2d_source_set` 模板（:63），
通过 `foreach(item, ace_platforms)`（:439-461）为每个平台生成独立 source set：

- **ohos/ohos_ng**（:226-237）：sanitize 共 **7 项**
  （`boundary_sanitize`、`integer_overflow`、`ubsan`、`cfi`、`cfi_cross_dso`、
  `cfi_no_nvcall`、`cfi_vcall_icall_only`，`debug=false`），
  依赖 `init:libbegetutil`，启用 `ENABLE_OHOS_ENHANCE`、`OHOS_TEXT_ENABLE`、
  `SUPPORT_OHOS_PIXMAP`、`ROSEN_OHOS` 宏（:360-379）。
- **android/ios**（:240-245）：跨平台编译，ios 额外定义 `ROSEN_IOS`。
- **windows/mac/linux**（:381-409）：`cflags += ["-std=c++17"]`，
  `deps += ["$rosen_root/modules/platform:hilog"]`，is_arkui_x 时加 `CROSS_PLATFORM`。

### 通用平台依赖（rosen/modules/platform）

`config.gni`（:14-36）按平台切换依赖来源：
- **ohos**：`eventhandler_deps`→`libeventhandler`、`ipc_deps`→`ipc_core`、
  `image_deps`→`image_native`、`utils_deps`→`c_utils:utils`（:21-31）
- **其他平台**：`hilog_deps` 指向 `libhilog_$platform`（:32-36）

## Preview 渲染后端

原"待补充背景"已结案。preview 模式使用 **GLFW 窗口 + EGL/GLES** 后端，非纯 CPU 光栅化：

- `rosen/modules/glfw_render_context/BUILD.gn:35-52` 在 `rosen_preview` 下产
  `libglfw_render_context.so`，`deps = ["//third_party/glfw:glfw"]`。
- `render_service_client/BUILD.gn:368-373` preview 分支依赖 `EGL`、`GLESv3`、
  `libglfw_render_context`。
- windows/mac 的 RS 平台 BUILD.gn 依赖 `egl:libEGL`、`graphic_surface:surface_headers`、
  `libglfw_render_context`，并通过 `enable_glfw_window`（ace_platforms.gni:16，默认 false）
  注入 `USE_GLFW_WINDOW` 宏（windows/BUILD.gn:44-46、darwin/BUILD.gn:47-49）。
- **linux preview 通过 `GPU_DISABLED` 禁用 GPU**（config_linux.gni:20）。
- 2D 绘制侧（`2d_graphics/BUILD.gn:281-304`）：`rs_enable_gpu` 为 true 时加 GPU 相关源文件
  （`skia_gpu_context.cpp`、`skia_task_executor.cpp`、`gpu_context.cpp`）。

详见 `docs/knowledge/glfw-render-context.md`。

## ohos_ng 运行时差异

原"待补充背景"已结案（跨仓确认）：

- `NG_BUILD` 宏**仅定义于** `adapter/ohos/build/config_ng.gni:19`，
  **本仓 C++ 代码不消费该宏**（grep `NG_BUILD` 仅命中定义本身）。
- ohos 与 ohos_ng 的差异仅在 adapter 配置层：
  - `ohos`：`form/plugin_components_support = true`，defines 含 `FORM_SUPPORTED`/`PLUGIN_COMPONENT_SUPPORTED`
  - `ohos_ng`：`form/plugin_components_support = false`，defines 仅 `NG_BUILD`
  - `ohos` 的 `platform_deps` → `ace_ohos_standard_entrance_ohos` / `ace_osal_ohos_ohos`
  - `ohos_ng` 的 `platform_deps` → `..._ohos_ng` / `..._ohos_ng`
- **`NG_BUILD` 的实际效果在 ace_engine 仓内**，本仓无法静态确认运行时行为。

## copy_arkui_adapters.py 流程

原"待补充背景"已结案。脚本位于 `utils/build/copy_arkui_adapters.py`（57 行）：

- 调用点：`graphic_config.gni:389-393`，参数 `[_ace_adapter_dir, _graphic_2d_adapter_dir]`
  （ace_engine adapter → graphic_2d adapter），仅 `is_arkui_x` 时执行。
- 执行逻辑（:41-54）：
  1. 取 source_dir（ace_engine adapter 目录）、dest_dir（graphic_2d adapter 目录）。
  2. 遍历 source_dir 下所有 item，**跳过 `ohos` 和 `preview`**（:42-43）。
  3. 仅处理含 `build/platform.gni` 的子目录（:48-50）。
  4. `shutil.copytree(source/item/build, dest/item/build, dirs_exist_ok=True)`（:52-54）。
- 产物布局：把 ace_engine 的 android/ios 等 adapter 的 `build/` 子目录复制到
  `graphic_2d/adapter/<platform>/build/`，使 `ace_platforms.gni` 能 import 它们的 `platform.gni`。

## platform 子模块平台差异

原"待补充背景"已结案。`rosen/modules/platform/BUILD.gn` 各子模块按平台选择实现：

| 子模块 | ohos | android/ios | 其他（preview） |
| --- | --- | --- | --- |
| eventhandler | 外部 `libeventhandler` | appframework `cross_platform_eh` | `:impl_eventhandler`（8 cpp） |
| image_native | 外部 `image_native` | 外部 `image_native` | `:mock_image_native`（仅 `pixel_map.cpp`） |
| ipc_core | 外部 `ipc:ipc_core` | `:mock_ipc_core`（6 cpp） | `:mock_ipc_core` |
| utils | 外部 `c_utils:utils` | 外部 `c_utils:utilsbase` | `:mock_utils`（3 cpp） |
| hilog | 外部 `hilog:libhilog` | `hilog_deps` | 外部 `hilog:libhilog` |
| ace_skia | `skia:skia_canvaskit` | `skia_$platform` | `skia:skia_canvaskit` |

行号引用：eventhandler（:31/33/45-73）、image_native（:94/96/108-116）、
ipc_core（:123/135-149）、utils（:156/157/181-198）、hilog（:78-86）、ace_skia（:18-24）。

`impl_eventhandler` 的 8 个 cpp 含 `epoll_io_waiter_mingw.cpp`（mingw 专用）；
`mock_utils` 用 `bounds_checking_function:libsec_shared`；
`mock_ipc_core` 含 `ipc_object_stub.cpp`/`iremote_broker.cpp`/`message_parcel.cpp` 等 6 个。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| adapter 目录分离 | `adapter/{ohos,preview}/` 并列 | 设备端与预览端 ACE 依赖/宏/选项隔离 |
| ohos + ohos_ng 双平台 | platform.gni 注册两平台，ASAN 下仅保留 ohos | NG 架构需独立编译路径 |
| ASAN libace override | config.gni:32-34 **仅 ohos** 改 `libace_compatible` | ohos_ng 始终用 libace |
| foreach 平台实例化 | `graphics2d_source_set` 模板 | 同一源码为每平台生成编译单元 |
| platform 按 OS 子目录 | `src/platform/{ohos,windows,darwin}/` | 物理隔离；win/darwin 复用 ohos 日志 |
| linux 复用 windows 子目录 | BUILD.gn:20 `rosen_is_win \|\| "linux"` | linux 宿主与 windows 共用源码 |
| is_arkui_x adapter 复制 | `copy_arkui_adapters.py` 跳过 ohos/preview | 从 ace_engine 复制 android/ios adapter |
| 跨平台委托 | ios/android 委托 appframework | 移动端适配由 appframework 仓维护 |
| mock 实现 | `:mock_ipc_core`/`:mock_utils`/`:mock_image_native` | preview 无系统组件时提供桩 |

## 测试锚点

| 路径 | 说明 |
| --- | --- |
| `rosen/test/.../unittest/platform/ohos/` | platform 单测（仅 ohos）：accessibility/ashmem 等 |
| `.../unittest/platform/ohos/backend/` | backend 子目录测试 |
| `rosen/test/.../fuzztest/platform/ohos/` | platform fuzz（仅 ohos）：display/transaction 系列 |
| `rosen/test/dtk/BUILD.gn` | `ohos_indep_compiler_enable` 时定义 `GRAPHIC_2D_INDEP_BUILD`（:33-36） |
| 注册 | `rosen/test/render_service/BUILD.gn:213-214`（单测）、:99-108（fuzz） |

> windows/darwin 平台无测试目录。

## 待补充背景

- `NG_BUILD` 的实际运行时效果在 ace_engine 仓内，本仓无法静态确认。
- `ace_root/build/search.py` 不在本仓，动态发现逻辑需跨仓确认。
- windows/darwin 平台无单测/fuzz 覆盖。
- `rosen_is_linux` 死分支（platform/BUILD.gn:26）为历史遗留，建议清理但本次不动。
