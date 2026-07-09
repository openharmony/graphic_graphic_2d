# Buffer Reclaim / Memory Info Manager 知识路由

## 适用场景

改动涉及以下任一内容时，必须先读本文：

- SurfaceBuffer 主动回收/恢复机制、UI 截图节点（`AddUICaptureNode` / `RemoveUICaptureNode` / `CheckSameProcessUICaptureNode`）。
- `SetDmabufInfo`、dma-buf 状态上报、`SURFACE_ON_TREE` / `IMAGE_OFF_TREE` 等 `DmabufRsInfo` 枚举。
- 节点上下树 (onTree / offTree)、Root 节点状态变化、`MemoryTrack` 的 `GetGlobalRootNodeStatusChangeFlag` / `GetNodeOnTreeStatus`。
- `RSBufferReclaim`、`RSMemoryInfoManager`、`BufferReclaimParam` 的初始化或新增接口。
  
  
## 模块定位

本文覆盖 `render_service_base` 中的两个独立但相关的特性：

- **Buffer Reclaim**：离树 SurfaceNode 的 lastBuffer 主动回收/恢复机制，编译开关 `BufferReclaimParam::IsBufferReclaimEnable()`，目前**仅对 RosenWeb 节点启用**（见下方"运行链路"）。
- **Memory Info Manager**：把 RS 侧 dma-buf 的"在树上/树下/Root"状态上报给内核 dma-buf reclaim 子系统，编译宏 `RS_MEMORY_INFO_MANAGER`，运行时再受 `RSSystemProperties::GetRSMemoryInfoManagerParam()` 二次控制。


## 目录结构

```text
rosen/modules/
├── render_service_base/
│   ├── include/
│   │   ├── feature/
│   │   │   ├── buffer_reclaim/
│   │   │   │   └── rs_buffer_reclaim.h                # RSBufferReclaim 单例
│   │   │   └── memory_info_manager/
│   │   │       └── rs_memory_info_manager.h           # RSMemoryInfoManager 全静态
│   │   └── memory/
│   │       └── rs_memory_track.h                      # MemoryTrack 单例（被 MemoryInfoManager 复用）
│   └── src/
│       ├── feature/
│       │   ├── buffer_reclaim/rs_buffer_reclaim.cpp
│       │   └── memory_info_manager/rs_memory_info_manager.cpp
│       ├── memory/rs_memory_track.cpp
│       └── pipeline/
│           ├── rs_surface_handler.cpp                 # 真正执行 ReclaimLastBufferProcess / TryResumeLastBuffer
│           ├── rs_render_node.cpp                     # SetIsOnTheTree 触发 RecordNodeOnTreeStatus
│           └── rs_surface_render_node.cpp             # SetSurfaceMemoryInfo 调用点
├── render_service/
│   ├── core/
│   │   └── feature_cfg/
│   │       ├── feature_param/performance_feature/
│   │       │   └── buffer_reclaim_param.{h,cpp}       # BufferReclaimParam 单例（开关/参数）
│   │       └── xml_parser/performance_feature_parser/
│   │           └── buffer_reclaim_param_parse.{h,cpp} # XML 解析回收参数
│   └── ...
└── render_service_base/src/render/rs_image_base.cpp   # SetImageMemoryInfo 调用点（图片上传/销毁）
```

## 运行链路

### Buffer 回收的每帧触发

入口是 `RSMainThread::PostTryReclaimLastBuffer`（`rs_main_thread.cpp:1965-1989`），由 `ConsumeAndUpdateAllNodes` 内层 lambda 逐节点调用（`rs_main_thread.cpp:2023`），后者由主线程 `mainLoop_` **每帧** 触发。计数器 `curFrameBufferReclaimCount_` 在每帧开头清零（`rs_main_thread.cpp:1999`）。

**触发条件（按代码顺序，所有条件都满足才进入实际回收）：**

