# Animation 与窗口动画

## 适用范围

- RSAnimation 客户端动画体系：属性动画、弹簧动画、关键帧动画、路径动画
- RSRenderAnimation 服务端动画估值
- 隐式动画与交互式动画
- Transition 过渡效果
- 粒子动画
- 窗口动画（RSWindowAnimation）IPC 体系
- 动画帧率协商（FrameRateRange）

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| 客户端动画基类 | `rosen/modules/render_service_client/core/animation/rs_animation.h` | RSAnimation：Start/Pause/Resume/Finish/Reverse |
| 属性动画 | `rs_property_animation.h` | RSPropertyAnimation<T>：属性值动画 |
| 曲线动画 | `rs_curve_animation.h` | RSCurveAnimation<T>：曲线插值 |
| 弹簧动画 | `rs_spring_animation.h` | RSSpringAnimation：弹簧物理模型 |
| 插值弹簧动画 | `rs_interpolating_spring_animation.h` | RSInterpolatingSpringAnimation |
| 关键帧动画 | `rs_keyframe_animation.h` | RSKeyframeAnimation：多关键帧 |
| 路径动画 | `rs_path_animation.h` | RSPathAnimation：沿路径运动 |
| Symbol 动画 | `rs_symbol_animation.h` | RSSymbolAnimation：符号动画 |
| 过渡效果 | `rs_transition.h` / `rs_transition_effect.h` | RSTransition / RSTransitionEffect |
| 粒子参数 | `rs_particle_params.h` | ParticleParams：粒子效果参数 |
| 运动路径选项 | `rs_motion_path_option.h` | RSMotionPathOption |
| 隐式动画 | `rs_implicit_animator.h` / `rs_implicit_animation_param.h` | 隐式动画管理 |
| 交互式动画 | `rs_interactive_implict_animator.h` | 交互式隐式动画 |
| 动画回调 | `rs_animation_callback.h` | 动画完成/重复回调 |
| 时序协议 | `rs_animation_timing_protocol.h` | RSAnimationTimingProtocol：duration/delay/repeat |
| 时序曲线 | `rs_animation_timing_curve.h` | RSAnimationTimingCurve：ease/spring/custom |
| 服务端动画基类 | `rosen/modules/render_service_base/include/animation/rs_render_animation.h` | RSRenderAnimation：服务端估值 |
| 服务端曲线动画 | `rs_render_curve_animation.h` | RSRenderCurveAnimation |
| 服务端弹簧动画 | `rs_render_spring_animation.h` | RSRenderSpringAnimation |
| 服务端关键帧 | `rs_render_keyframe_animation.h` | RSRenderKeyframeAnimation |
| 服务端路径动画 | `rs_render_path_animation.h` | RSRenderPathAnimation |
| 服务端过渡 | `rs_render_transition.h` / `rs_render_transition_effect.h` | 服务端过渡 |
| 粒子系统 | `rs_render_particle_system.h` / `rs_render_particle.h` / `rs_render_particle_emitter.h` | 粒子渲染 |
| 动画管理器 | `rs_animation_manager.h` | RSAnimationManager：节点级动画管理 |
| 帧率范围 | `rs_frame_rate_range.h` | FrameRateRange |
| 弹簧模型 | `rs_spring_model.h` | RSSpringModel：弹簧物理模型 |
| 插值器 | `rs_interpolator.h` / `rs_cubic_bezier_interpolator.h` | 时间插值 |
| 窗口动画接口 | `rosen/modules/animation/window_animation/include/rs_iwindow_animation_controller.h` | 窗口动画控制器接口 |
| 窗口动画 Stub | `rs_window_animation_stub.h` / `rs_window_animation_proxy.h` | 窗口动画 IPC |
| 窗口动画 Target | `rs_window_animation_target.h` | 窗口动画目标 |
| 动画完成回调 | `rs_window_animation_finished_callback.h` | 窗口动画完成通知 |

## 核心模型

### 动画双端架构

```
客户端 (RSAnimation)                    服务端 (RSRenderAnimation)
├── Start(target)                       ├── Animate(timestamp)->OnAnimate(fraction)
├── OnStart() → 创建 RSRenderAnimation  ├── 估值计算 → 更新 RSRenderProperty
├── state_: INITIALIZED/RUNNING/PAUSED  └── RSAnimationManager 驱动
│                  /FINISHED
└── 通过 Transaction 同步参数
```

