# IPC / Parcel / Command / SAFuzz 安全输入专题

## 适用场景

改动涉及以下任一内容时，先读本文：

- 新增或修改 RS IPC 接口、Stub、Proxy、回调注册或权限校验。
- 修改 `MessageParcel` 字段顺序、字段类型、默认值、兼容分支或读取失败返回。
- 新增或修改 `RSCommand`、`RSTransactionData`、command marshalling/unmarshalling。
- 新增 SAFuzz 配置、随机 command builder、Parcel writer helper 或 IPC simulator case。
- 解析应用可控数据，例如 PixelMap、Surface、字体、shader、JSON、路径、数组或文件描述符。

本文把所有跨进程、跨语言、跨进程回调和公共 API 输入都视为不可信输入。
内部算法本身不一定需要 fuzz；只有当它被 IPC、Command、NDK、NAPI、回调或资源解码触达时，
才按安全输入面处理。

## 快速代码地图

| 领域 | 优先位置 |
| --- | --- |
| RS 客户端入口 | `rosen/modules/render_service_base/include/transaction/` |
| Client Proxy | `rosen/modules/render_service_base/src/platform/ohos/transaction/zidl/` |
| Service Stub | `rosen/modules/render_service/main/render_server/transaction/zidl/` |
| Render 连接 | `rosen/modules/render_service/main/render_process/transaction/zidl/` |
| RSCommand | `rosen/modules/render_service_base/include/command/` |
| Transaction | `rosen/modules/render_service_base/src/transaction/` |
| SAFuzz 配置 | `rosen/modules/safuzz/rs_ipc_dos_simulator/configs/test_case_config.json` |
| SAFuzz command | `rosen/modules/safuzz/rs_ipc_dos_simulator/command/` |
| SAFuzz transaction | `rosen/modules/safuzz/rs_ipc_dos_simulator/transaction/` |
| 普通 fuzz | `rosen/test/render_service/`、`rosen/test/2d_graphics/fuzztest/` |

## 安全输入模型

### 输入来源分层

1. **公共 API 输入**：应用通过 `RSInterfaces`、NDK、NAPI、CJ、ANI、Taihe 传入参数。
2. **IPC 输入**：Proxy 写入 `MessageParcel`，Stub 从 `MessageParcel` 读取。
3. **Transaction 输入**：`COMMIT_TRANSACTION` 携带 `RSTransactionData` 和 `RSCommand` 列表。
4. **回调输入**：WMS、HGM、VSync、屏幕、窗口动画等 callback 通过 Stub/Proxy 回到 RS。
5. **资源输入**：PixelMap、SurfaceBuffer、字体、shader、图片压缩数据、文件描述符、JSON。

安全判断从外到内做：公共入口校验 API 语义，Proxy/Stub 校验序列化协议，
Command 校验对象状态，资源解析校验大小、范围、生命周期和失败回滚。

### 基本不变量

- Proxy 写入顺序必须和 Stub 读取顺序一致；新增字段要说明旧端如何处理缺省值。
- 每个 `Read*`、`ReadParcelable`、`ReadRemoteObject`、`ReadFileDescriptor` 失败都要立即返回。
- enum、flag、mode、size、count、index、ratio、rect、matrix、range 不能只依赖类型安全。
- NodeId、ScreenId、SurfaceId、pid、uid 只能作为索引，不能作为权限或对象有效性的证明。
- 对象创建、缓存插入、引用计数增加、注册回调等副作用必须在全部参数校验后执行。
- 同一请求失败时必须保持旧状态，不留下半注册 callback、半创建 node、半提交 buffer。

## IPC / Parcel 设计检查

### Proxy 写入侧

新增或修改 IPC 接口时，先定位 Proxy 写入函数，记录以下字段：

- 是否写 `WriteInterfaceToken()`，descriptor 是否和 Stub 一致。
- `MessageOption` 是 `TF_SYNC` 还是 `TF_ASYNC`。
- 字段顺序、字段类型、数组长度、对象是否允许为空。
- `RemoteObject`、`Surface`、`PixelMap`、`SyncFence`、fd 的所有权和关闭责任。
- `SendRequest` 失败、reply 格式异常、业务返回码失败时的返回值和日志。

Proxy 侧只是第一层防线。即使 Proxy 当前不会写出非法值，Stub 侧仍然要按不可信输入处理，
因为 fuzz、老版本客户端、异常进程或其它语言绑定可能绕过当前 Proxy 假设。

### Stub 读取侧

Stub 侧检查顺序建议固定为：

1. 校验 interface token。
2. 校验权限、调用方身份和接口开关。
3. 按 Proxy 顺序读取字段；任一读取失败立即返回。
4. 校验长度、范围、枚举、空指针、对象归属和资源大小。
5. 最后调用业务实现；业务实现不要再次依赖 parcel 原始状态。

需要特别关注的 Parcel 类型：

- `vector` / `map`：必须有最大长度或业务上限，避免 OOM 和超时。
- `Rect` / `Matrix` / `Region`：检查负尺寸、NaN、Inf、极大坐标和溢出。
- `sptr<Surface>` / `RemoteObject`：允许空时要有明确语义，不允许空时要早返回。
- `PixelMap` / 图片：检查尺寸、row bytes、格式、动态范围和内存上限。
- `fd` / `SyncFence`：明确等待超时、复制和关闭责任，不做无界等待。

## Command / Transaction 安全边界

`RSCommand` 通过 `COMMIT_TRANSACTION` 进入服务端，不能只看单个 command 的构造函数。
检查时按下面链路走：

