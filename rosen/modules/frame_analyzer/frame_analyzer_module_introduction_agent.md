# Frame Analyzer 帧分析模块介绍

## 1. 概述

Frame Analyzer 模块是 Rosen 渲染服务中的帧性能分析工具模块，负责采集、可视化和持久化记录每一帧中各渲染阶段的时间戳信息。该模块为开发者提供帧级别的性能调试能力，帮助定位渲染管线中的耗时瓶颈。

## 2. 模块职责

### 2.1 核心职责

1. **帧事件采集**
    - 采集每一帧中 UI 侧和 RS 侧各阶段的时间戳
    - 支持两种工作模式：绘制模式（paint）和保存模式（saver）
    - 通过系统参数动态切换工作模式，无需重启

2. **帧数据可视化**
    - 将帧队列中的时间数据绘制为时间条（TimeBar）图
    - 按颜色区分不同渲染阶段（动画、布局、绘制、刷新等）
    - 绘制 FPS 参考线（60fps 和 30fps），直观标识掉帧

3. **帧数据持久化**
    - 将帧事件以文本形式写入日志文件
    - 日志文件按进程 PID 命名，存储在 `/data/frame_render/` 目录
    - 文件不可用时回退到控制台输出

4. **跨平台支持**
    - 提供 OHOS 完整实现和 MinGW 桩实现两套代码
    - MinGW 版本用于 Windows 平台的预览/开发场景

### 2.2 帧事件类型划分

帧事件分为两大区域：

| 区域 | 事件范围 | 说明 |
|------|---------|------|
| **UI 事件（UIMarks）** | HandleInputStart ~ DrawEnd | 应用侧的输入处理、动画、构建、上传、布局、绘制 |
| **RS 事件** | WaitVsyncStart ~ FlushEnd | 渲染服务侧的等待 VSync、释放、刷新 |

### 2.3 模块间协作关系

| 本模块类 | 协作模块/类 | 协作方式 |
|----------|------------|---------|
| FrameCollector | HiTrace | 异步追踪帧事件，用于 trace/perfetto 分析 |
| FrameCollector | FrameSaver | 将帧事件写入日志文件 |
| FramePainter | FrameCollector | 从帧队列获取数据进行可视化绘制 |
| FramePainter | Drawing::Canvas | 使用 2D 图形库绘制时间条和参考线 |

## 3. 模块文件结构

```
frame_analyzer/
├── BUILD.gn                                  # 构建配置
├── export/                                   # 对外导出头文件
│   ├── frame_collector.h                     # 帧采集器
│   ├── frame_info.h                          # 帧事件类型与数据结构定义
│   ├── frame_painter.h                       # 帧可视化绘制器
│   └── ring_queue.h                          # 环形队列模板
└── src/                                      # 实现文件
    ├── frame_collector.cpp                   # 帧采集器实现（OHOS）
    ├── frame_painter.cpp                     # 帧可视化绘制器实现（OHOS）
    ├── frame_saver.h                         # 帧持久化保存器头文件（内部）
    ├── frame_saver.cpp                       # 帧持久化保存器实现（OHOS）
    └── mingw/                                # MinGW 平台桩实现
        ├── frame_collector.cpp               # 帧采集器空实现
        └── frame_painter.cpp                 # 帧可视化空实现
```

### 3.1 数据结构定义

#### 3.1.1 frame_info.h

**功能**: 定义帧事件类型枚举、帧数据结构和相关常量。

**帧事件类型枚举**:
```cpp
enum class FrameEventType : int32_t {
    // UI 事件（应用侧）
    HandleInputStart = 0,    // 输入处理开始
    HandleInputEnd,          // 输入处理结束
    AnimateStart,            // 动画开始
    AnimateEnd,              // 动画结束
    BuildStart,              // 构建开始
    BuildEnd,                // 构建结束
    UploadStart,             // 上传开始
    UploadEnd,               // 上传结束
    LayoutStart,             // 布局开始
    LayoutEnd,               // 布局结束
    DrawStart,               // 绘制开始
    DrawEnd,                 // 绘制结束

    // RS 事件（渲染服务侧）
    WaitVsyncStart,          // 等待 VSync 开始
    WaitVsyncEnd,            // 等待 VSync 结束
    ReleaseStart,            // 释放开始
    ReleaseEnd,              // 释放结束
    FlushStart,              // 刷新开始
    FlushEnd,                // 刷新结束

    Max,                     // 事件类型总数上限
};
```

**事件编号规则**: 起始事件为偶数，结束事件为奇数（成对出现）。