### 动画类型

1. **RSPropertyAnimation<T>**：曲线/弹簧/关键帧/路径动画的抽象基类。
2. **RSCurveAnimation<T>**：通过 RSAnimationTimingCurve 定义插值曲线。
3. **RSSpringAnimation**：物理弹簧，duration 由模型估算；多动画同属性时**替换前者并继承速度**；blend 期间临时用 300ms。
4. **RSInterpolatingSpringAnimation**：插值弹簧，duration 由模型估算；多动画同属性时**叠加运行**（additive）。
   两者差异见下文"RSSpringModel 参数与调优"。
5. **RSKeyframeAnimation**：多个关键帧，每帧可有独立曲线。
6. **RSPathAnimation**：沿 RSPath 路径运动，支持旋转跟随。
7. **RSTransition**：进入/退出过渡效果（RSTransitionEffect）。

### 隐式动画

- `OpenImplicitAnimation()` / `CloseImplicitAnimation()` 包裹属性设置，自动为变更属性创建动画。
- `AddKeyFrame()` 在隐式动画中添加关键帧。
- `RSImplicitAnimator` 管理当前上下文的隐式动画集合。

### 交互式动画

- `InteractivePause()` / `InteractiveContinue()` / `InteractiveFinish()` / `InteractiveReverse()`。
- `RSInteractiveImplictAnimator`支持手势驱动的动画控制。

### 动画估值

- 服务端 `RSRenderAnimation::Animate()` 内由 `RSAnimationFraction::GetAnimationFraction()` 根据时间计算当前 fraction，
  再调用 `OnAnimate(float fraction)`。
- 曲线动画的 fraction 经 `RSInterpolator::Interpolate()` 映射为 interpolated fraction；
  弹簧动画则经 `RSSpringValueEstimator::UpdateAnimationValue()` 估算。
- `RSRenderPropertyAnimation::SetAnimationValue()` / `SetPropertyValue()` 将估值写入 RSRenderProperty。
- RSRenderProperty 变更触发 ModifierNG 的 `SetDirty()`。

### 动画帧率协商

- 每个 RSAnimation 可设置 preferred frame rate range。
- `FrameRateRange` 传递给 HGM，协商最终渲染帧率。
- `RSAnimationRateDecider` 汇总节点上所有动画的帧率需求。

### 窗口动画

- `RSIWindowAnimationController`：WMS 实现此接口控制窗口动画。
- `RSWindowAnimationTarget`：描述动画目标窗口（节点 ID、Surface 等）。
- `RSWindowAnimationFinishedCallback`：动画完成后通知 WMS。
- 通过 Stub/Proxy IPC 在 RenderService 和 WMS 间通信。

## 动画生命周期

### 客户端生命周期

客户端 `RSAnimation` 的核心状态在 `rosen/modules/render_service_client/core/animation/rs_animation.cpp`：

```text
INITIALIZED
  -> Start(target)
  -> StartInner(target)
  -> RUNNING
  -> Pause() / InteractivePause()
  -> PAUSED
  -> Resume() / InteractiveContinue() / InteractiveReverse()
  -> RUNNING
  -> Finish() / InteractiveFinish()
  -> FINISHED
```

检查点：

- `Start()` 只能从 `INITIALIZED` 进入；重复 start 要安全拒绝。
- target 为空、target 已销毁、RSUIContext 为空时，不能创建半挂载动画。
- `Pause()`、`Resume()`、`Reverse()`、`Finish()` 都有状态前置条件。
- 完成回调要区分“客户端请求结束”和“服务端估值真正结束”。

### 服务端 / RenderThread 生命周期

服务端或渲染线程侧 `RSRenderAnimation` 在
`rosen/modules/render_service_base/src/animation/rs_render_animation.cpp` 估值：

```text
RSAnimationManager::AddAnimation()
  -> RSRenderAnimation::Start()
  -> RSAnimationManager::Animate()
  -> RSRenderAnimation::Animate()
  -> SetAnimationValue()/SetPropertyValue() 写入 RSRenderProperty
  -> OnAnimationFinished()
  -> RemoveAnimation()
```

