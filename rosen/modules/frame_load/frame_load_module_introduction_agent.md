# Frame Load 帧负载预测模块介绍

## 1. 概述

Frame Load 模块是 Rosen 渲染服务中的帧负载预测与调度辅助模块，负责模糊渲染预测、帧截止时间（deadline）计算以及游戏场景帧调度处理。该模块通过对帧负载的预判，帮助渲染管线在不同场景下做出更优的调度决策。

## 2. 模块职责

### 2.1 核心职责

1. **大区域模糊预测**
    - 预测当前帧是否存在大区域模糊绘制
    - 提供预测式（predictive）和精确式（precisely）两种预测模式
    - 将预测结果上报给帧率调控模块（FrameReport），辅助帧率决策

2. **帧截止时间计算**
    - 根据屏幕刷新率、LTPO 状态、VSync 偏移等参数计算帧截止时间
    - 针对 120Hz OLED 屏幕做特殊优化处理
    - 将计算结果上报给 RsFrameReport

3. **游戏场景帧调度**
    - 检测游戏场景的进入和退出
    - 在游戏场景切换时动态调整 VSync 偏移
    - 根据统一渲染（UniRender）模式选择不同的偏移策略

### 2.2 模块间协作关系

| 本模块类 | 协作模块/类 | 协作方式 |
|----------|------------|---------|
| RsFrameBlurPredict | RsFrameReport | 上报模糊预测结果 |
| RsFrameBlurPredict | RSRenderNode | 获取节点的模糊绘制信息 |
| RsFrameBlurPredict | RsVariableFrameRate | 接收模糊场景事件通知 |
| RsFrameDeadlinePredict | VSyncGenerator | 获取 VSync 偏移量 |
| RsFrameDeadlinePredict | RsFrameReport | 上报帧截止时间 |
| RsFrameDeadlinePredict | HgmContext | 获取屏幕刷新率等信息 |
| RsGameFrameHandler | FrameReport | 查询游戏场景状态 |
| RsGameFrameHandler | RSUniRenderJudgement | 判断统一渲染模式 |

## 3. 模块文件结构

```
frame_load/
├── BUILD.gn                              # 构建配置
├── include/
│   ├── rs_frame_blur_predict.h           # 模糊预测类头文件
│   ├── rs_frame_deadline_predict.h       # 帧截止时间预测类头文件
│   └── rs_game_frame_handler.h           # 游戏帧处理类头文件
└── src/
    ├── rs_frame_blur_predict.cpp          # 模糊预测实现
    ├── rs_frame_deadline_predict.cpp      # 帧截止时间预测实现
    └── rs_game_frame_handler.cpp          # 游戏帧处理实现
```

### 3.1 核心功能文件

#### 3.1.1 rs_frame_blur_predict.h/cpp

**功能**: 帧模糊预测器，预测当前帧是否包含大区域模糊绘制，辅助帧率调控。

**设计模式**: 单例模式（`GetInstance()`）

**主要职责**:
- 接收模糊场景事件，标记当前帧是否为有效模糊帧
- 遍历渲染节点，收集大区域模糊绘制信息
- 提供预测式和精确式两种预测上报模式
- 每帧结束后重置预测状态

**核心成员**:
```cpp
class RsFrameBlurPredict {
public:
    static RsFrameBlurPredict& GetInstance();
    void AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively();
    void TakeEffectBlurScene(const EventInfo& eventInfo);
    void UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely();
    void PredictDrawLargeAreaBlur(RSRenderNode& node);

private:
    RsFrameBlurPredict();
    ~RsFrameBlurPredict();
    void GetCurrentFrameDrawLargeAreaBlurPredictively();
    void GetCurrentFrameDrawLargeAreaBlurPrecisely();
    void ResetPredictDrawLargeAreaBlur();

    const std::string RS_BLUR_PREDICT_INVALID = "0";  // 无效预测
    const std::string RS_BLUR_PREDICT_LONG = "1";     // 长模糊（大区域模糊）
    const std::string RS_BLUR_PREDICT_SHORT = "2";    // 短模糊（无大区域模糊）
    std::atomic<bool> isValidBlurFrame_{false};       // 当前帧是否为有效模糊帧
    std::atomic<bool> predictBegin_{false};            // 预测是否已开始（精确模式触发后置true）
    std::pair<bool, bool> predictDrawLargeAreaBlur_ = {false, false};  // first:预测式, second:精确式
};
```

