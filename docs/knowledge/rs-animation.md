# Animation 与窗口动画

## 适用范围

- RSAnimation 客户端动画体系：属性动画、弹簧动画、关键帧动画、路径动画
- RSRenderAnimation 服务端动画估值
- 隐式动画与交互式动画
- Transition 过渡效果
- 粒子动画
- 窗口动画（RSWindowAnimation）IPC 体系
- 动画帧率协商（RSFrameRateRange）

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
| 帧率范围 | `rs_frame_rate_range.h` | RSFrameRateRange |
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
├── Start(target)                       ├── OnAnimate(timestamp)
├── OnStart() → 创建 RSRenderAnimation  ├── 估值计算 → 更新 RSRenderProperty
├── state_: INITIALIZED/RUNNING/PAUSED  └── RSAnimationManager 驱动
│                  /FINISHED
└── 通过 Transaction 同步参数
```

### 动画类型

1. **RSPropertyAnimation<T>**：属性值从 start 到 end 线性变化。
2. **RSCurveAnimation<T>**：通过 RSAnimationTimingCurve 定义插值曲线。
3. **RSSpringAnimation**：基于 RSSpringModel 的物理弹簧模型，无固定 duration。
4. **RSKeyframeAnimation**：多个关键帧，每帧可有独立曲线。
5. **RSPathAnimation**：沿 RSPath 路径运动，支持旋转跟随。
6. **RSTransition**：进入/退出过渡效果（RSTransitionEffect）。

### 隐式动画

- `OpenImplicitAnimation()` / `CloseImplicitAnimation()` 包裹属性设置，自动为变更属性创建动画。
- `AddKeyFrame()` 在隐式动画中添加关键帧。
- `RSImplicitAnimator` 管理当前上下文的隐式动画集合。

### 交互式动画

- `InteractivePause()` / `InteractiveContinue()` / `InteractiveFinish()` / `InteractiveReverse()`。
- `RSInteractiveImplicitAnimator` 支持手势驱动的动画控制。

### 动画估值

- 服务端 `RSRenderAnimation::OnAnimate(timestamp)` 根据时间计算当前 fraction。
- fraction 通过 Interpolator 映射为 interpolated fraction。
- `RSRenderPropertyAnimation::ApplyValue()` 将估值写入 RSRenderProperty。
- RSRenderProperty 变更触发 ModifierNG 的 SetDirty()。

### 动画帧率协商

- 每个 RSAnimation 可设置 preferred frame rate range。
- `RSFrameRateRange` 传递给 HGM，协商最终渲染帧率。
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
  -> ApplyValue() 写入 RSRenderProperty
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
- HGM：动画的 `RSFrameRateRange` 参与刷新率投票。

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
| 弹簧物理模型 | RSSpringModel / RSSpringAnimation | 自然物理运动，无固定 duration，更真实的交互反馈 |
| 帧率协商 | RSFrameRateRange + HGM | 动画按需驱动帧率，空闲时降低刷新率节省功耗 |
| 窗口动画 IPC | Stub/Proxy + FinishedCallback | WMS 控制窗口动画时序，RenderService 负责渲染 |

## 待补充背景

- RSSpringModel 的参数含义（mass/stiffness/damping）和调优方法。
- 粒子动画的完整参数结构和渲染流程。
- RSRenderInteractiveImplicitAnimatorMap 的多实例管理。
- 窗口动画与 RSNode Transition 的关系和优先级。
- 动画取消时的资源清理和回调保证。