`RSRenderAnimation` 还有 `GROUP_WAITING`、`PAUSED`、`FINISHED` 等状态。
修改 group animation、粒子动画、关键帧或 spring 时，
要确认 `minLeftDelayTime` 和下一帧请求逻辑。

### 隐式和交互式动画

隐式动画由 `RSImplicitAnimator` 在属性设置期间创建；交互式动画由
`RSInteractiveImplictAnimator` 维护 `INACTIVE`、`ACTIVE`、`RUNNING`、`PAUSED` 状态。

交互式路径需要额外检查：

- `InteractivePause()` 之后是否允许设置 fraction。
- `InteractiveContinue()` 是否恢复到正确方向。
- `InteractiveReverse()` 是否只在暂停态生效。
- `InteractiveFinish()` 的位置参数是 START、CURRENT 还是 END。
- 交互动画销毁后，已注册的 animation id 是否从 manager 中移除。

## 取消、销毁和跨线程边界

### 取消语义

仓内动画取消并不总是表现为单一 `Cancel()` API。常见路径包括：

- 隐式动画创建新动画时取消旧属性动画，并记录 cancel trace。
- 节点销毁或动画 manager 清理时，调用 `RemoveAnimation()` 或 finish callback。
- 交互动画通过 `InteractiveFinish()`、`FinishOnPosition()` 或状态回退结束。
- 窗口动画由 WMS callback 或 target 生命周期结束触发完成通知。

修改取消语义时必须明确：

- 是否回调完成 callback。
- 是否写入最终属性值，写入 START、CURRENT 还是 END。
- 是否通知 HGM 帧率需求下降。
- 是否从 spring animation 注册表、interactive animator map 和 manager map 中移除。
- 是否会向已销毁 node、context 或 window target 投递迟到回调。

### 销毁边界

- `RSAnimation` 持有 target 的弱引用；回调执行前要重新 lock。
- `RSRenderAnimation` 只用 nodeId 查找服务端节点，节点不存在时应安全结束或忽略。
- 动画回调可能跨线程投递；lambda 不要捕获裸 `this`，除非调用方证明生命周期。
- 节点销毁时要清理关联动画，否则下一帧 `Animate()` 可能访问不存在的 property。

### 跨线程同步

动画可能同时影响 UI 线程、RenderThread、Render Service 和 HGM：

- UI 线程：`RSUIDirector` 调 `RSModifierManager::Animate()`。
- RenderThread：`RSRenderThread::Animate()` 遍历节点动画。
- Render Service：`RSAnimationManager::Animate()` 驱动服务端估值。
- HGM：动画的 `FrameRateRange` 参与刷新率投票。

新增动画类型或新增 animatable property 时，要说明估值发生在哪一侧。
不能在客户端已经结束时假设服务端下一帧已移除，
也不能在服务端 finish 时重复触发客户端回调。

## 修改检查清单

- Start/Pause/Resume/Finish/Reverse/Interactive* 的非法状态都已覆盖。
- 取消路径是否有可重复单测，尤其是节点销毁、空 target 和重复 finish。
- 最终属性值和 callback 语义是否与旧动画保持兼容。
- 新动画是否正确注册/注销到 `RSAnimationManager` 或 `RSModifierManager`。
- 帧率范围是否在动画开始、暂停、结束后更新。
- 跨线程回调是否只捕获 `weak_ptr`、id 或有生命周期保证的对象。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 客户端/服务端双端动画 | RSAnimation + RSRenderAnimation | 客户端管理生命周期和回调，服务端执行估值，减少 IPC |
| AnimationState 状态机 | INITIALIZED → RUNNING → PAUSED/FINISHED | 严格状态转换，避免非法操作 |
| 隐式动画上下文 | OpenImplicitAnimation / CloseImplicitAnimation | 简化 API，应用开发者无需手动创建动画对象 |
| 弹簧物理模型 | RSSpringModel / RSSpringAnimation | 自然物理运动，duration 由模型计算，更真实的交互反馈 |
| 帧率协商 | FrameRateRange + HGM | 动画按需驱动帧率，空闲时降低刷新率节省功耗 |
| 窗口动画 IPC | Stub/Proxy + FinishedCallback | WMS 控制窗口动画时序，RenderService 负责渲染 |

