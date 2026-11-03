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

## 补充背景

### MemAllocator 的内存回收与复用策略

- 线性布局：`startPtr_` + `capacity_` + `size_`，`ALIGN_SIZE=4` 对齐；`Allocate<T>` 用 placement new 在尾部构造。
- 扩容：`MEMORY_EXPANSION_FACTOR=2`，当 `capacity_ - size_ < sizeof(T)` 时 `Resize((capacity_ + sizeof(T)) * 2)`。
- 只读包装：`BuildFromData(data, size)` 直接引用外部缓冲（析构不释放），`BuildFromDataWithCopy` 拷贝副本。
- 回收：`ClearData()` 重置 `size_` 但保留缓冲（容量可复用），`Clear()` 释放底层内存；后续 `Add`/`Allocate` 复用既有缓冲追加。
- 偏移寻址：`AddrToOffset`/`OffsetToAddr` 支持序列化后按偏移定位 OpItem，替代裸指针。

### DrawCmdList 跨 IPC 序列化格式

- 自定义连续缓冲格式（非 TLV）：`opAllocator_` 存 OpItem 链表，`imageAllocator_`/`bitmapAllocator_` 存附加像素数据。
- 首部：前两个 `int32_t` 为 width + height（`offset_ = 2 * sizeof(int32_t)`），其后依次排列 OpItem。
- OpItem 布局：`OpItem` 基类（`type` + `nextOpItem_`）+ 子类 `ConstructorHandle`（定长 POD），靠 `nextOpItemOffset_` 链接；image/bitmap 通过 `OpDataHandle{offset,size}` 引用。
- Marshalling：`MarshallingDrawOps` 遍历 vector 调 `op->Marshalling(cmdList)` 写入 ConstructorHandle；Unmarshalling 由 `UnmarshallingDrawOps` 按链表偏移读 type，查表构造 DrawOpItem 存入 vector。
- 附加对象：ExtendImageObject/BaseObj/NineObject/LatticeObject、ExtendDrawFuncObj、RecordCmd、ExtendObject、DrawingObject、SurfaceBufferEntry 各自维护独立 vector，跨进程单独传输后由 `Setup*` 重建。
- 两种模式：IMMEDIATE 录制时直接写连续缓冲；DEFERRED 先存 vector，`MarshallingDrawOps` 时再写入缓冲，回放后 `ClearData` 回收 op/image/bitmap allocator 但保留首部。

### SurfaceBufferEntry 在录制回放中的使用场景

- 结构：`SurfaceBufferEntry{ surfaceBuffer_, acquireFence_ }`，仅 `ROSEN_OHOS` 下存在。
- 管理：`AddSurfaceBufferEntry`/`GetSurfaceBufferEntry`/`GetAllSurfaceBufferEntry`/`SetupSurfaceBufferEntry`，独立 vector + `surfaceBufferEntryMutex_`。
- 场景：服务于 `SURFACEBUFFER` 类 OpItem，承载外部 buffer + acquire fence 跨进程传递；回放时 OpItem 取出 buffer/fence 在真实 Canvas 上绘制并按 fence 同步。
- 设计意图：buffer/fence 是 Native 句柄，无法塞进定长 ConstructorHandle，故独立成 Entry 由 CmdList 统一管理生命周期。

### UnmarshallingHelper 的版本兼容性处理

- 注册表：`opUnmarshallingFuncLUT_`（type→func）+ `opUnmarshallingSize_`（type→size），`shared_mutex` 读写锁保护。
- 注册：`UNMARSHALLING_REGISTER(name, type, func, size)` 宏在编译期全局对象构造时 `Register` emplace，新增 OpItem 类型自动注册。
- 查找：`GetFuncAndSize(type)` 返回 `{func, size}`，未注册返回 `{nullptr, 0}`。
- 兼容：`UnmarshallingPlayer::Unmarshalling` 若 func 为 null 或 `availableSize < desirableSize` 则返回 nullptr，主循环按 `nextOpItemOffset_` 跳过该 op 继续，实现前向/后向兼容——旧版本不识别的新 op 类型被跳过而非崩溃。
- replay 容错：`isReplayMode` 下若 size 不足会 `malloc` 补齐后仍尝试反序列化（性能分析回放场景）。
- 大小校验：`DrawOpItem::GetOpSize = sizeof(DrawOpItem) + ConstructorHandle size`，用于链表遍历与越界检查。

### RecordCmd 嵌套录制的典型使用场景

- 结构：`RecordCmd{ drawCmdList_, cullRect_ }`，持有完整 DrawCmdList + cull rect；`Playback(Canvas*)` 在指定 Canvas 回放，受 cullRect 裁剪。
- 嵌套管理：`CmdList::AddRecordCmd` 存入 `recordCmdVec_`，跨进程独立传输后由 `SetupRecordCmd` 重建。
- 深度/数量限制：`RECORD_CMD_MAX_DEPTH=800`、`RECORD_CMD_MAX_SIZE=100000` 防止递归爆栈/内存耗尽。
- 典型场景：Picture 录制回放（`RecordingCanvas::DrawPicture` 把内层录制作为 RecordCmd 嵌入外层）、节点合成复用（一个节点录制引用另一个节点的录制结果）、SaveLayer 子序列封装复用。
- 设计意图：把可复用的子绘制序列封装为独立 RecordCmd，外层 CmdList 仅持引用，避免重复录制相同内容。

