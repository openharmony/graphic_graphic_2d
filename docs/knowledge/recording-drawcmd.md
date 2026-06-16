# Recording / DrawCmd

## 适用范围

- RecordingCanvas 录制画布，记录绘制调用序列
- DrawCmdList / CmdList 绘制命令列表存储与回放
- OpItem / DrawOpItem 各类绘制操作项
- MemAllocator 命令内存分配
- RecordCmd 嵌套录制命令
- DrawCmd 反序列化与回放

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| recording_canvas.h | `rosen/modules/2d_graphics/include/recording/recording_canvas.h` | RecordingCanvas：继承 NoDrawCanvas，录制所有绘制调用 |
| cmd_list.h | `rosen/modules/2d_graphics/include/recording/cmd_list.h` | CmdList 基类：OpItem 链表、ImageData/BitmapData/扩展对象管理 |
| draw_cmd_list.h | `rosen/modules/2d_graphics/include/recording/draw_cmd_list.h` | DrawCmdList：CmdList 的绘制专用子类 |
| draw_cmd.h | `rosen/modules/2d_graphics/include/recording/draw_cmd.h` | DrawOpItem 及所有绘制操作项定义 |
| op_item.h | `rosen/modules/2d_graphics/include/recording/op_item.h` | OpItem 基类：类型、链表偏移 |
| mem_allocator.h | `rosen/modules/2d_graphics/include/recording/mem_allocator.h` | MemAllocator：线性内存分配器 |
| recording_handle.h | `rosen/modules/2d_graphics/include/recording/recording_handle.h` | OpDataHandle：数据引用句柄 |
| record_cmd.h | `rosen/modules/2d_graphics/include/recording/record_cmd.h` | RecordCmd：嵌套录制命令 |
| cmd_list_helper.h | `rosen/modules/2d_graphics/include/recording/cmd_list_helper.h` | CmdListHelper：辅助工具 |
| mask_cmd_list.h | `rosen/modules/2d_graphics/include/recording/mask_cmd_list.h` | MaskCmdList：遮罩专用命令列表 |

## 核心模型

**录制回放架构**：

```
RecordingCanvas (客户端绘制)
  → 重写所有 Draw* 方法
  → 为每次调用创建对应 DrawOpItem
  → 通过 CmdList::AddOp<T>(args...) 分配到 MemAllocator
  → DrawCmdList 持有完整 OpItem 链表

DrawCmdList (跨进程传输)
  → 序列化 OpItem 链表 + ImageData + BitmapData + 扩展对象
  → 通过 IPC 传输到服务端

服务端回放
  → DrawCmdList::Playback(Canvas&)
  → 遍历 OpItem 链表，每个 DrawOpItem 调用 Marshal+Playback
  → 在真实 Canvas 上重放绘制操作
```

**CmdList** 核心数据结构：
- `MemAllocator opAllocator_`：线性内存分配器，OpItem 连续存储
- OpItem 链表：每个 OpItem 通过 `nextOpItemOffset_` 链接下一个
- 附加数据管理：ImageData、BitmapData、ExtendObject、DrawingObject、SurfaceBufferEntry
- 线程安全：`std::recursive_mutex mutex_` 保护 AddOp
- 限制：`MAX_OPITEMSIZE = 170000`、`RECORD_CMD_MAX_DEPTH = 800`、`RECORD_CMD_MAX_SIZE = 100000`

**DrawOpItem 类型**（`draw_cmd.h` 中定义 50+ 种）：
- 几何：POINT、LINE、RECT、ROUND_RECT、ARC、OVAL、CIRCLE、PATH
- 图像：BITMAP、IMAGE、IMAGE_NINE、IMAGE_LATTICE、PIXELMAP、SURFACEBUFFER
- 文本：TEXT_BLOB、SYMBOL
- 裁剪：CLIP_RECT、CLIP_ROUND_RECT、CLIP_PATH、CLIP_REGION
- 变换：SET_MATRIX、CONCAT_MATRIX、TRANSLATE、SCALE、ROTATE
- 状态：SAVE、SAVE_LAYER、RESTORE、DISCARD
- 特殊：SHADOW、BACKGROUND、DRAW_FUNC、IMAGE_SNAPSHOT、VERTICES

**反序列化**：通过 `UnmarshallingHelper` 注册表 + `UNMARSHALLING_REGISTER` 宏自动注册各类型的反序列化函数。

数据流：RecordingCanvas 录制 → DrawCmdList 序列化 → IPC 传输 → 服务端反序列化 → Playback 回放到真实 Canvas。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 线性内存分配 | `MemAllocator` 连续分配 OpItem | 避免逐项堆分配，提高录制性能和缓存友好性 |
| OpItem 链表偏移 | `nextOpItemOffset_` 而非指针 | 支持序列化后跨进程使用，偏移量比指针安全 |
| 录制深度/大小限制 | `RECORD_CMD_MAX_DEPTH` / `RECORD_CMD_MAX_SIZE` | 防止恶意或异常录制导致内存耗尽 |
| ExtendImageObject 体系 | `ExtendImageObject`/`ExtendImageBaseObj`/`ExtendImageNineObject`/`ExtendImageLatticeObject` | 不同图片绘制场景需要不同的回放接口 |
| UNMARSHALLING_REGISTER 宏 | 编译期自动注册反序列化函数 | 新增 OpItem 类型时自动注册，减少遗漏 |
| NoDrawCanvas 作为基类 | `RecordingCanvas : public NoDrawCanvas` | 录制时不需要真实绘制，NoDrawCanvas 避免无意义的 GPU 操作 |

## 待补充背景

- MemAllocator 的内存回收与复用策略
- DrawCmdList 跨 IPC 序列化的完整格式（TLV 或自定义格式）
- SurfaceBufferEntry 在录制回放中的使用场景
- UnmarshallingHelper 的版本兼容性处理
- RecordCmd 嵌套录制的典型使用场景