## 补充背景

### RSSpringModel 参数与调优

**mass/stiffness/damping → response/dampingRatio 转换公式**（`CreateInterpolatingSpring`/`CreateSpringCurve`）：

- `response = 2π·√(mass/stiffness)`（`PI = M_PI`）
- `dampingRatio = damping / (2·√(mass·stiffness))`
- 参数非法（`stiffness≤0` 或 `mass·stiffness≤0`）时回退 `DEFAULT_RESPONSE=0.55` / `DEFAULT_DAMPING_RATIO=0.825`。

**RSSpringModel 构造参数**（response/dampingRatio 经上述路径得到）：

| 参数名 | 参数含义 | 作用 |
| --- | --- | --- |
| response | 响应周期 | `naturalAngularVelocity = 2π/response`；response 越大，振荡周期越长、运动越慢 |
| dampingRatio | 阻尼比 | <1 欠阻尼（有回弹）；=1 临界阻尼（最快无回弹）；>1 过阻尼（缓慢无回弹） |
| initialOffset | 初始偏移量 | 起始位置与目标的位移差，决定初始形变量，影响估算 duration 的振幅基准 |
| initialVelocity | 初速度 | 影响前段运动；RSSpringAnimation 间继承时由上一动画经 `InheritSpringStatus` 传入 |
| minimumAmplitudeRatio | 最小振幅比 | 决定何时判定弹簧静止；值越大，弹簧动画越早结束，估算 duration 越短 |

**两类弹簧动画行为差异**：

| | RSSpringAnimation | RSInterpolatingSpringAnimation |
| --- | --- | --- |
| 服务端类 | RSRenderSpringAnimation | RSRenderInterpolatingSpringAnimation（继承 `RSSpringModel<float>`） |
| 估值器 | RSSpringValueEstimator | RSCurveValueEstimator（自算位移 `1+CalculateDisplacement`） |
| 多动画同属性 | 替换前者 + 继承速度（`InheritSpringAnimation`） | 叠加运行（additive） |
| blendDuration | 有（新旧 response 线性过渡） | 无 |
| 支持交互式控制 | 否 | 是 |
| duration | 模型估算 | 模型估算 |

- 三种物理分支：欠阻尼（dampingRatio<1）、临界（=1）、过阻尼（>1），见 `CalculateSpringParameters` / `CalculateDisplacement`。
- duration 由 `RSSpringModel::EstimateDuration()` 估算，范围 `[1e-3, 300]`；
  RSSpringAnimation 的 `OnInitialize` 调 `springValueEstimator_->UpdateDuration()` 后 `SetDuration`，
  blend 期间临时 `SetDuration(300)`；
  RSInterpolatingSpringAnimation 的 `OnInitialize` 设 `initialOffset_=-1` 后 `EstimateDuration()·1000` 设 duration。

### 粒子动画参数与渲染流程

`ParticleParams`（`rs_particle_params.h:135`）顶层字段：

| 顶层字段 | 类型 | 含义 |
| --- | --- | --- |
| emitterConfig_ | EmitterConfig | 粒子发射器配置（子字段见下） |
| velocity_ | ParticleVelocity | 粒子初速度（大小+角度） |
| acceleration_ | ParticleAcceleration | 粒子加速度（大小+角度） |
| color_ | ParticleColorParaType | 粒子颜色（含分布、更新方式、生命周期变化） |
| opacity_ | ParticleParaType\<float\> | 粒子不透明度 |
| scale_ | ParticleParaType\<float\> | 粒子大小 |
| spin_ | ParticleParaType\<float\> | 粒子自旋角度 |

`EmitterConfig`（`rs_render_particle.h:137`）子字段：

| 字段 | 类型 | 含义 |
| --- | --- | --- |
| emitRate_ | int | 每秒发射粒子数 |
| emitShape_ | ShapeType | 发射器形状：RECT/CIRCLE/ELLIPSE/ANNULUS |
| position_ | Vector2f | 发射器位置 |
| emitSize_ | Vector2f | 发射窗口大小 |
| particleCount_ | int32_t | 发射粒子总数 |
| lifeTime_ | Range\<int64_t\> | 单个粒子生命周期（ms） |
| type_ | ParticleType | 粒子类型：POINTS/IMAGES |
| radius_ | float | 半径 |
| image_ | shared_ptr\<RSImage\> | 图片（IMAGES 时） |
| imageSize_ | Vector2f | 图片大小 |
| shape_ | shared_ptr\<Shape\> | 发射器形状（如 AnnulusRegion） |