**帧事件颜色映射**:
```cpp
static inline const std::map<FrameEventType, uint32_t> frameEventColorMap = {
    {FrameEventType::AnimateStart,     0x0000cc00}, // 中绿色 - 动画
    {FrameEventType::BuildStart,       0x0000ffff}, // 青色   - 构建
    {FrameEventType::LayoutStart,      0x0000ff00}, // 绿色   - 布局
    {FrameEventType::DrawStart,        0x000000ff}, // 蓝色   - 绘制
    {FrameEventType::WaitVsyncStart,   0x00006600}, // 暗绿色 - 等待VSync
    {FrameEventType::ReleaseStart,     0x00ffff00}, // 黄色   - 释放
    {FrameEventType::FlushStart,       0x00ff0000}, // 红色   - 刷新
};
```

**帧信息数据结构**:
```cpp
struct UIMarks {
    int32_t frameNumber = 0;
    std::array<int64_t, static_cast<size_t>(FrameEventType::UIMarksLen)> times = {};
};

struct FrameInfo {
    int32_t frameNumber = 0;
    bool skiped = false;   // 是否为跳过的帧
    std::array<int64_t, static_cast<size_t>(FrameEventType::LoopLen)> times = {};
};
```

**关键常量**:
```cpp
static constexpr int32_t frameQueueMaxSize = 60;           // 帧队列最大容量（60帧）
static constexpr double frameTotalMs = 160;                 // 可视化窗口总时长（160ms）
static constexpr const char *switchRenderingText = "debug.graphic.frame";  // 系统参数开关名
static constexpr const char *saveDirectory = "/data/frame_render";         // 帧日志保存目录
```

---

### 3.2 环形队列

#### 3.2.1 ring_queue.h

**功能**: 通用的固定容量环形队列模板，支持正向和反向迭代器。

**设计特点**:
- 基于固定大小 `std::array` 实现，无动态内存分配
- 队列满时自动覆盖最早元素
- 提供正向（`Iter`）和反向（`RevertIter`）迭代器

**核心接口**:
```cpp
template<class T, int cap>
class RingQueue {
public:
    static constexpr int realcap = cap + 1;  // 实际容量 = 用户容量 + 1

    int GetSize() const;        // 获取当前元素数量
    T &Push(const T& t);        // 入队（满时覆盖最早元素，返回最新元素引用）
    void Clear();               // 清空队列

    Iter begin();               // 正向迭代器起点
    Iter end();                 // 正向迭代器终点
    RevertIter rbegin();        // 反向迭代器起点（最新元素）
    RevertIter rend();          // 反向迭代器终点（最早元素）

private:
    Array queue_;
    int front_ = 0;
    int back_ = 0;
};
```

**使用方式**:
```cpp
using FrameInfoQueue = RingQueue<struct FrameInfo, frameQueueMaxSize>;  // 60 帧的环形队列
```

---

### 3.3 核心功能文件

#### 3.3.1 frame_collector.h/cpp

**功能**: 帧事件采集器，是模块的核心入口，负责收集和管理帧事件数据。

**设计模式**: 单例模式（`GetInstance()`）

**主要职责**:
- 接收并处理各渲染阶段的时间戳事件
- 维护帧队列（环形队列，容量 60 帧）
- 分离 UI 事件和 RS 事件的处理逻辑
- 通过系统参数动态开启/关闭/切换工作模式
- 集成 HiTrace 异步追踪，支持 trace 分析

**核心成员**:
```cpp
class FrameCollector {
public:
    static FrameCollector &GetInstance();

    void SetRepaintCallback(std::function<void()> repaint);
    const FrameInfoQueue &LockGetFrameQueue();    // 加锁获取帧队列
    void UnlockFrameQueue();                      // 解锁帧队列
    bool IsEnabled() const;
    void SetEnabled(bool enable);
    void MarkFrameEvent(const FrameEventType &type, int64_t timeNs = 0);  // 标记帧事件
    void ClearEvents();

private:
    FrameCollector();
    void ProcessFrameEvent(int32_t index, int64_t timeNs);
    bool ProcessUIMarkLocked(int32_t index, int64_t timeNs);
    static void SwitchFunction(const char *key, const char *value, void *context);

    // 帧处理状态
    std::mutex pendingMutex_;
    int32_t currentFrameNumber_ = 0;     // 当前帧编号
    struct UIMarks pendingUIMarks_ = {};  // 正在收集的 UI 标记
    struct UIMarks currentUIMarks_ = {};  // 当前完成的 UI 标记
    struct FrameInfo *pbefore_ = nullptr; // 指向当前帧（VSync 前部分）
    struct FrameInfo *pafter_ = nullptr;  // 指向当前帧（VSync 后部分）
    bool haveAfterVsync_ = false;         // 是否已收到 VSync 后事件

    // 帧队列
    std::mutex frameQueueMutex_;
    FrameInfoQueue frameQueue_;

    // 工作模式
    bool enabled_ = false;                // 是否启用绘制模式
    bool usingSaver_ = false;             // 是否使用保存模式
    std::shared_ptr<FrameSaver> saver_ = nullptr;
    std::function<void()> repaint_ = nullptr;
};
```