**工作流程**:
1. **场景激活**: `TakeEffectBlurScene()` 被调用，设置 `isValidBlurFrame_` 标记
2. **节点遍历**: 渲染过程中对每个节点调用 `PredictDrawLargeAreaBlur()`，收集模糊信息
3. **预测上报**:
   - 预测式: `AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively()` — 基于前一帧的预测结果提前决策
   - 精确式: `UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely()` — 基于当前帧的实际结果精确决策
4. **状态重置**: 每帧结束时重置 `predictDrawLargeAreaBlur_`

**预测结果上报格式**:
```cpp
// 通过 RsFrameReport::BlurPredict() 上报
param["frameBlurPredict"] = RS_BLUR_PREDICT_INVALID;  // "0" - 无效（预测未开始）
param["frameBlurPredict"] = RS_BLUR_PREDICT_LONG;     // "1" - 长模糊（存在大区域模糊）
param["frameBlurPredict"] = RS_BLUR_PREDICT_SHORT;    // "2" - 短模糊（无大区域模糊）
```

---

#### 3.1.2 rs_frame_deadline_predict.h/cpp

**功能**: 帧截止时间预测器，根据屏幕刷新率和硬件特性计算帧渲染的截止时间。

**设计模式**: 单例模式（`GetInstance()`）

**主要职责**:
- 针对 120Hz OLED 屏幕计算额外的渲染时间预留
- 区分 LTPO（低温多晶氧化物）和非 LTPO 屏幕的不同策略
- 将帧截止时间上报给 RsFrameReport

**核心成员**:
```cpp
class RsFrameDeadlinePredict {
public:
    static RsFrameDeadlinePredict& GetInstance();
    void ReportRsFrameDeadline(uint32_t rate, int64_t period, bool ltpoEnabled, bool forceRefreshFlag);

private:
    RsFrameDeadlinePredict();
    ~RsFrameDeadlinePredict() = default;

    int64_t preIdealPeriod_ = 0;     // 上一次的理想帧周期（ns）
    int64_t preExtraReserve_ = 0;    // 上一次的额外预留时间（ns）
};
```

**截止时间计算常量**:
```cpp
constexpr int64_t FIXED_EXTRA_DRAWING_TIME = 3000000;  // 3ms - 非 LTPO 固定额外绘制时间
constexpr int64_t SINGLE_SHIFT = 2700000;              // 2.7ms - 单倍偏移阈值
constexpr int64_t DOUBLE_SHIFT = 5400000;              // 5.4ms - 双倍偏移阈值
```

**计算逻辑**:
```cpp
void ReportRsFrameDeadline(uint32_t rate, int64_t period, bool ltpoEnabled, bool forceRefreshFlag) {
    int64_t extraReserve = 0;
    int64_t vsyncOffset = 0;
    int64_t idealPeriod = period;

    if (rate == OLED_120_HZ) {
        if (ltpoEnabled) {
            // LTPO 屏幕：根据 VSync 偏移动态计算额外预留时间
            vsyncOffset = CreateVSyncGenerator()->GetVSyncOffset();
            if (vsyncOffset > SINGLE_SHIFT && vsyncOffset <= DOUBLE_SHIFT) {
                extraReserve = SINGLE_SHIFT;    // 2.7ms
            } else if (vsyncOffset > DOUBLE_SHIFT && vsyncOffset < idealPeriod) {
                extraReserve = DOUBLE_SHIFT;    // 5.4ms
            }
        } else {
            // 非 LTPO 屏幕：固定 3ms 额外时间
            extraReserve = FIXED_EXTRA_DRAWING_TIME;
        }
    }

    // 仅在参数变化时上报
    if (idealPeriod == preIdealPeriod_ && extraReserve == preExtraReserve_) {
        return;
    }
    // 强制刷新时使用理想周期，否则加上额外预留
    int64_t drawingTime = forceRefreshFlag ? idealPeriod : idealPeriod + extraReserve;
    RsFrameReport::ReportFrameDeadline(drawingTime, currentRate);
}
```

**决策矩阵**:

| 屏幕类型 | 刷新率 | LTPO | VSync 偏移范围 | 额外预留 |
|---------|--------|------|---------------|---------|
| OLED | 120Hz | 是 | (2.7ms, 5.4ms] | 2.7ms |
| OLED | 120Hz | 是 | (5.4ms, period) | 5.4ms |
| OLED | 120Hz | 否 | - | 3ms（固定） |
| 非 OLED 或非 120Hz | - | - | - | 0 |

---

#### 3.1.3 rs_game_frame_handler.h/cpp

**功能**: 游戏场景帧调度处理器，在游戏场景切换时动态调整 VSync 偏移量。

**主要职责**:
- 监听游戏场景的进入和退出
- 根据统一渲染（UniRender）模式选择 VSync 偏移策略
- 通过回调函数将新的偏移量应用到渲染管线