渲染流程：

1. **客户端构建** — `RSParticleAnimation`（`RSAnimation::IsParticleAnimation` 为 true）持有 `ParticleParams`。
2. **参数转换** — `ParticleParams::SetParamsToRenderParticle()` 将 `ParticleParaType`/`ParticleColorParaType`
   转为服务端 `RenderParticleParaType`/`RenderParticleColorParaType`；CURVE 分支用
   `curve.GetInterpolator(duration)` 生成 interpolator。
3. **服务端创建** — `AnimationCommandHelper::CreateParticleAnimationNG` 创建
   `RSRenderProperty<RSRenderParticleVector>` + Particle modifier，依次
   `AddModifier` → `AttachRenderProperty` → `SetStartTime` → `Attach`。
4. **注册与标识** — `RSAnimationManager` 维护 `particleAnimations_`，提供
   `RegisterParticleAnimation`/`UnregisterParticleAnimation`/`GetParticleAnimation`；
   类型标识 `RSRenderAnimationType::PARTICLE_ANIMATION`。
5. **每帧驱动**（`RSRenderParticleAnimation::Animate`）— 算 `deltaTime` →
   `RSRenderParticleSystem::Emit` 发射粒子（内含 `RSRenderParticleEmitter`）→
   `UpdateParticle` 更新粒子状态 → `property->Set` 写回 → `IsFinish` 判定结束并 `RemoveDrawModifier`。
6. **后台/重建** — `DestroyInRender` 对粒子用 `GetRunningTimeNs()` 取 fraction；
   后台无限粒子动画 `needUpdateStartTime_` 复位；`FillRebuildProgress` 用 `Warmup` 预热。

### RSRenderInteractiveImplictAnimatorMap 可交互动画管理

- **定义**：`RSRenderInteractiveImplictAnimatorMap`（`rs_render_interactive_implict_animator_map.h`），
  由 `friend RSContext / RSMainThread` 构造， 由 `RSContext` 持有。
- **数据结构**：`unordered_map<InteractiveImplictAnimatorId, shared_ptr<RSRenderInteractiveImplictAnimator>>`，
  key为64位ID（高32位PID + 低32位递增计数器）。

**注册**（`RegisterInteractiveImplictAnimator`）：拒绝重复ID，仅首次注册成功。

**注销**（`UnregisterInteractiveImplictAnimator`）：
  - 非组动画：客户端 `RSInteractiveImplictAnimator` 析构时发送IPC命令 `DestoryInteractiveAnimator`。
  - 组动画：`RSRenderTimeDrivenGroupAnimator::FinishAnimator()` ：结束所有子动画后，主动注销自己。

**线程安全**：服务端map无显示锁，所有访问在RS主线程。客户端 `RSUIContext` 由 `interactiveImplictAnimatorMutex_`保护。

### 窗口动画与 RSNode Transition 的关系

**窗口动画（`RSIWindowAnimationController`，跨进程 IPC，窗口级）**

- 触发：WMS 在 app 启动/切换/返回/最小化/关闭/解锁等窗口生命周期事件时经 Stub/Proxy 调 controller
  （`OnStartApp`/`OnAppTransition`/`OnAppBackTransition`/`OnMinimizeWindow`/`OnCloseWindow`/`OnScreenUnlock`）。
- 目标载体：`RSWindowAnimationTarget::surfaceNode_` 反序列化时建为 **ProxyNode**
  （`RSSurfaceNode::UnmarshallingAsProxyNode`，`rs_window_animation_target.cpp:92`），代理节点并非应用渲染树中的真实节点。
- Launcher操控代理节点属性实现动画效果，完成后JS调用 `finishedCallback->OnAnimationFinished()` 通知窗口管理器。

**RSNode Transition（`RSTransition` / `RSRenderTransition`，进程内，节点级）**