1. **功能开关 + RosenWeb 限定**：
   `!BufferReclaimParam::GetInstance().IsBufferReclaimEnable() || !surfaceNode->IsRosenWeb()`
   （`rs_main_thread.cpp:1968`）。即：开关关闭，**或节点不是 RosenWeb**，直接 return。
   > 注意：当前规格下**只有 RosenWeb 节点**才会进入回收流程。这是规格限定，不是 bug。
2. **单帧回收上限**：`curFrameBufferReclaimCount_ >= MAX_BUFFER_RECLAIM_NUMS_IN_SINGLE_FRAME`
   （常量 `1`，`rs_main_thread.cpp:270`）则跳过。
3. **节点状态**：`!surfaceNode->IsOnTheTree() && surfaceHandler->IsNeedSwapLastBuffer()`
   （`rs_main_thread.cpp:1976`），即节点离树且 handler 有可回收的 lastBuffer。
4. **UI 截图豁免**：同进程存在 UI 截图节点时（`RSBufferReclaim::CheckSameProcessUICaptureNode`，`rs_main_thread.cpp:1977`）调用 `surfaceHandler->ResetLastBufferInfo()` 跳过，因为截图需要访问 buffer 数据。
5. **延迟触发**：`surfaceHandler->ReclaimLastBufferPrepare()`（`rs_main_thread.cpp:1981`）成功后递增 `curFrameBufferReclaimCount_`，再 `PostTask` 投递 `surfaceHandler->ReclaimLastBufferProcess()`（`rs_main_thread.cpp:1984`）到主线程，最终落到 `RSBufferReclaim::DoBufferReclaim`。

**`RSSurfaceHandler` 内部的两阶段：**

- `ReclaimLastBufferPrepare`（`rs_surface_handler.cpp:260-264`）：内部 `++lastBufferReclaimNum_`，达到 `MIN_RENDER_FRAMES_AFTER_CLEAN_CACHE` 才返回 true。**防止 cache clean 后立即回收导致掉帧**。
- `ReclaimLastBufferProcess`（`rs_surface_handler.cpp:266-293`）：实际执行。先 `RSBufferReclaim::CheckBufferReclaim`，再 `DoBufferReclaim`，最后清零 `lastBufferReclaimNum_` 和 `lastBufferId_`。

### Buffer 恢复的两个入口

buffer 一旦被回收，可能在两处被恢复：

1. **常规恢复**：`RSSurfaceHandler::TryResumeLastBuffer`（`rs_surface_handler.cpp:295-314`），检查 `buffer->IsReclaimed()` 后调 `RSBufferReclaim::DoBufferResume`。
2. **渲染时按需恢复**：`RSSurfaceRenderNodeDrawable`（`rs_surface_render_node_drawable.cpp:209`）在渲染前主动调 `DoBufferResume`。**第二入口是文档常被遗漏的路径**，分析"为什么被回收的 buffer 还能用"时必须考虑。


### 关键开关与编译宏

| 名称 | 类型 | 默认/控制 | 位置 |
| --- | --- | --- | --- |
| `BufferReclaimParam::IsBufferReclaimEnable()` | 运行时开关 | XML 解析 | `render_service/core/feature_cfg/feature_param/performance_feature/buffer_reclaim_param.h` |
| `RSSystemProperties::GetRSMemoryInfoManagerParam()` | 运行时开关 | 系统属性 | `render_service_base` 平台层 |
| `RSSystemProperties::GetClosePixelMapFdEnabled()` | 运行时开关 | 影响 Image 链路 key 选择 | 同上 |
| `RS_MEMORY_INFO_MANAGER` | 编译宏 | 控制 `MemoryTrack` 节点状态接口和 `MemoryInfo` 字段 | `rs_memory_track.h:40-48, 58-61, 101-106, 142-144` |


## 决策树检查点

修改 Buffer 回收逻辑时，按下面顺序定位，不要只看 `DoBufferReclaim`：

