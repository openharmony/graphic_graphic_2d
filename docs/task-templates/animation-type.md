# 新增动画类型

适用于新增或扩展 Rosen animation 类型、动画参数、播放语义或服务端属性更新路径。

执行要点：

- 确认客户端 animation、modifier、transaction 和服务端属性更新路径。
- 明确默认值、动画插值语义、重复播放语义和跨线程生命周期。
- 检查 start、end、cancel、repeat、空节点、节点销毁和跨线程销毁边界。
- 复用附近 animation、modifier 和 transaction 的日志、错误码和测试夹具。

验证建议：

- 跑最近 animation 单测、`render_service_client` 单测或 `render_service_base` animation 单测。
- 涉及 transaction 编码或旧数据兼容时，补 marshalling/command 单测。
- 无完整 OpenHarmony 根环境时，至少做路径/符号核对和 `git diff --check`。