- 触发：节点树变化（可见性变化/增删子节点）且处于 `animateTo` 隐式块内时，`RSNode::NotifyTransition`
  → `BeginImplicitTransition`/`CreateImplicitTransition`/`EndImplicitTransition`（`rs_node.cpp:3094`）；
  需节点已设 `transitionEffect_` 且 `NeedImplicitAnimation()` 为真。
- 执行：`RSTransition::OnStart` 建 `RSRenderTransition` 并发 `RSAnimationCreateTransition` 命令
  （`rs_transition.cpp:50/56`）；服务端 `RSRenderTransition::OnAttach` 给目标节点加 transition modifier
  并按 fraction 驱动（`rs_render_transition.cpp:54-60/35-44`），`OnDetach` 移除。
- 退场保活：退场 transition 递增 `disappearingTransitionCount_`，移入父节点的 `disappearingChildren_` 保持存活，
  直到 `disappearingTransitionCount_` 归零后才真正移除。

**关系与优先级**

- 代理节点机制使二者不直接触碰同一对象：窗口动画作用于 ProxyNode/leash（整窗 surface），
  RSTransition 作用于应用渲染树内具体节点的 modifier。
- 合成层面是"叠加"而非"二选一"：leash 变换在合成/HWC 层作用于整窗 surface，
  RSTransition 影响应用渲染帧内节点 modifier，二者可同时作用于同一个 `RSSurfaceNode`。
- 二者处于不同层级，本仓**无**二者间的显式优先级仲裁代码。

### 动画取消时的资源清理与回调保证

**取消与资源清理**

- 客户端隐式 CANCEL：custom 属性走 `SetValue` + `UpdateCustomAnimation` + `CancelAnimationByProperty`；
  非 custom 走 `SetValue` + `UpdateOnAllAnimationFinish` + `FinishAnimationByProperty`。
- 服务端按 propertyId：`RSAnimationManager::CancelAnimationByPropertyId` → `OnAnimationFinished` + 擦除。
- 按 animationId（含 pending）：`AttemptCancelAnimationByAnimationId`，未找到入 `pendingCancelAnimation_`，
  后续 `AddAnimation` 命中即丢弃。
- 按 pid：`FilterAnimationByPid` → `Finish` + `Detach` + 擦除。
- 弹簧注销：`RSRenderSpringAnimation::OnDetach` → `UnregisterSpringAnimation`。

**动画回调机制**

1. **服务端结束** — `RSAnimationManager::OnAnimationFinished` 把 `RSAnimationCallback(... FINISHED)`
   命令加入消息队列；弹簧在位移/速度近零时先发 `LOGICALLY_FINISHED`（仅 `finishCallbackType_=LOGICALLY` 时）。
2. **客户端路由** — `RSUIDirector::AnimationCallbackProcessor` 按 nodeId 找节点
   （找不到回退到 RSUIContext / fallback 节点），调用 `RSNode::AnimationCallback(FINISHED)`。
3. **节点处理** — `RSNode::AnimationCallback`：`RemoveAnimationInner` 移除动画 +
   `animation->CallFinishCallback()`。
4. **释放引用（-1）** — `CallFinishCallback` 执行 `finishCallback_.reset()`，仅把本动画持有的
   `shared_ptr` 引用计数减 1，并置 `state_=FINISHED`。
6. **归 0 执行** — 当所有引用释放完（各动画 `CallFinishCallback` + 栈 pop），引用计数为 0，
   触发 `~AnimationFinishCallback` → `~AnimationCallback`，析构内执行 `callback_()` 回调到 ArkUI。
7. **互斥与兜底** — `Execute()` 执行后置 `callback_=nullptr`，析构不再重复执行；
   `ProcessAnimationFinishCallbackGuaranteeTask` 对非 UI 非交互动画按 `max(2×估算时长, 1000ms)`
   投递 `weak_ptr` 延时任务，超时未释放则强制执行 `finishcallback` 兜底；空隐式块（无动画产生）由
   `ProcessEmptyAnimations` 直接 `Execute()`。
8. **弹簧提前回调** — `LOGICALLY_FINISHED` 走 `CallLogicallyFinishCallback`，仅 `finishCallback_.reset()`
   释放引用（不置 FINISHED、不移除动画），可使 ArkUI 回调在弹簧近静止时提前触发；
   动画对象要到 `FINISHED` 才从节点移除。