```text
每帧 mainLoop_ -> ConsumeAndUpdateAllNodes
  -> PostTryReclaimLastBuffer (rs_main_thread.cpp:1965)
     ├─ BufferReclaimParam::IsBufferReclaimEnable() 开关
     ├─ surfaceNode->IsRosenWeb() 规格限定
     ├─ curFrameBufferReclaimCount_ 单帧上限
     ├─ !IsOnTheTree() && IsNeedSwapLastBuffer() 节点状态
     └─ CheckSameProcessUICaptureNode UI 截图豁免
  -> RSSurfaceHandler::ReclaimLastBufferPrepare  (延迟 MIN_RENDER_FRAMES_AFTER_CLEAN_CACHE 帧)
  -> PostTask -> RSSurfaceHandler::ReclaimLastBufferProcess
     ├─ RSBufferReclaim::CheckBufferReclaim (全局硬上限 50)
     └─ RSBufferReclaim::DoBufferReclaim
        └─ SurfaceBuffer::TryReclaim (内核 dma-buf reclaim)
           └─ RegisterBufferDestructorCallBack (buffer 析构时回 RemoveBufferReclaim)
```

决策树检查点：


1. **新增豁免场景**：要在 `PostTryReclaimLastBuffer` 还是 `RSSurfaceHandler::ReclaimLastBufferProcess` 加判定？前者影响"是否触发"，后者影响"是否真正执行"。
2. **新增恢复触发点**：除了 `RSSurfaceHandler::TryResumeLastBuffer` 和 `RSSurfaceRenderNodeDrawable`，新加的渲染/截图/动画路径是否也需要主动 `DoBufferResume`？
3. **调整阈值**：`MIN_RENDER_FRAMES_AFTER_CLEAN_CACHE`（防掉帧延迟）和 `MAX_BUFFER_RECLAIM_NUMS`（全局硬上限 50）的语义不同，不要混改。
4. **新增上报字段**：要回答字段来源（渲染端/服务端/IPC）、更新频率（每帧/事件/配置变化）、缺省值对旧进程是否安全、同步失败能否恢复。

## 同步边界

### Buffer 回收：handler 与 manager 的分工

- `RSSurfaceHandler` 持有 `buffer_.buffer`（`sptr<SurfaceBuffer>`）和 `lastBufferId_`，是**状态的真正持有者**。
- `RSBufferReclaim` 只持有 `bufferReclaimNumsSet_`（`set<uint64_t>`）和 `uiCaptureNodeMap_`，是**全局协调者**，不直接持有 buffer 对象。

### Memory 上报：onTree / rootNode 的两层判定

- 第一层：节点是否在树上（`onTree` 参数 / `MemoryTrack::GetNodeOnTreeStatus`）。
- 第二层：是否为 Root 节点变化触发（`MemoryTrack::GetGlobalRootNodeStatusChangeFlag`）。
- 两层组合产生 4 种 `DmabufRsInfo` 枚举，**每次调用只命中一种**，不存在"一次调用同时下发多个枚举"。

## 优先查看位置

- **新增 buffer 回收豁免场景**：
  看 `render_service/core/pipeline/main_thread/rs_main_thread.cpp` 的 `PostTryReclaimLastBuffer`（`1965-1989` 行）。
- **调整回收阈值/延迟**：
  看 `render_service_base/src/pipeline/rs_surface_handler.cpp` 的 `ReclaimLastBufferPrepare`（`MIN_RENDER_FRAMES_AFTER_CLEAN_CACHE`）和 `rs_buffer_reclaim.cpp:23` 的 `MAX_BUFFER_RECLAIM_NUMS`。
- **新增 buffer 恢复路径**：
  现有两条：`rs_surface_handler.cpp:295` 和 `rs_surface_render_node_drawable.cpp:209`。新增前先确认是否真的需要第三条。
- **调整单帧回收上限**：
  看 `rs_main_thread.cpp:270` 的 `MAX_BUFFER_RECLAIM_NUMS_IN_SINGLE_FRAME` 和 `:1999` 的清零位置。
