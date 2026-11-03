# Rust 基础绑定

## 适用范围

改动涉及 render_service_base/rust Rust FFI 桥接和基础绑定时，先读本文，
再回到代码确认当前实现。

本文是背景知识和排查路线，不替代代码。修改前仍需读取对应头文件、实现文件和测试。

## 快速代码地图

| 方向 | 关键文件（完整路径） |
| --- | --- |
| Rust 源码 | `rosen/modules/render_service_base/rust/src/lib.rs`（唯一 .rs） |
| Rust 构建集成 | `rosen/modules/render_service_base/BUILD.gn`（:20-22、40-71、729-740） |
| cxx 桥接生成 | `rust_cxx("particle_cxx_gen")` target（:40-42） |
| Rust FF 库 | `ohos_rust_shared_ffi`/`ohos_rust_static_ffi("particle_cxx_rust")`（:44-58） |
| CXX C++ 依赖 | `ohos_static_library("particle_cxx_cppdeps")`（:60-71） |
| C++ 调用点 | `rosen/modules/render_service_base/src/animation/rs_render_particle_effector.cpp:74` |
| component 依赖 | `bundle.json:134`（`rust_cxx`，a07565464b 新增） |
| template 定义 | `code/build/templates/rust/rust_cxx.gni`、`rust_template.gni`（**本仓外**） |

## 核心模型

### cxx 桥接架构