**帧事件处理流程**:

```
MarkFrameEvent(type, timeNs)
        │
        ▼
  ┌─ timeNs == 0 ──→ 自动获取当前时间戳
        │
        ▼
  ┌─ usingSaver_ ──→ saver_->SaveFrameEvent(type, timeNs)
        │
        ▼
  ┌─ !enabled_ ──→ 退出（仅 saver 模式不继续处理）
        │
        ▼
  ProcessFrameEvent(index, timeNs)
        │
        ├──→ UI 事件 (index <= DrawEnd):
        │      ProcessUIMarkLocked()
        │      ├─ 起始事件：记录时间到 pendingUIMarks_
        │      └─ 结束事件(DrawEnd)：将 pendingUIMarks_ 提交为 currentUIMarks_
        │
        └──→ RS 事件:
               ├─ WaitVsyncStart: Push 新 FrameInfo，填入 UI 标记数据
               └─ 其他 RS 事件: 填入 pafter_ 的时间数组
```

**动态开关机制**:
```cpp
// 通过系统参数控制，无需重启
// 设置命令示例：
//   hdc shell param set debug.graphic.frame paint    → 开启可视化绘制模式
//   hdc shell param set debug.graphic.frame saver    → 开启日志保存模式
//   hdc shell param set debug.graphic.frame disable  → 关闭

void FrameCollector::SwitchFunction(const char *key, const char *value, void *context) {
    if (str == "paint")   → 清空队列，关闭 saver，开启 enabled
    if (str == "saver")   → 清空队列，开启 saver，关闭 enabled
    if (str == "disable") → 关闭 saver 和 enabled
}
```

---

#### 3.3.2 frame_painter.h/cpp

**功能**: 帧数据可视化绘制器，将帧队列中的时间数据渲染为时间条图。

**主要职责**:
- 从 FrameCollector 获取帧队列数据
- 为每帧的各渲染阶段生成时间条（TimeBar）
- 绘制 FPS 参考线（60fps 绿线、30fps 红线）
- 区分重帧（超过 16.6ms）和轻帧，使用不同透明度

**核心成员**:
```cpp
class FramePainter {
public:
    FramePainter(FrameCollector &collector);
    void Draw(Drawing::Canvas &canvas);    // 主绘制入口

private:
    struct TimeBar {                       // 时间条结构
        bool isHeavy = false;              // 是否为重帧
        uint32_t color = 0;               // 颜色
        double posX = 0;                   // X 坐标
        double posY = 0;                   // Y 坐标
        double width = 0;                  // 宽度
        double height = 0;                 // 高度
    };

    void DrawFPSLine(Drawing::Canvas &canvas, uint32_t fps, double thickness, uint32_t color);
    std::vector<struct TimeBar> GenerateTimeBars(uint32_t width, uint32_t height, uint32_t fps);
    double SumTimesInMs(const struct FrameInfo &info);

    FrameCollector &collector_;            // 关联的帧采集器
};
```

**绘制逻辑**:
```cpp
void FramePainter::Draw(Drawing::Canvas &canvas) {
    if (!collector_.IsEnabled()) return;

    auto bars = GenerateTimeBars(width, height, normalFPS /* 60 */);

    // 绘制时间条
    for (const auto &[isHeavy, color, x, y, w, h] : bars) {
        auto alpha = isHeavy ? 0x7f : 0x3f;  // 重帧：半透明，轻帧：更透明
        brush.SetColor(color | (alpha << 24));
        canvas.DrawRect(Drawing::Rect(x, y, w + x, h + y));
    }

    // 绘制 FPS 参考线
    DrawFPSLine(canvas, 60, height / 160, 0xbf00ff00);  // 60fps 绿线
    DrawFPSLine(canvas, 30, height / 160, 0xbfff0000);  // 30fps 红线
}
```

**时间条生成逻辑**:
```
GenerateTimeBars()
        │
        ▼
  从帧队列反向遍历（最新帧在右侧）
        │
        ▼
  对每帧:
  ├─ 计算总耗时 SumTimesInMs()
  ├─ 判断是否为重帧（总耗时 >= 16.6ms 即 1/60s）
  └─ 对每个渲染阶段:
     ├─ 查找颜色映射
     ├─ 计算 diffMs = endTime - startTime
     ├─ 转换为像素高度
     └─ 生成 TimeBar
```