```text
客户端 API / Modifier / Animation
  -> RSCommand 或 RSTransactionData
  -> COMMIT_TRANSACTION
  -> command unmarshalling
  -> 查找 RSRenderNode / RSContext 对象
  -> Apply / Process / Playback
```

### Command 参数约束

- `NodeId` 不存在、节点类型不匹配、节点已销毁时，应返回或忽略，不能解引用。
- 几何参数要处理负尺寸、极大值、NaN、Inf 和矩阵不可逆。
- 资源参数要处理空 PixelMap、空 SurfaceBuffer、fence 超时、缓存未命中。
- 动画和 modifier command 要处理属性不存在、重复 command、乱序 command、旧版本字段缺失。
- 同帧多条 command 作用同一节点时，要确认最终值、去重和 dirty 标记语义。

### Transaction 级风险

- `commandList` 数量、重复次数和单 command 参数大小共同决定耗时，不能只限制单字段。
- command unmarshalling 失败后，本次 transaction 不能继续使用半解析对象。
- command 可能跨线程生效；不要捕获临时对象、裸指针或没有生命周期保证的引用。
- 涉及 ABI 或旧数据兼容时，要保留默认值路径，并补旧字段/新字段混合输入用例。

## SAFuzz 覆盖策略

### IPC 接口 case

新增 IPC 接口时，在 `test_case_config.json` 增加对应 case。配置要和 Proxy 一致：

- `testCaseDesc`：使用 `<INTERFACE_NAME>_TEST_XXX`。
- `interfaceName`：接口枚举名或 simulator 中注册的接口名。
- `inputParams`：按 Proxy 写入顺序排列，例如 `WriteUint64;WriteInt32`。
- `writeInterfaceToken`：和 Proxy 是否写 token 保持一致。
- `messageOption`：同步接口用 `TF_SYNC`，异步接口用 `TF_ASYNC`。
- `connectionType`：跟同一连接路径的已有 case 保持一致。

如果缺少 writer helper，优先补 SAFuzz Parcel utility，不要把二进制细节塞进 JSON。

### Command case

新增或修改 `RSCommand` 时，至少补一个 `COMMIT_TRANSACTION` case：

```json
{
  "testCaseDesc": "COMMIT_TRANSACTION_TEST_XXX",
  "interfaceName": "COMMIT_TRANSACTION",
  "inputParams": "WriteRSTransactionData",
  "writeInterfaceToken": false,
  "messageOption": "TF_ASYNC",
  "commandList": "YourCommandName*100",
  "commandListRepeat": 1,
  "connectionType": 0
}
```

同时完成两处注册：

- 在 `rosen/modules/safuzz/rs_ipc_dos_simulator/command/` 对应 command family 中声明随机构造。
- 在 `rs_transaction_data_utils.cpp` 注册 command 名，确保 JSON 中的字符串完全一致。

### 随机数据原则

- 优先使用已有 `Uint64`、`Uint32`、`Int32`、`Bool`、`Float`、`String` 等 helper。
- 新增自定义 helper 时同时覆盖正常值、边界值、极值、空对象和读取失败。
- 对数组、路径、区域、图片、命令列表这类放大器，随机长度必须有上限。
- 对 fence、fd、SurfaceBuffer 这类系统资源，fuzz 中要覆盖空值、重复释放和等待失败。

## 普通 Fuzz / 单测协同

SAFuzz 覆盖 IPC 协议和 transaction simulator，不替代普通 fuzzer。
当功能存在直接 API 或 helper 入口时，还要补最近的普通 fuzz：

- `rsinterfaces*_fuzzer`：覆盖 `RSInterfaces` 客户端调用链。
- `rsrenderservice*stub_fuzzer`：覆盖 service stub 的 `OnRemoteRequest`。
- `render_service_base/fuzztest/*command*`：覆盖 command marshalling 和 apply 边界。
- `rosen/test/2d_graphics/fuzztest/`：覆盖 2D / NDK 输入和资源解析。

单测用于固定语义：权限失败、旧字段默认值、错误码、空对象、边界尺寸、重复 command、
回调注册/注销和资源释放顺序。fuzz 发现问题后必须补可重复单测。

## 修改检查清单

提交前逐项确认：

- Proxy 和 Stub 字段顺序一致，新增字段有兼容说明。
- 每个 Parcel 读取失败都有明确返回，不继续使用默认构造对象误当合法输入。
- 权限、token、调用方身份和 feature flag 的检查早于业务副作用。
- 数组长度、字符串长度、图片尺寸、command 数量和等待时间有上限。
- `commandList` 字符串和 SAFuzz 注册名完全一致。
- `test_case_config.json` 可通过 JSON 语法检查。
- 普通 fuzz、SAFuzz、单测各自覆盖了不同层，不用其中一个替代全部。
- 涉及公开 API、ABI、IPC 协议或错误码时，在 PR 中写清兼容影响和 XTS 预期。

## 验证建议

文档或路由修改只做静态检查。代码修改按影响面选择最近验证：

```sh
python3 -m json.tool rosen/modules/safuzz/rs_ipc_dos_simulator/configs/test_case_config.json >/dev/null
git diff --check
prebuilts/build-tools/linux-x86/bin/ninja -C out/<product-name> \
  //foundation/graphic/graphic_2d/rosen/modules/safuzz:safuzztest
prebuilts/build-tools/linux-x86/bin/ninja -C out/<product-name> <nearest_fuzz_or_unittest_target>
```

真实崩溃、权限绕过、fd/fence 泄漏、设备 buffer 或显示链路问题，
不能只用本地 fuzz 结论关闭。
需要补设备日志、复现输入、首个错误栈、影响版本和是否可被应用侧触达。