本仓使用 [cxx](https://cxx.rs/) 框架在 Rust 和 C++ 之间建立安全 FFI 桥接。整体结构为：

```
Rust 侧 (lib.rs)
  ├── #[cxx::bridge] mod ffi { ... }   ← 声明 FFI 接口
  └── fn generate_value(...) -> f32     ← Rust 实际实现（模块级私有，无 pub）

C++ 侧 (自动生成 + 手动集成)
  ├── rust_cxx("particle_cxx_gen")     ← 自动生成 lib.rs.h / lib.rs.cc
  ├── particle_cxx_rust                ← 编译为 .so 或 .a
  ├── particle_cxx_cppdeps             ← 仅传播 defines/sanitize（无 sources）
  ├── rust_cxx:cxx_cppdeps（外部）      ← cxx 运行时（cxx.h/cxx.cc）
  └── librender_service_base           ← 最终消费 Rust 产物
```

> 注：`particle_cxx_cppdeps` 当前**无 sources 字段**，不编译 cxx.cc；
> cxx 运行时由 `external_deps = ["rust_cxx:cxx_cppdeps"]`（BUILD.gn:738）外部提供。
> 原文档称其为"C++ 侧 cxx 运行时"已不准确。

### 当前绑定的唯一函数

`lib.rs` 通过 cxx bridge 暴露了一个函数（确认仍唯一，无新增）：

```rust
fn generate_value(start_value: f32, end_value: f32,
                  start_time: i32, end_time: i32, current_time: i32) -> f32;
```

实现（lib.rs:25-33，**含除零保护**，原文档代码片段过时）：

```rust
fn generate_value(start_value: f32, end_value: f32, start_time: i32, end_time: i32, current_time: i32) -> f32 {
    let t = if end_time - start_time != 0 {
        (current_time - start_time) as f32 / (end_time - start_time) as f32
    } else {
        0.0
    };
    let interpolation_value = start_value * (1.0 - t) + end_value * t;
    interpolation_value
}
```

当 `end_time == start_time` 时 `t = 0.0`，返回 `start_value`（非 0.0，原文档描述有误）。

> 注：实现函数在 `mod ffi` 之外，是模块级私有函数（无 `pub`），由 cxx 桥接暴露给 C++。
> bridge 内无 `extern "C++"` 块（Rust 不调用 C++），单向暴露。

### 构建集成

`rosen/modules/render_service_base/BUILD.gn` 中 Rust 构建集成分三层：

1. **编译条件**（:20-22）：
   ```gn
   enable_compiler_rust = !ohos_indep_compiler_enable &&
                          ((!is_mingw && !is_mac && !is_cross_platform &&
                           !(host_os == "linux" && host_cpu == "arm64")))
   ```
   仅在完整 OHOS 设备端构建时启用 Rust 编译。
   > 注：实际代码在 `!is_mingw && ...` 外层多一对 `((...))` 括号，逻辑等价。

2. **Rust target 选择**（:44-58）：
   - `is_ohos && is_clang && target_cpu == "arm64" && defined(build_ext_path)` 条件下
     编译为 `ohos_rust_shared_ffi`（动态库）。
   - 其他条件编译为 `ohos_rust_static_ffi`（静态库）。
   - 两种方式都 `external_deps = ["rust_cxx:lib"]`，`part_name = "graphic_2d"`，`subsystem_name = "graphic"`。

3. **集成到 librender_service_base**（:729-740）：
   - 当 `enable_compiler_rust` 为 true 时：
     - `defines += ["ENABLE_RUST"]`（:729、:39 顶层 defines 也有）。
     - `sources += get_target_outputs(":particle_cxx_gen")`（:730）引入 cxx 生成的 `.h`/`.cc`。
     - `include_dirs += ["${target_gen_dir}/rust/src"]`（:731），C++ 侧 `#include "lib.rs.h"`。
     - `deps` 含 `particle_cxx_cppdeps`、`particle_cxx_gen`、`particle_cxx_rust`。
     - `external_deps += ["rust_cxx:cxx_cppdeps", "rust_cxx:lib"]`（:737-740）。

> `rust_cxx`、`ohos_rust_shared_ffi`、`ohos_rust_static_ffi` template 均定义在
> `code/build/templates/rust/`（本仓外），cxxbridge 可执行由 `rust_cxx:cxxbridge($host_toolchain)` 提供。

### cxx_cppdeps 特殊配置

`particle_cxx_cppdeps`（:60-71）是 `ohos_static_library`，**当前无 sources**，
仅传播 defines 和 sanitize 配置：

```gn
ohos_static_library("particle_cxx_cppdeps") {
  branch_protector_ret = "pac_ret"       // PAC 返回地址保护
  sanitize = {
    cfi = false
    cfi_cross_dso = false
    debug = false
    blocklist = "./rs_base_blocklist.txt"
  }
  defines += ["RUST_CXX_NO_EXCEPTIONS"]
  ...
}
```

> 原文档只展示 `cfi=false`、`cfi_cross_dso=false` 并用 `...` 隐藏，
> 实际还有 `debug=false`、`blocklist`、`branch_protector_ret="pac_ret"`。

`rs_base_blocklist.txt`（18 行）含 `[cfi]` 段，排除 `RSRenderAnimatableProperty*`、
`*Animation*` 等符号的 CFI 检查。

`RUST_CXX_NO_EXCEPTIONS` 与 Rust 侧的 no-panic 语义对齐；
本仓内 C++ 源码无 `#ifdef RUST_CXX_NO_EXCEPTIONS`，具体影响在 `rust_cxx` 组件的
cxx.h/cxx.cc（本仓外）。

## C++ 调用点与功能语义差异

原"待补充背景"称"无法从源码定位 C++ 侧调用点"，**已找到**：

`rosen/modules/render_service_base/src/animation/rs_render_particle_effector.cpp:74`
（`UpdateCurveValue` 方法）：

```cpp
#ifdef ENABLE_RUST
        value = generate_value(startValue, endValue, startTime, endTime, activeTime);
#else
        if (endTime - startTime > 0) {
            float t = static_cast<float>(activeTime - startTime) / static_cast<float>(endTime - startTime);
            auto& interpolator = valChangeOverLife[i]->interpolator_;
            t = (interpolator != nullptr) ? interpolator->Interpolate(t) : t;
            value = start_value * (1.0f - t) + end_value * t;
        }
#endif
```

调用链（均在同一文件，每帧每活跃粒子的每 CURVE-updator float 属性调一次）：
- `UpdateOpacity`（:178）、`UpdateScale`（:203）、`UpdateSpin`（:224）、
  `UpdateAccelerationAngle`（:246）、`UpdateAccelerationValue`（:268）。
- `UpdateColorCurveValue`（:90-123）**不**调用 `generate_value`，Color 有独立 `Lerp` 路径。

C++ 侧引用（rs_render_particle_effector.cpp:21-24）：
```cpp
#ifdef ENABLE_RUST
#include "cxx.h"
#include "lib.rs.h"
#endif
```

**关键功能语义差异**（原文档未提）：
- Rust 路径 `generate_value` **只做线性插值**，**忽略 `interpolator_`（缓动曲线）**。
- C++ `#else` 路径会调 `interpolator->Interpolate(t)` 应用缓动曲线（bounce/elastic 等）。
- 因此启用 `ENABLE_RUST` 会**改变粒子动画行为**，使曲线属性按线性插值
  而非配置的缓动曲线运动。

## 测试缺口

原"待补充背景"已结案：

- **Rust 侧**：`lib.rs` 无 `#[test]`、无 `#[cfg(test)]`、无 `tests/` 目录。无任何 Rust 单元测试。
- **C++ 侧**：
  - `rosen/test/.../animation/rs_render_particle_test.cpp` 只测 `CalculateParticlePosition`，
    **不覆盖** `UpdateCurveValue`/`generate_value`。
  - 测试 target deps 不含 `particle_cxx_*`，Rust 代码只能经 librender_service_base 间接链入。
- **fuzz**：
  - `rosen/test/2d_graphics/fuzztest/draw/particle_fuzzer/`（2d_graphics 侧，不涉及 Rust）。
  - `rosen/test/.../fuzztest/rsparticlenoise_fuzzer/`（noise field，不涉及 generate_value）。
  - **无针对 Rust FFI 边界或 generate_value 的 fuzz**。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| cxx 而非手写 FFI | `#[cxx::bridge]` + `rust_cxx` target | 编译期类型安全，避免手写 FFI 内存错误 |
| 条件编译 Rust | `enable_compiler_rust` 排除 mingw/mac 等 | 预览/交叉编译环境工具链可能不可用 |
| arm64+ext 用动态库 | `ohos_rust_shared_ffi` 分支 | 扩展构建场景下动态加载支持运行时替换 |
| 禁用 cfi 和异常 | `cfi=false` + `RUST_CXX_NO_EXCEPTIONS` | cxx 生成代码与 Rust panic 不兼容 |
| 单一桥接模块 | 当前仅 `generate_value` 一个函数 | 初期阶段，以最小接口验证端到端 |
| ENABLE_RUST 忽略 interpolator | `generate_value` 只线性插值 | 改变粒子缓动曲线行为（风险） |
| particle_cxx_cppdeps 无 sources | 仅传播 defines/sanitize | cxx 运行时由外部 `rust_cxx:cxx_cppdeps` 提供 |

## 待补充背景

- **无 Rust 迁移路线图**：仓内无规划文档；`#[cxx::bridge]` 全仓仅此一处，
  Rust 集成自 2023-08 引入后基本停滞在单一函数验证阶段。
- `enable_compiler_rust` 排除 mac 构建的原因：BUILD.gn 无注释说明，
  推测为 mac 预览环境 Rust 工具链不可用，但无文档佐证。
- `RUST_CXX_NO_EXCEPTIONS` 对 cxx 错误处理路径的具体影响：
  本仓内无使用点，需查 `rust_cxx` 组件的 cxx.h/cxx.cc（本仓外）。
- 无 Rust 代码审查或安全性审计流程文档（`OAT.xml` 无 Rust 条目）。
- 启用 `ENABLE_RUST` 会忽略 `interpolator_` 导致粒子动画行为变化，
  是否为预期行为需模块责任人确认。
- git 演进：原始提交（555af209fe）条件为 `!is_mingw && !is_mac`，
  后演进为当前复杂条件；a07565464b（2025-04-11）将 `deps` 改 `external_deps`、
  bundle.json 加 `rust_cxx`、移除 `particle_cxx_cppdeps` 的 sources。
