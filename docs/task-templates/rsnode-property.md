# 新增 RSNode 属性

适用于新增或扩展客户端节点属性、modifier 属性、服务端 render node 状态或 drawable 消费逻辑。

执行要点：

- 从客户端节点或 modifier 定义入手，确认默认值、动画语义和线程边界。
- 检查 `render_service_client`、`render_service_base`、服务端 render node 或 drawable。
- 确认属性同步路径是否经过 transaction/command，以及旧 transaction 数据是否有默认兼容行为。
- 小步复用附近日志、错误码、智能指针、锁和线程投递方式。

关注对象：

- `RSRenderNode`
- `RSSurfaceNode`
- `RSCanvasNode`
- `RSRenderModifier`

验证建议：

- 跑最近 command、marshalling 或 pipeline 单测。
- 涉及动画时补 animation 或 modifier 单测。
- 涉及 drawable 消费时补 drawable 或 render_service pipeline 验证。
- 无完整 OpenHarmony 根环境时，至少做路径/符号核对和 `git diff --check`。
