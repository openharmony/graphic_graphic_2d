# 新增 RSInterface IPC 接口

适用于新增或变更 Render Service IPC 接口、proxy/stub、transaction command 或回调协议。

执行要点：

- 先改接口声明和 proxy/stub，再检查权限、参数校验、错误码和日志。
- Parcel 字段顺序必须与 proxy 写入顺序一致。
- 读取失败要有明确返回和日志，不要让异常输入继续进入服务端逻辑。
- 检查旧客户端或旧 transaction 数据是否需要默认兼容行为。

锚点：

- `rs_interfaces.h`
- `rs_client_to_service_connection_proxy.cpp`
- `rs_render_service_stub.cpp`
- `command/`
- `transaction/`

验证建议：

- 补 `rsinterfaces_*` 客户端 fuzzer。
- 补 SAFuzz 配置和 `rsrenderservice*stub` fuzzer。
- 跑最近 IPC fuzz、stub fuzz 或 marshalling 单测。
- 无完整 OpenHarmony 根环境时，至少做 proxy/stub 字段顺序核对、符号检索和 `git diff --check`。
