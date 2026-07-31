# 多语言 API 绑定（NAPI/CJ/ANI/Taihe）

## 适用范围

改动涉及 `interfaces/kits/` 中 NAPI、CJ、ANI、Taihe 绑定，或 Text 目录下对应绑定、
公开 API 行为、SysCap、权限与 XTS 验证时，先读本文。

先区分“目录中存在实现”和“已进入部件聚合”。正式随本部件打包的入口以
`interfaces/kits/*/BUILD.gn` 的 package group 和 `bundle.json` 为准。

## 四类入口定位

| 入口 | 本仓角色 | 主要声明或注册方式 |
| --- | --- | --- |
| NAPI | JavaScript/ArkTS Native API 绑定 | `napi_module`、`napi_module_register` |
| CJ | 仓颉 FFI 绑定 | 导出 C ABI/FFI 函数 |
| ANI | ArkTS Native Interface 运行时绑定 | `ANI_Constructor`、native method 绑定 |
| Taihe | IDL 与代码生成链路 | `.taihe` → ANI/ABI 桥接代码、ETS 和 ABC |

Taihe 在本仓中不是第五套独立底层能力。它根据 IDL 生成 ANI 桥接层，再与手写 native 实现
共同构建。目录名或 target 名带 `taihe`，不代表它与 ANI 运行时无关。

## 快速代码地图

| 方向 | 关键路径 |
| --- | --- |
| NAPI 聚合 | `interfaces/kits/napi/BUILD.gn` |
| NAPI 图形模块 | `interfaces/kits/napi/graphic/` |
| CJ 聚合 | `interfaces/kits/cj/BUILD.gn` |
| CJ 模块 | `interfaces/kits/cj/{color_manager,effect_kit}/` |
| ANI 聚合 | `interfaces/kits/ani/BUILD.gn` |
| ANI 模块 | `interfaces/kits/ani/` |
| Taihe 聚合 | `interfaces/kits/taihe/BUILD.gn` |
| Taihe UIEffect | `interfaces/kits/taihe/ui_effect/` |
| NAPI Text | `frameworks/text/interface/mlb/napi/` |
| ANI Text | `frameworks/text/interface/mlb/ani/` |
| 部件聚合和头文件 | `bundle.json` |

## 当前进入 package group 的能力

| 能力 | NAPI | CJ | ANI group | Taihe group |
| --- | --- | --- | --- | --- |
| Drawing | `drawingnapi` | — | `ani_drawing` | — |
| ColorManager | 普通版和 Sendable 版 | `cj_color_manager_ffi` | `ani_color_space_manager` | — |
| EffectKit | `effectkit` | `cj_effect_kit_ffi` | `effectKit_ani`、ABC | — |
| HDR Capability | `hdrcapability_napi` | — | `ani_hdr_capability` | — |
| HGM | `libhgmnapi` | — | — | — |
| UIEffect | `uieffect_napi` | — | — | `uieffect_taihe_native`、ABC |
| WebGL | `libwebglnapi` | — | — | — |
| WindowAnimation | `windowanimationmanager_napi` | — | Taihe 生成的 ANI target | — |
| Text | `textnapi` | — | `text_engine_ani_group` | — |

`interfaces/kits/ani/ui_effect/` 中仍有 `uiEffect_ani` 实现，但当前 `ani_packages` 没有依赖它。
部件正式聚合的是 `interfaces/kits/taihe/ui_effect/`。不能仅凭目录存在就宣称产物已部署。

## 绑定层的数据流

一项公开能力通常经过以下层次：

1. API 声明：ArkTS/ETS 文件、Taihe IDL、CJ FFI 头文件或 NAPI 导出名。
2. 加载与注册：`napi_module_register`、`ANI_Constructor` 或 Taihe 生成的注册代码。
3. 参数转换和生命周期：JS/ETS/CJ 值与 C++ 对象互转，native 指针清理和异常映射。
4. 共享核心：`2d_graphics`、`color_manager`、`effect`、RS Client 等内部实现。
5. 构建与部署：共享库、生成的 ABC、package group 和 `bundle.json`。

公开行为变更不能只改第 3 层。
声明、加载库名、构建依赖、权限、错误语义和测试都属于接口。

## 测试与 XTS 边界

本仓的 `interfaces/kits/{napi,cj,ani,taihe}` 和 Text 绑定目录中，当前没有统一聚合的
`ohos_unittest`、`ohos_moduletest` 或 XTS target。ANI 目录中的 `drawing_test.ets`、
`effectKit_entry.ets` 等是构建或示例入口，不能等同于 XTS 覆盖。

因此：

- 本仓可做 package target 构建、注册符号核对和底层模块测试。
- 公共 API 的 XTS 用例名、覆盖率和执行入口不能从本仓得出，应在对应 XTS 仓确认。
- 未拿到 XTS 仓或设备时，最终结论必须写“未验证”，不能把编译通过当作 API 行为通过。

## 新增绑定的最小闭环

本仓没有单独的开发规范或脚手架，现阶段应复用同类目录，按以下最小闭环实施：

1. 定义公开声明、命名空间、类型、错误和可选参数。
2. 实现 native 注册、参数校验、对象所有权和异常转换。
3. 接入共享核心能力，避免复制业务逻辑。
4. 声明 `.so`、生成 ABC/IDL target，并加入对应 package group。
5. 检查 `bundle.json`、安装目录、加载库名、SysCap 和权限。
6. 增加语言侧正向、非法参数、生命周期和权限测试，并确认外部 XTS。