- **修改 XML 配置项**：
  看 `render_service/core/feature_cfg/xml_parser/performance_feature_parser/buffer_reclaim_param_parse.cpp` 和 `feature_param/performance_feature/buffer_reclaim_param.cpp`。
- **新增上报枚举**：
  枚举定义在外部头文件（`Memory::DmabufRsInfo`，来自 `mem_mgr_client`，不在本仓库）。本仓库只能在 `rs_memory_info_manager.cpp:36-81` 增加分支。
- **新增 dma-buf 上报来源**：
  现有 Surface 和 Image 两条。新加来源要在 `rs_memory_info_manager.cpp` 实现新的 `SetXxxMemoryInfo` 静态方法，并在对应节点生命周期处调用。
- **修改节点状态查询逻辑**：
  看 `render_service_base/include/memory/rs_memory_track.h` 和 `src/memory/rs_memory_track.cpp`。注意相关接口受 `RS_MEMORY_INFO_MANAGER` 宏控制。
- **新增测试**：
  看 `rosen/test/render_service/render_service_base/unittest/feature/buffer_reclaim/rs_buffer_reclaim_test.cpp` 和 `memory_info_manager/rs_memory_info_manager_test.cpp`。

## 关键类型地图

- `RSBufferReclaim`：
  位置 `render_service_base/include/feature/buffer_reclaim/rs_buffer_reclaim.h`。
  单例（`GetInstance()`），管理 buffer 回收集合和 UI 截图节点映射。两把锁分别保护两组数据。
- `RSMemoryInfoManager`：
  位置 `render_service_base/include/feature/memory_info_manager/rs_memory_info_manager.h`。
  全静态方法，无需实例化。所有方法都先查 `GetRSMemoryInfoManagerParam()` 开关。
- `MemoryTrack`：
  位置 `render_service_base/include/memory/rs_memory_track.h`。
  单例（`Instance()`），原属于内存统计模块。`RS_MEMORY_INFO_MANAGER` 宏开启时额外提供 `globalRootNodeStatusChangeFlag`（`std::atomic<bool>`）和节点状态查询接口。
- `BufferReclaimParam`：
  位置 `render_service/core/feature_cfg/feature_param/performance_feature/buffer_reclaim_param.h`。
  单例，提供 `IsBufferReclaimEnable()` / `SetBufferReclaimEnable()`。
- `BufferReclaimParamParse`：
  位置 `render_service/core/feature_cfg/xml_parser/performance_feature_parser/buffer_reclaim_param_parse.h`。
  从 XML 解析回收参数到 `BufferReclaimParam`。
- `RSSurfaceHandler`：
  位置 `render_service_base/include/pipeline/rs_surface_handler.h`。
  buffer 持有方。`ReclaimLastBufferPrepare` / `ReclaimLastBufferProcess` / `TryResumeLastBuffer` 是回收/恢复的真正执行点。
- `RSSurfaceRenderNodeDrawable`：
  位置 `render_service/core/drawable/rs_surface_render_node_drawable.cpp:209`。
  渲染时主动 `DoBufferResume` 的第二入口。
- `SurfaceBuffer`（外部，不在本仓库）：
  提供 `TryReclaim()` / `TryResumeIfNeeded()` / `IsReclaimed()` / `RegisterBufferDestructorCallBack()` 接口。
- `MemMgrClient`（外部，不在本仓库）：
  提供 `SetDmabufInfo(fd, DmabufRsInfo)` 接口，来自 `mem_mgr_client.h`。


## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| Buffer 回收按 ID 跟踪 | `bufferReclaimNumsSet_` `std::set<uint64_t>` | 快速查找 buffer 是否已被回收，避免重复回收或恢复失败 |
| UI 截图节点按进程分组 | `uiCaptureNodeMap_` `map<uint32_t, map<NodeId, uint32_t>>` | 截图节点影响 buffer 回收，按进程分组便于进程退出时批量清理 |
| 内存信息通过静态方法 | `RSMemoryInfoManager` 全静态 | 内存信息是全局统计，无需实例化 |
| 节点上下树状态跟踪 | `RecordNodeOnTreeStatus` | Surface 在树上/树下状态影响内存统计，需要准确跟踪 |
| Buffer 析构回调 | `BufferDestructorCallBack` | Buffer 被系统回收时需要通知，回调机制保证及时清理 |
| 两把锁分离 | `mutex_` + `uiCaptureNodeMapMutex_` | buffer 回收集合和截图节点映射独立加锁，避免互锁 |
| Root 状态用 `atomic<bool>` | `MemoryTrack::globalRootNodeStatusChangeFlag` | 高频读（每次 Surface/Image 上报都查），用原子变量避免锁开销 |
| `DoBufferReclaim` 失败回滚 | `rs_buffer_reclaim.cpp:55` | `TryReclaim` 失败时 erase 刚插入的 id，保持 set 与实际状态一致 |
| `DoBufferResume` 失败重插 | `rs_buffer_reclaim.cpp:76` | `TryResumeIfNeeded` 失败时把 id 重新插回 set，保持"set 中的 id 一定处于已回收状态"的不变量 |
| 仅 RosenWeb 启用回收 | `rs_main_thread.cpp:1968` | Web 节点的 buffer 占用大且离树后短期不再用，性价比最高 |

## Buffer 回收与 SurfaceBuffer 生命周期的交互

`TryReclaim` / `TryResumeIfNeeded` 是纯内核层面的换出/换入操作，与 SurfaceBuffer 的对象生命周期完全解耦：

1. **不涉及引用计数**：`TryReclaim` 只调 `reclaimFunc_(pid,fd)`，然后设置 `isReclaimed_` 标志。不调用 `DecRef/UnRef`，不影响 sptr 计数。
2. **不释放 fd**：`handle_->fd` 在 reclaim/resume 前后使用有效，BufferHandle 不被释放。
3. **BufferDestructorCallBack 仅清理 RSBufferReclaim 自身记录**：buffer 析构时回调 `RemoveBufferReclaim(bufferId)`，只是从 `bufferReclaimNumsSet_` 中移除 ID，不释放 buffer 本身。
4. **两条独立路径**：
   - **对象生命周期**：由 sptr/RefBase 管理，fd 在 BufferHandle 析构时关闭。
   - **页面回收**：由内核 DMA-BUF 子系统管理，reclaim 换出，resume 换入，用户态 fd 句柄保持不变。

关键点：即使 buffer 被内核换出，用户态的 fd 仍然有效，下次访问时内核会自动 swap-in。两者互不干扰。

## 内存信息上报的目标和格式

`SetDmabufInfo` 是 `RSMemoryInfoManager` 借助 `MemMgrClient` 向内核 dma-buf reclaim 子系统上报 RS 侧 dma-buf 状态的核心接口，本质是**去重 + 延迟标记**：

> - `SetDmabufInfo` 的调用点全仓唯一，只在 `rs_memory_info_manager.cpp:37/40/43/46/70/73/76/79`；`mem_mgr_client.h` 也只被 4 个文件 include（`rs_memory_info_manager.cpp:20` 等），`rs_buffer_reclaim.cpp` 不在其中——编译期看不到 `SetDmabufInfo` 符号。
> - `RSBufferReclaim::DoBufferReclaim` 走的是 `buffer->TryReclaim()`（`rs_buffer_reclaim.cpp:51`），`DoBufferResume` 走 `buffer->TryResumeIfNeeded()`（`rs_buffer_reclaim.cpp:72`），出口仅这两条，都不经 `SetDmabufInfo`。
> - `TryReclaim` 的实现位于 surface / BufferClient 子系统（不在本仓库），已确认其内部不存在通往 `SetDmabufInfo` 的暗线。
> - **语义区别**：`RSBufferReclaim` 是"动手回收"（让 buffer 内容被丢弃，释放内存，有 `TryResumeIfNeeded` 反向操作）；`SetDmabufInfo` 是"动嘴上报"（告知内核 buffer 在 RS 树上的归属，供内核做 swap 决策，无反向操作只能覆盖）。两者目的维度不同。

