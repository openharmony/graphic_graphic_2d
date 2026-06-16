# Rust 基础绑定

## 适用范围

改动涉及 render_service_base/rust Rust FFI 桥接和基础绑定时，先读本文，再回到代码确认当前实现。

本文是背景知识和排查路线，不替代代码。修改前仍需读取对应头文件、实现文件和测试。

## 快速代码地图

| 方向 | 关键文件（完整路径） |
| --- | --- |
| Rust 源码 | `rosen/modules/render_service_base/rust/src/lib.rs` |
| Rust 构建集成 | `rosen/modules/render_service_base/BUILD.gn`（第 20-71 行、698-711 行） |
| cxx 桥接生成 | `rust_cxx("particle_cxx_gen")` target |
| Rust FF 库 | `ohos_rust_shared_ffi("particle_cxx_rust")` 或 `ohos_rust_static_ffi("particle_cxx_rust")` |
| CXX C++ 依赖 | `ohos_static_library("particle_cxx_cppdeps")` |
| 外部依赖 | `rust_cxx:lib`（cxx crate 运行时） |

## 核心模型

### cxx 桥接架构

本仓使用 [cxx](https://cxx.rs/) 框架在 Rust 和 C++ 之间建立安全 FFI 桥接。整体结构为：

```
Rust 侧 (lib.rs)
  ├── #[cxx::bridge] mod ffi { ... }   ← 声明 FFI 接口
  └── fn generate_value(...) -> f32     ← Rust 实际实现

C++ 侧 (自动生成 + 手动集成)
  ├── rust_cxx("particle_cxx_gen")     ← 自动生成 C++ 绑定代码
  ├── particle_cxx_rust                ← 编译为 .so 或 .a
  ├── particle_cxx_cppdeps             ← C++ 侧 cxx 运行时
  └── librender_service_base           ← 最终消费 Rust 产物
```

### 当前绑定的唯一函数

`lib.rs` 通过 cxx bridge 暴露了一个函数：

```rust
fn generate_value(start_value: f32, end_value: f32,
                  start_time: i32, end_time: i32,
                  current_time: i32) -> f32;
```

实现为线性插值：

```rust
let t = (current_time - start_time) as f32 / (end_time - start_time) as f32;
let interpolation_value = start_value * (1.0 - t) + end_value * t;
```

当 `end_time == start_time` 时返回 0.0，避免除零。

该函数用于动画属性插值计算，是 Rust 在渲染服务中的首个集成点。

### 构建集成

`rosen/modules/render_service_base/BUILD.gn` 中 Rust 构建集成分三层：

1. **编译条件**：
   - `enable_compiler_rust = !ohos_indep_compiler_enable && !is_mingw && !is_mac && !is_cross_platform && !(host_os == "linux" && host_cpu == "arm64")`
   - 仅在完整 OHOS 设备端构建时启用 Rust 编译。

2. **Rust target 选择**：
   - `is_ohos && is_clang && target_cpu == "arm64" && defined(build_ext_path)` 条件下编译为 `ohos_rust_shared_ffi`（动态库）。
   - 其他条件编译为 `ohos_rust_static_ffi`（静态库）。
   - 两种方式都依赖 `rust_cxx:lib`，`part_name = "graphic_2d"`，`subsystem_name = "graphic"`。

3. **集成到 librender_service_base**：
   - 当 `enable_compiler_rust` 为 true 时：
     - 定义 `ENABLE_RUST` 宏。
     - 引入 cxx 生成的 C++ 源码（`get_target_outputs(":particle_cxx_gen")`）。
     - 添加生成的头文件路径（`${target_gen_dir}/rust/src`）。
     - 依赖 `particle_cxx_cppdeps`、`particle_cxx_gen`、`particle_cxx_rust`。
     - 外部依赖 `rust_cxx:cxx_cppdeps`、`rust_cxx:lib`。

### cxx_cppdeps 特殊配置

`particle_cxx_cppdeps` 是一个 `ohos_static_library`，其 sanitize 配置显式禁用了 cfi：
```gn
sanitize = {
  cfi = false
  cfi_cross_dso = false
  ...
}
```
并定义 `RUST_CXX_NO_EXCEPTIONS` 宏，与 Rust 侧的 no-panic 语义对齐。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| cxx 而非手写 FFI | `#[cxx::bridge]` 注解 + `rust_cxx` GN target | cxx 提供编译期类型安全检查，避免手写 FFI 的常见内存安全错误 |
| 条件编译 Rust | `enable_compiler_rust` 排除 mingw/mac/cross_platform/独立编译器 | Rust 工具链在预览和交叉编译环境可能不可用，需要优雅降级 |
| arm64+ext 使用动态库 | `ohos_rust_shared_ffi` 条件分支 | 扩展构建场景下需要动态加载 Rust 库以支持运行时替换 |
| 禁用 cfi 和异常 | `particle_cxx_cppdeps` 中 `cfi = false` + `RUST_CXX_NO_EXCEPTIONS` | cxx 生成的 C++ 代码与 Rust panic 机制不兼容，CFI 对跨语言边界支持有限 |
| 单一桥接模块 | 当前仅 `generate_value` 一个函数 | Rust 集成处于初期阶段，以最小可行接口验证端到端流程 |

## 待补充背景

这些内容需要模块责任人后续补充，不能仅靠静态扫描完全确定：

- Rust 集成的长期路线图：哪些模块计划从 C++ 迁移到 Rust？
- `generate_value` 的调用方和调用频率（当前只能在 BUILD.gn 中确认集成，无法从源码定位 C++ 侧调用点）。
- Rust 代码的测试策略：是否有 Rust 侧的单元测试？C++ 侧集成测试的覆盖情况？
- `enable_compiler_rust` 排除 mac 构建的原因（工具链可用性？性能？）。
- `RUST_CXX_NO_EXCEPTIONS` 对 cxx 错误处理路径的影响。
- Rust 代码的代码审查和安全性审计流程。
