# 常见任务模板索引

本目录按任务通路维护可复用模板。
每个模板独立成文，避免根 `AGENTS.md` 随通路增加而膨胀。

| 场景 | 模板 |
| --- | --- |
| 新增 RSNode 属性 | `rsnode-property.md` |
| 新增 RSInterface IPC 接口 | `rsinterface-ipc.md` |
| 新增动画类型 | `animation-type.md` |
| 新增 NAPI、NDK、CJ、ANI 或 Taihe 公开 API | `public-api-binding.md` |
| 新增 `BUILD.gn` target | `build-gn-target.md` |

新增模板时：

- 文件名使用短横线小写英文，格式为 `docs/task-templates/<topic>.md`。
- 先写适用场景，再写实现、同步、验证和门禁。
- 更新本索引；根 `AGENTS.md` 不再维护任务模板入口表。
- 若模板依赖具体模块知识，同时补充或引用 `docs/knowledge/` 中的对应文档。