这是一份由现有实现归纳的仓内检查清单，不是跨仓 API 发布规范。新增公开 API、错误码或
默认行为前仍需人工确认。

## 关键能力差异

### 普通与 Sendable ColorManager

两者使用相同的 native `ColorSpace` 和相同的 `create` 参数解析，差异在 JS 对象封装：

- 普通模块名为 `graphics.colorSpaceManager`，使用普通对象和 `napi_wrap`，同时导出
  `ColorSpace`、`CMError`、`CMErrorCode`。
- Sendable 模块名为 `graphics.sendableColorSpaceManager`，使用
  `napi_create_sendable_object_with_properties` 和 `napi_wrap_sendable`，只导出 `create`。
- 两者最终都依赖 `utils/color_manager`，颜色空间计算语义不应分叉。

Sendable 对象的跨并发实例使用规则由 NAPI 运行时定义。本仓能确认对象创建和包装方式，
不能仅靠这段实现推导所有线程、序列化或传递限制。

### NAPI 与 ANI Drawing

两者当前覆盖 Canvas、Path、Pen、Brush、Font、Typeface、Filter、Region、Matrix 等
相近对象族，但声明和注册链路独立：

- NAPI 以 `DrawingInit` 注册 JS 类和方法。
- ANI 以 ETS 声明、`ANI_Constructor` 和 native method 表绑定，并额外生成 ABC。
- ANI 使用独立的 native 生命周期清理和运行时类型签名。

目录级组件相近不代表方法、重载、错误和权限一一一致。判断具体 API 是否对齐时，
应对照 NAPI 的 `js_drawing_init.cpp`、各 `js_*` 实现，以及 ANI 的 ETS 声明、
`ani_drawing_module.cpp` 和各 `ani_*` 实现，逐项比较签名和异常行为。

### NAPI 与 Taihe UIEffect

两者都调用 RS Client UIEffect 和 2D `Filter`、`VisualEffect`、`Mask` 等核心对象，但绑定
方式和当前 API 面不同：

- NAPI 直接在 C++ 中声明并注册 `createFilter`、`createEffect` 等方法。
- Taihe 以两份 `.taihe` IDL 定义 API，生成 ANI/ABI、ETS，再连接 `*_taihe.cpp` 实现。
- Taihe IDL 覆盖 Blur、PixelStretch、LiquidMaterial 等能力；NAPI 还存在 frosted glass、
  motion blur 等当前 IDL 未声明的方法。

因此 Taihe 不是 NAPI 的机械转译。修改共享能力时，要分别检查 NAPI 注册表、Taihe IDL、
生成产物输入和两侧 native 参数校验。

## SysCap 核对

`bundle.json.component.syscap` 是部件级能力列表，不是每个绑定文件注解的简单并集。公开声明
还可能引用依赖部件的 SysCap，例如图片能力。

检查顺序应为：

1. 从公开 ETS、头文件或 IDL 收集 `@syscap`。
2. 区分本部件提供的能力和依赖部件提供的能力。
3. 本部件能力与 `bundle.json.component.syscap` 核对。
4. 依赖能力与 `bundle.json.component.deps` 及对应部件声明核对。
5. 再检查权限校验和系统应用限制，SysCap 不能替代权限。

例如 ANI Drawing 使用 `SystemCapability.Graphics.Drawing`，该项存在于本部件 SysCap；
EffectKit 声明中的图片 SysCap 则属于 multimedia image 依赖。两者都可能正确。

## `inner_kits` 的边界

`bundle.json` 当前为部分绑定声明内部可见头文件，包括：

- NAPI Drawing、Text、ColorManager 转换层和 HDR 工具。
- CJ ColorManager 和 EffectKit。
- ANI ColorManager 转换层、Drawing 和 Text。

`inner_kits` 不是语言 API 清单，也不代表头文件是 SDK 公共 API。修改其中的 target、
`header_base`、结构体或符号可能影响其他部件，需先做依赖检索和兼容性确认。

## 公开行为变更检查清单

1. 先确认变更是否涉及公开 API、默认值、错误码、权限或 XTS 预期；涉及则先人工确认。
2. 找到底层共享能力和所有已聚合的语言入口，不以目录数量代替 package group 检查。
3. 同步声明、native 注册、参数转换、对象生命周期和错误语义。
4. 检查异步回调的线程归属、native 指针所有权和异常路径释放。
5. 同步共享库、ABC/IDL、package group、`bundle.json` 和安装路径。
6. 核对 SysCap、权限、系统应用限制和加载库名。
7. 为每个受影响入口验证正常、边界、非法参数和生命周期场景。
8. 到外部 XTS 仓确认用例；无法访问时明确记录缺口。

## 验证建议

静态检查至少包括：

- package group 是否引用正确 target，target 名是否真实存在。
- NAPI 模块名、ANI/Taihe 加载库名和输出名是否一致。
- ETS、Taihe IDL、native 签名和错误分支是否对应。
- `bundle.json` 的 SysCap、依赖和 `inner_kits` 是否与变更匹配。
- `git diff --check` 和路径引用检查。

构建从 OpenHarmony 源码根目录执行，优先构建受影响的 package target 和底层模块。
公共 API 行为还需对应语言运行时、XTS 和必要的真实设备验证；
没有这些环境时不得声称完整通过。
