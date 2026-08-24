# 渲染服务安全检视问题记录（2026-08-17）

检视范围：rosen/modules/render_service 截图与连接清理路径。
检视方式：静态代码走读，结合调用图与实际业务使用方式判断。
未做设备验证，结论以当前 master 调用图为准。

## 问题一：RSUiCaptureTaskParallel::Run 中 endNodeDrawable_（rs_ui_capture_task_parallel.cpp:367）

```cpp
if (HasEndNodeRect()) {
    if (endNodeDrawable_->GetRenderParams()->GetMatrix().Invert(invertMatrix)) {
```

- 疑问：`endNodeDrawable_` / `GetRenderParams()` 是否需要判空。
- 结论：**不需要，非缺陷**。
  依据：
  1. `endNodeDrawable_` 在 `CreateResources()`（:217-241）创建并判空，失败即 `return false`，
     `Capture()` 中止，`Run()` 不会被投递执行。
  2. `IsStartEndSameNode()`（:847）在 `CreateResources()` 之后才调用（Capture :164 与 :177 的顺序），
     `CreateResources` 执行时 `isStartEndNodeSame_` 恒为 false，走 endNode 路径必创建 drawable。
  3. `renderParams_` 在 `OnGenerate` 的 `InitRenderParams` 全分支 `make_unique` 创建、无置空路径
     （rs_render_node_drawable_adapter.cpp:158-184）。
- 残留风险：366 行条件（只判 `HasEndNodeRect()`）与 384 行（`HasEndNodeRect() && !isStartEndNodeSame_`）
  不对称。安全性依赖"IsStartEndSameNode 必须晚于 CreateResources"的隐式跨函数时序，
  若调用顺序被调整（把 :177 前移），同节点场景将变成空指针崩溃。
- 建议（加固，非必改）：366 行条件对齐 384 行，或补 `endNodeDrawable_ != nullptr` 防御。

## 问题二：RSSurfaceCaptureTaskParallel::AddBlur 中 outImage（rs_surface_capture_task_parallel.cpp:702-706）

```cpp
auto outImage = geRender->ApplyImageEffect(canvas, *mesaContainer, {...}, Drawing::SamplingOptions());
canvas.DrawImage(*outImage, 0, 0, Drawing::SamplingOptions());
```

- 疑问：`outImage` 是否需要判空。
- 结论：**需要，现存缺陷，建议必修**。
  依据：
  1. `GERender::ApplyImageEffect`（graphics_effect 仓 ge_render.cpp:64-79）循环内
     `resImage = geShaderFilter->ProcessImage(...)`（:147），直接承接滤镜返回值。
  2. `GEMESABlurShaderFilter::ProcessImage` 存在大量真实失败路径：
     ge_mesa_blur_shader_filter.cpp 多处 `return nullptr`（:289、:318、:335、:377、:401、:419），
     触发条件为 `MakeImage` 离屏分配失败（GPU/内存压力）等。
  3. 失败相关性：进入 MESA 分支的前提是 `ApplyHpsBlur` 已失败（:696），图形环境处于降级状态，
     后续 `MakeImage` 再失败的概率放大。
  4. API 自身调用契约：`GERender::DrawImageEffect` 调用后立即判空（ge_render.cpp:54-57）。
- 影响：`DrawImage(*outImage, ...)` 空解引用，发生在渲染线程（capture 任务），
  崩溃即 render service 系统进程死亡。
- 修复建议：判空后提前 return（与本函数 :689 对 image 的判空风格一致），并补 RS_LOGE。

## 问题三：RSSurfaceCaptureTaskParallel::ClearCacheImageByFreeze 中 surfaceNodeDrawable（:176-191）

```cpp
auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
std::function<void()> clearCacheTask = [id, surfaceNodeDrawable]() -> void {
    surfaceNodeDrawable->SetCacheImageByCapture(nullptr);
};
```