**可视化效果示意**:

```
│  ┌─┐              ← 60fps 参考线（绿色）
│  │ │  ┌─┐
│  │ │  │ │  ┌─┐
│  │ │  │ │  │ │  ┌──┐
│  │ │  │ │  │ │  │  │
│  │ │  │ │  │ │  │  │
│  │ │  │ │  │ │  │  │
│  │ │  │ │  │ │  │  │
│  │ │  │ │  │ │  │  │
│  │ │  │ │  │ │  │  │
│  │ │  │ │  │ │  │  │    ← 30fps 参考线（红色）
│  │ │  │ │  │ │  │  │
│  │ │  │ │  │ │  │  │
│  │ │  │ │  │ │  │  │
└──┴─┴──┴─┴──┴─┴──┴──┴──
  帧1    帧2   帧3   帧4
```

---

#### 3.3.3 frame_saver.h/cpp

**功能**: 帧事件持久化保存器，将帧事件写入日志文件。

**主要职责**:
- 在 `/data/frame_render/` 目录下创建日志文件
- 文件命名格式：`{PID}.log`
- 逐事件写入事件名称和时间戳
- 文件不可用时回退到控制台输出

**核心成员**:
```cpp
class FrameSaver {
public:
    FrameSaver();
    ~FrameSaver();

    void SaveFrameEvent(const FrameEventType &type, int64_t timeNs);

private:
    std::ofstream ofs_;   // 文件输出流
};
```

**保存逻辑**:
```cpp
FrameSaver::FrameSaver() {
    // 1. 检查 /data/frame_render/ 目录是否存在
    // 2. 不存在则创建目录（权限 0777）
    // 3. 打开 {PID}.log 文件（追加模式）
}

void FrameSaver::SaveFrameEvent(const FrameEventType &type, int64_t timeNs) {
    if (!ofs_.is_open()) {
        // 文件未打开，回退到日志输出
        LOGI("%s %s", GetNameByFrameEventType(type).c_str(), ...);
        return;
    }
    // 写入文件：事件名称 + 时间戳
    ofs_ << GetNameByFrameEventType(type) << " " << timeNs << std::endl;
}
```

**日志输出示例**:
```
DrawStart 1234567890123
DrawEnd 1234567890456
LayoutStart 1234567890500
LayoutEnd 1234567890800
FlushStart 1234567891000
FlushEnd 1234567891500
```

---

### 3.4 MinGW 平台桩实现

#### 3.4.1 src/mingw/frame_collector.cpp

**功能**: MinGW 平台的 FrameCollector 空实现，所有方法为 no-op。

**原因**: MinGW 用于 Windows 平台的预览开发场景，不需要实际的帧采集功能。

#### 3.4.2 src/mingw/frame_painter.cpp

**功能**: MinGW 平台的 FramePainter 空实现，`Draw()` 方法为 no-op。

---

### 3.5 构建配置

#### 3.5.1 BUILD.gn

**功能**: 定义 libframe_analyzer 共享库的构建配置。

**构建产物**: `libframe_analyzer.so`

**平台差异编译**:
```gn
ohos_shared_library("libframe_analyzer") {
  if (current_os == "ohos") {
    # OHOS 平台：完整实现
    sources = [
      "src/frame_collector.cpp",
      "src/frame_painter.cpp",
      "src/frame_saver.cpp",
    ]
  } else {
    # 非 OHOS 平台（MinGW）：桩实现
    sources = [
      "src/mingw/frame_collector.cpp",
      "src/mingw/frame_painter.cpp",
    ]
  }
}
```

**依赖关系**:
```gn
# OHOS 平台依赖
deps = [ "sandbox:sandbox_utils" ]                    # 沙箱路径工具

external_deps = [
    "c_utils:utils",          # C++ 工具库
    "hilog:libhilog",         # 日志库
    "hitrace:hitrace_meter",  # 追踪库
    "init:libbeget_proxy",    # 系统参数代理
    "init:libbegetutil",      # 系统参数工具
    "skia:skia_canvaskit",    # Skia 图形库（用于 Canvas 绘制）
]
```

**可选依赖**:
```gn
# 使用 Rosen Drawing 时额外依赖 2d_graphics 模块
if (use_rosen_drawing) {
    defines = [ "USE_ROSEN_DRAWING" ]
    deps += [ "2d_graphics:2d_graphics" ]
}
```
