# RSTransactionData::UnmarshallingCommand 专项检视报告（2026-08-19）

检视对象：rosen/modules/render_service_base/src/transaction/rs_transaction_data.cpp
`RSTransactionData::UnmarshallingCommand`（:361-466）及其消费链
（rs_unmarshal_thread.cpp RecvParcel / ReportTransactionDataStatistics /
rs_transaction_data.cpp Process）。

威胁模型：任意持 RS 连接的应用进程可构造 MessageParcel——commandSize、
isUniRender、每条命令的 hasCommand/type/subType/参数、尾部元数据均攻击者可控。
本函数是 render service 反序列化边界，检视聚焦恶意输入下的资源消耗与防御有效性。

结论速览：

| # | 问题 | 定性 | 处置 |
| --- | --- | --- | --- |
| 1 | hasCommand=0 空命令洪水绕过统计杀人阈值 | **缺陷（校验旁路 + 资源消耗，低-中危）** | 建议修复 |
| 2 | isUniRender=true + 真命令洪水的无界增长/OOM | 非缺陷（三层有界 + 防御有效） | 不修改 |
| 3 | IsCallingPidValid 事后校验，反序列化开销不可回收 | 非缺陷（架构固有代价 + 防御覆盖） | 不修改（可选纵深优化） |
| 4 | （用户后续补充，本报告预留） | — | — |

## 问题 1：hasCommand=0 空命令洪水绕过统计阈值（缺陷）

```cpp
if (hasCommand) {                          // :411
    ... // 反序列化 + emplace
} else {
    continue;                              // :438-439 空条目直接跳过，payload_ 不增长
}
```

- 疑问：攻击者全部设置 hasCommand=0，payload_ 恒空，
  `ReportTransactionDataStatistics` 的 opCount 恒 0，
  ALARM(10000)/KILL(20000) 阈值永不触发，循环 CPU 开销无限产生。
- 结论：**成立，缺陷**。攻击链各环节均确认：
  1. **绕过点**：空条目分支不 emplace、不调工厂、无任何校验；统计计数来源
     `GetCommandCount()` = `payload_.size()`（恒 0）+ payload 内 CmdList op 数
     （空 payload 无可加）→ `transactionDataStatistics_[pid]` 每次加 0。
  2. **校验低估**：最小命令尺寸按 uni 模式 5 字节计（:383），但 hasCommand=0
     实际只消耗 1 字节——parcel 可容纳的空条目数是校验假设的 **5 倍**
     （4MB → ~400 万次迭代，而非 80 万）。
  3. **后续流程照常**：transData 非空（尾部元数据正常读入）→ 存入
     cachedTransactionDataMap_（:176）→ 主线程空 Process + vsync 唤醒
     （:178-187）。
- 不修改的风险：
  1. **防护机制被架空**：KillAppWithReason 永不触发、HiSysEvent 永不上报
     （EventWrite 需 totalCount ≥ 10000）——攻击无限持续且 DFX 无感知、
     事后无事件可查。
  2. **三路资源消耗**：CPU（unmarshal 线程空迭代，挤占正常应用事务处理）；
     binder 带宽与内存（每 parcel 4MB 传输）；主线程 vsync 唤醒风暴
     （每个空事务仍触发 RequestNextVSync）。
  3. **受害者与攻击者分离**：消耗的是 render service（系统进程），
     影响整机 UI 流畅度；攻击者自身进程无感。
  4. 缓解因素（如实）：单 parcel 杀伤毫秒级；binder 洪水可被系统侧限流
     部分缓解；属性能侵蚀型而非功能破坏型。
- 修复方向（多层任选）：
  - **最小修（推荐）**：统计口径改为 UnmarshallingCommand 实际处理的条目数
    （含 continue 的空条目）——函数内计数并透出（如 parsedCount_ 成员或
    返回值扩展），ReportTransactionDataStatistics 以实际解析条数计入；
  - 收紧校验：空条目按实际最小消耗（1 字节）计算 readableSize 上界，
    消除 5 倍超售（对纯空条目洪水仍需配合统计修复）；
  - 入口限频：对持续高频空事务的 pid 扩展现有 shouldDrop 节流机制。