- 疑问：lambda 内 `surfaceNodeDrawable` 是否存在空指针解引用。
- 结论：**不存在，非缺陷**。
  依据（OnGenerate 在此上下文的全部返回路径）：
  1. node 为 null 返回 null——不可达，:179 已判空且走 surfaceNode 分支。
  2. GeneratorMap 无此类型返回 null（adapter.cpp:137-141）——不可达，SURFACE_NODE 由静态注册器
     `RSSurfaceRenderNodeDrawable::instance_`（rs_surface_render_node_drawable.cpp:92）在库加载时注册。
  3. 生成器路径返回 `new RSSurfaceRenderNodeDrawable(...)`（:148-152），OHOS 禁异常，
     new 失败直接 abort 而非返回空。
  4. 缓存命中路径返回 `lock()` 成功结果，非空有保证。
- 生命周期：lambda 按值捕获 shared_ptr，PostTask 异步期间对象保活，无悬空；
  `static_pointer_cast` 正确（RSSurfaceRenderNodeDrawable : public RSRenderNodeDrawable）。
- 备注：不作修改。

## 问题四：RSClientToRenderConnection::CleanAll 的 cleanDone_ TOCTOU（rs_client_to_render_connection.cpp:157-180）

```cpp
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (cleanDone_) { return; }                    // 检查：持锁
}                                                   // 锁释放，窗口打开
renderPipelineAgent_->Clean(remotePid_, false);     // 动作：不持锁
{
    std::lock_guard<std::mutex> lock(mutex_);
    cleanDone_ = true;                              // 置位：动作结束
}
```

- 疑问：`cleanDone_` 的写入与读取是否存在 TOCTOU 竞态。
- 结论：**存在，教科书式 check-then-act，低危（重复执行），非崩溃级**。
  依据：
  1. 竞态窗口覆盖整个 `Clean()` 执行期。`Clean()` 内部为
     `ScheduleMainThreadTask(...).wait()` 同步等待（rs_render_pipeline_agent.cpp:2213-2219）
     + 渲染线程同步任务，负载高时窗口可达数十毫秒。
  2. 并发调用方真实存在（不同线程可达同一对象）：
     - `RSConnectionDeathRecipient::OnRemoteDied`（:215），IPC 死亡通知线程，toDelete=true；
     - `REMOVE_CONNECTION` IPC（rs_connect_to_render_process_stub.cpp:97），IPC worker，toDelete=true；
     - `~RSClientToRenderConnection`（:143），任意释放最后引用的线程，toDelete=false。
  3. 典型场景：客户端进程死亡瞬间，死亡通知与 REMOVE_CONNECTION 并发进入，双双通过检查，
     `Clean()` 与 `RemoveConnection()` 各执行两次。
- 影响评估：`Clean()` 重活投递主线程任务队列串行执行，无数据竞争；`CleanResources` 语义幂等，
  重复执行主要造成冗余清理与性能抖动。保护意图（恰好一次）落空。
- 争议点记录：曾讨论"标志位需在 clean 结束后再置位"。分析结论：晚置位并无失败语义
  （`Clean()` 返回 void，内部 wait() 返回值被丢弃，失败也照样置位），也无调用方依赖
  false 状态重试（可重复清理走 `CleanForRefresh()`，完全绕开 cleanDone_）。
- 修复建议（暂不实施，已确认记录）：
  方案 A（推荐）：检查与置位在同一个临界区内完成，动作移出锁外——
  ```cpp
  {
      std::lock_guard<std::mutex> lock(mutex_);
      if (cleanDone_) { return; }
      cleanDone_ = true;   // 先占位再干活
  }
  renderPipelineAgent_->Clean(remotePid_, false);
  ```
  方案 B：`std::call_once`，后来者阻塞等待首个完成，满足"清理完成后标志位才生效"的严格语义。

## 汇总

| # | 位置 | 定性 | 处置 |
| --- | --- | --- | --- |
| 1 | rs_ui_capture_task_parallel.cpp:367 | 非缺陷（调用图保证非空） | 可选加固 |
| 2 | rs_surface_capture_task_parallel.cpp:706 | 现存缺陷（空解引用，渲染进程崩溃级） | 建议必修 |
| 3 | rs_surface_capture_task_parallel.cpp:187 | 非缺陷（调用图保证非空） | 不修改 |
| 4 | rs_client_to_render_connection.cpp:157-180 | TOCTOU 竞态（低危，重复执行） | 暂不修改，已记录 |