> 以下"去重 / 延迟标记"是 `Memory::MemMgrClient`（来自 `mem_mgr_client.h`，**不在本仓库内**）的内部实现细节，本仓库只作为调用方，无法直接看到对应代码。

1. **去重**：在用户态维护 fd -> DmabufRsInfo 的映射表，避免对同一个 fd 重复下发相同的 ioctl 调用。
2. **延迟标记**：当 dma-buf 对应的 RS 节点从树上移除（off-tree）时，清空映射并通知内核。
3. **最终目的**：让内核的 dma-buf reclaim 子系统感知每个 dma-buf 的当前状态，以便内核在内存回收时做出正确的 swap-out / swap-in 决策。

### Surface 侧调用链路

入口是 `RSMemoryInfoManager::SetSurfaceMemoryInfo(onTree, handler)`（`rs_memory_info_manager.cpp:26-49`）：

```text
SetSurfaceMemoryInfo(onTree, handler)
  -> handler->GetBuffer() -> SurfaceBuffer
  -> SurfaceBuffer::GetFileDescriptor() 获取 fd
  -> 根据 onTree 与 MemoryTrack::GetGlobalRootNodeStatusChangeFlag() 组合挑选四种枚举之一：
       SURFACE_ON_TREE / SURFACE_OFF_TREE / SURFACE_ON_TREE_IN_ROOT / SURFACE_OFF_TREE_IN_ROOT
  -> MemMgrClient.SetDmabufInfo(fd, 选定枚举)
  -> 内核 dma-buf reclaim 子系统
```

### Image 侧调用链路

入口是 `RSMemoryInfoManager::SetImageMemoryInfo(pixelMap)`（`rs_memory_info_manager.cpp:51-82`），仅覆盖 `AllocatorType::DMA_ALLOC` 的 PixelMap：

```text
PixelMap(AllocatorType::DMA_ALLOC)
  -> MemoryTrack::Instance().GetNodeOnTreeStatus(GetPixels() 或 GetFd())  // 受 GetClosePixelMapFdEnabled() 开关影响，决定用 GetPixels() 还是 GetFd() 作为 key
  -> reinterpret_cast<SurfaceBuffer*>(pixelMap->GetFd())  // DMA 分配时 GetFd() 实际存放的是 SurfaceBuffer* 地址，按约定强转回来
  -> SurfaceBuffer::GetFileDescriptor() 获取 fd
  -> 根据 GetNodeOnTreeStatus 返回值挑选四种枚举之一：
       IMAGE_ON_TREE / IMAGE_OFF_TREE / IMAGE_ON_TREE_IN_ROOT / IMAGE_OFF_TREE_IN_ROOT
  -> MemMgrClient.SetDmabufInfo(fd, 选定枚举)
  -> 内核 dma-buf reclaim 子系统
```

### DmabufRsInfo 状态枚举

> 枚举定义位于外部头文件（`Memory::DmabufRsInfo`，来自 `mem_mgr_client`），本仓库只使用不定义。

| 枚举值 | 含义 |
| --- | --- |
| `SURFACE_ON_TREE` | Surface Buffer 在树上 |
| `SURFACE_OFF_TREE` | Surface Buffer 在树下 |
| `SURFACE_ON_TREE_IN_ROOT` | Surface Buffer 在树上且属于 Root 节点 |
| `SURFACE_OFF_TREE_IN_ROOT` | Surface Buffer 在树下且属于 Root 节点 |
| `IMAGE_ON_TREE` | IMAGE(DMA) 在树上 |
| `IMAGE_OFF_TREE` | IMAGE(DMA) 在树下 |
| `IMAGE_ON_TREE_IN_ROOT` | IMAGE(DMA) 在树上且属于 Root 节点 |
| `IMAGE_OFF_TREE_IN_ROOT` | IMAGE(DMA) 在树下且属于 Root 节点 |

