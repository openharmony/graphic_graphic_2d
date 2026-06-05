# Buffer Reclaim / Memory Info Manager

## 适用范围

- Buffer 回收机制
- 内存信息管理
- Surface 内存统计
- Image 内存统计
- 节点上下树状态跟踪

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSBufferReclaim | `rosen/modules/render_service_base/include/feature/buffer_reclaim/rs_buffer_reclaim.h` | Buffer 回收管理器 |
| RSMemoryInfoManager | `rosen/modules/render_service_base/include/feature/memory_info_manager/rs_memory_info_manager.h` | 内存信息管理器 |

## 核心模型

### Buffer Reclaim

`RSBufferReclaim` 为单例，管理 SurfaceBuffer 的回收和恢复：

1. **Buffer 回收**：`DoBufferReclaim(buffer)` 执行 buffer 回收
2. **Buffer 恢复**：`DoBufferResume(buffer)` 恢复已回收的 buffer
3. **回收检查**：`CheckBufferReclaim` 检查是否需要回收
4. **移除回收**：`RemoveBufferReclaim(bufferId)` 按 ID 移除回收记录

关键数据结构：

- `bufferReclaimNumsSet_`：`set<uint64_t>` 记录已回收 buffer 的 ID
- `BufferDestructorCallBack`：buffer 析构回调

### UI 截图节点管理

`RSBufferReclaim` 还管理 UI 截图节点：

- `AddUICaptureNode(nodeId)`：添加 UI 截图节点
- `RemoveUICaptureNode(nodeId)`：移除 UI 截图节点
- `CheckSameProcessUICaptureNode(nodeId)`：检查同进程 UI 截图节点

关键数据：

- `uiCaptureNodeMap_`：`map<uint32_t, map<NodeId, uint32_t>>` 按进程分组的截图节点映射
- `uiCaptureNodeMapMutex_`：截图节点映射互斥锁

UI 截图节点会阻止对应 buffer 的回收，因为截图需要访问 buffer 数据。

### Memory Info Manager

`RSMemoryInfoManager` 提供静态方法管理内存信息：

1. **Surface 内存**：`SetSurfaceMemoryInfo(onTree, handler)` 设置 Surface 内存信息，根据节点是否在树上更新
2. **Image 内存**：`SetImageMemoryInfo(pixelMap)` 设置 Image 内存信息
3. **节点状态**：`RecordNodeOnTreeStatus(flag, nodeId, instanceRootNodeId)` 记录节点上下树状态
4. **根节点状态**：`ResetRootNodeStatusChangeFlag(nodeId, instanceRootNodeId)` 重置根节点状态变化标志

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| Buffer 回收按 ID 跟踪 | `bufferReclaimNumsSet_` set | 快速查找 buffer 是否已被回收，避免重复回收或恢复失败 |
| UI 截图节点按进程分组 | `uiCaptureNodeMap_` map of map | 截图节点影响 buffer 回收，按进程分组便于进程退出时批量清理 |
| 内存信息通过静态方法 | `RSMemoryInfoManager` 全静态 | 内存信息是全局统计，无需实例化 |
| 节点上下树状态跟踪 | `RecordNodeOnTreeStatus` | Surface 在树上/树下状态影响内存统计，需要准确跟踪 |
| Buffer 析构回调 | `BufferDestructorCallBack` | Buffer 被系统回收时需要通知，回调机制保证及时清理 |

## 待补充背景

- Buffer 回收的触发条件和策略（内存压力阈值等）
- `SurfaceHandler` 中内存信息的具体统计方式
- Image 内存统计的范围和精度
- Buffer 回收与 SurfaceBuffer 生命周期的交互
- 内存信息上报的目标和格式