## 问题 2：isUniRender=true 真命令洪水是否 OOM（非缺陷）

- 疑问：攻击者构造数百万 RSCommand new 对象无限制 emplace，是否无界增长 OOM。
- 结论：**不成立——增长有界，OOM 不可达，且统计阈值对该形态正常工作**。
  量化推导（uni 模式，选最小命令）：
  1. **parcel 硬上限**：PARCEL_MAX_CAPACITY 4MB（SetMaxCapacity :109）+
     marshalling 侧 PARCEL_SPLIT_THRESHOLD 1800KB 拆包——单笔事务命令数被
     字节数封死。最小命令如 RSRootNodeSetEnableRender（NodeId 8 + bool 1）
     = 每条 14 字节 parcel → 单 parcel ~30 万条；每命令堆对象 ~64B
     → 单 parcel 堆开销 ~19MB，有界。
  2. **payload_ 生命周期短**：unmarshal → cachedTransactionDataMap_（:176）→
     主线程 Process 消费 → 整体释放。堆积窗口 ≈ 单帧 vsync 间隔（~16ms），
     攻击者要在该窗口内灌 GB 级数据需数万 parcel/帧，被 binder 单进程
     带宽限制。
  3. **统计阈值有效（与问题 1 的关键差异）**：hasCommand=1 时 payload_.size()
     真实增长 → 每 parcel +30 万 → 第二发即破 KILL_COUNT(20000) →
     KillAppWithReason 杀掉攻击进程（isNonSystemAppCalling 且
     terminateEnabled 时）。
- 残余：terminateEnabled 关闭或系统调用路径时杀人不触发，只剩 ~19MB/parcel
  有界消耗 + EventWrite 告警，仍无 OOM；主线程 Process 30 万条无效命令
  数毫秒（单帧卡顿级）。
- 机制分界沉淀：**统计阈值防"真命令洪水"（有效），防不了"空命令洪水"
  （失明）**——修复问题 1 时应保持对真命令的计数语义不变。

## 问题 3：IsCallingPidValid 事后校验的开销不可回收（非缺陷）

- 疑问：授权检查在 ParseTransactionData 之后运行（只打标
  SetCallingPidValid(false)，不释放对象），Process 时 continue 跳过——
  反序列化阶段的开销已不可回收。
- 结论：**现象属实，但属 IPC 边界固有代价而非可修缺陷**：
  1. **校验对象在反序列化后才存在**：IsCallingPidValid 检查
     command->GetAllNodeIds()——命令内容必须先解出来才能校验，
     "不反序列化就校验"在信息论上不成立。
  2. **理论优化的收益有限**：把校验搬进 UnmarshallingCommand 循环内
     （每解出一条立即校验，无效当场 delete 不入 payload_）可省无效命令的
     存储与 Process 遍历，但反序列化本身（开销大头）不可省，约省一半。
  3. **既有防御覆盖该攻击面**：跨 pid 命令洪水 payload_.size() 真实计数
     （同问题 2 推导）→ ~2 发 parcel 即触发杀人——攻击上限 ≈ 2 个 parcel
     的 unmarshal 开销 + 一次 KillAppWithReason 流程。
- 与问题 1 的对照：

  | 攻击形态 | 统计计数 | 杀人触发 | 持续性 |
  | --- | --- | --- | --- |
  | 空命令洪水（问题 1） | 恒 0 | 永不（失明） | 无限 |
  | 跨 pid 命令洪水（本问） | 真实增长 | ~2 发即杀 | 仅 2 发 |

- 处置：不修改。可选纵深（性能优化性质）：循环内逐条校验 + 无效即弃，
  省内存与主线程遍历；非安全修复。

## 问题 4：（预留）

待用户补充第四个问题后填写。

## 附：与既有结论的关系

- 本报告问题 1 即总报告待修项之一（原 #38 位置，编号以最终汇总为准）；
  问题 2、3 为非缺陷项，论证过程可作为后续同类"洪水/OOM"疑虑的速查模板：
  单笔有界（parcel 上限）× 生命周期短（帧级消费）× 统计有效（真计数触发
  杀人）三问定性。