**核心成员**:
```cpp
class RsGameFrameHandler : public RefBase {
using HandleGameSceneChangedFunc = std::function<void(int64_t, int64_t)>;
using GetCoreInfoFunc = std::function<void(bool&, bool&, bool&, int64_t&, int64_t&)>;
public:
    RsGameFrameHandler(
        const HandleGameSceneChangedFunc& handleGameSceneChangedFunc,
        const GetCoreInfoFunc& getCoreInfoFunc);
    ~RsGameFrameHandler() = default;

    void HandleGameSceneChanged();

private:
    HandleGameSceneChangedFunc handleGameSceneChangedFunc_ = nullptr;  // 场景变更回调
    GetCoreInfoFunc getCoreInfoFunc_ = nullptr;                        // 核心信息获取函数
    std::atomic<bool> hasGameScene_{false};                            // 当前是否处于游戏场景
};
```

**VSync 偏移常量**:
```cpp
constexpr int64_t UNI_RENDER_VSYNC_OFFSET = 5000000;             // 5ms - UniRender 正常模式偏移
constexpr int64_t UNI_RENDER_VSYNC_OFFSET_DELAY_MODE = -3300000; // -3.3ms - UniRender 延迟模式偏移
```

**调度逻辑**:
```cpp
void HandleGameSceneChanged() {
    // 1. 获取核心信息
    bool isLtpoEnabled, isVsyncOffsetCustomized, isDelayMode;
    int64_t rsOffset, appOffset;
    getCoreInfoFunc_(isLtpoEnabled, isVsyncOffsetCustomized, isDelayMode, rsOffset, appOffset);

    // 2. 仅在非 LTPO 且有自定义 VSync 偏移配置时生效
    if (isLtpoEnabled || !isVsyncOffsetCustomized) {
        return;
    }

    // 3. 检测游戏场景是否发生变化
    bool hasGameScene = FrameReport::GetInstance().HasGameScene();
    if (hasGameScene_.exchange(hasGameScene) == hasGameScene) {
        return;  // 场景未变化，无需调整
    }

    // 4. 计算新的 VSync 偏移
    int64_t offset = 0;
    if (RSUniRenderJudgement::GetUniRenderEnabledType() == UNI_RENDER_ENABLED_FOR_ALL) {
        offset = isDelayMode ? UNI_RENDER_VSYNC_OFFSET_DELAY_MODE : UNI_RENDER_VSYNC_OFFSET;
    }

    // 5. 应用偏移：游戏场景用自定义值，非游戏场景恢复正常偏移
    handleGameSceneChangedFunc_(hasGameScene ? rsOffset : offset,
                                hasGameScene ? appOffset : offset);
}
```

**决策流程图**:

```
HandleGameSceneChanged()
        │
        ▼
  获取核心信息（LTPO、VSync偏移配置、延迟模式、RS/App偏移）
        │
        ▼
  ┌─ LTPO 已启用 ────→ 退出（不处理）
  │  VSync 偏移未自定义 ──→ 退出（不处理）
        │
        ▼
  查询游戏场景状态（FrameReport::HasGameScene）
        │
        ▼
  ┌─ 场景未变化 ──→ 退出（无需调整）
        │
        ▼
  ┌─ UniRender 全部启用 ──→ offset = 延迟模式 ? -3.3ms : 5ms
  │  否则 ──→ offset = 0
        │
        ▼
  ┌─ 游戏场景 ──→ 应用自定义 rsOffset / appOffset
  │  非游戏场景 ──→ 恢复 UniRender 偏移 offset
```

---

### 3.2 构建配置

#### 3.2.1 BUILD.gn

**功能**: 定义 frame_load 模块的构建配置。

**编译特性**:
- C++17 标准
- `-O3` 优化等级
- 启用 CFI（控制流完整性）安全检查
- 启用 LTO（链接时优化）

**编译源文件**:
```gn
sources = [
    "src/rs_frame_blur_predict.cpp",
    "src/rs_frame_deadline_predict.cpp",
]
```

> 注意：`rs_game_frame_handler.cpp` 存在于源码目录中但未包含在 BUILD.gn 的 sources 列表中，说明该文件可能处于开发阶段或由其他构建目标引用。

**依赖关系**:
```gn
deps = [
    "render_service_base:render_service_base_src",  # 渲染服务基础模块
    "frame_report:frame_report",                     # 帧上报模块
]

external_deps = [
    "hilog:libhilog",       # 日志库
    "libxml2:libxml2",      # XML 解析库
    "skia:skia_canvaskit",  # Skia 图形库
]
```
