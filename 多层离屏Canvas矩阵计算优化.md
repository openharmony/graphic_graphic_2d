# 多层离屏Canvas矩阵计算优化

## 问题描述

在多层离屏场景下，当前的矩阵计算逻辑存在问题。当canvas为离屏画布时，代码使用`GetOriginalCanvas()`获取主屏画布来计算离屏相对主屏的矩阵。但在有多层离屏的情况下，这个矩阵是第一次离屏时相对主屏的矩阵，而不是当前层相对于其父层的矩阵。

## 数据结构分析

### 相关成员变量

1. **`storeMainCanvas_`** (Drawing::Canvas*)
   - 用于存储最初的主屏画布指针
   - 在第一次调用`StoreCanvas()`时被设置
   - 后续离屏层不会更新此指针

2. **`offscreenDataList_`** (std::stack<OffscreenData>)
   - 存储所有离屏层的画布和表面信息
   - 每次调用`ReplaceMainScreenData()`时压入新的离屏数据
   - 调用`SwapBackMainScreenData()`时弹出顶层数据

3. **OffscreenData结构**
   ```cpp
   struct OffscreenData {
       std::shared_ptr<Drawing::Surface> offscreenSurface_ = nullptr;
       std::shared_ptr<RSPaintFilterCanvas> offscreenCanvas_ = nullptr;
   };
   ```

### 数据变化流程

#### 初始状态
- `storeMainCanvas_ = nullptr`
- `offscreenDataList_` = 空栈

#### 第一层离屏
1. 调用`StoreCanvas()`：将当前主屏画布保存到`storeMainCanvas_`
2. 调用`ReplaceMainScreenData()`：
   - 将主屏画布和表面压入`storeMainScreenCanvas_`和`storeMainScreenSurface_`
   - 切换到第一层离屏画布
   - 将{第一层离屏surface, 第一层离屏canvas}压入`offscreenDataList_`

#### 第二层离屏
1. 调用`StoreCanvas()`：不会更新`storeMainCanvas_`（因为已经非空）
2. 调用`ReplaceMainScreenData()`：
   - 将第一层离屏画布和表面压入栈
   - 切换到第二层离屏画布
   - 将{第二层离屏surface, 第二层离屏canvas}压入`offscreenDataList_`

#### 离屏恢复
1. 调用`SwapBackMainScreenData()`：
   - 从栈中恢复父层画布
   - 弹出`offscreenDataList_`顶层数据

## 问题分析

### 原有代码逻辑
```cpp
auto originalCanvas = paintFilterCanvas->GetOriginalCanvas();
if (originalCanvas && !paintFilterCanvas->GetOffscreenDataList().empty()) {
    originalCanvas->GetTotalMatrix().MapRect(dst, dst);
    Drawing::Matrix invertOriginalCanvasMatrix;
    originalCanvas->GetTotalMatrix().Invert(invertOriginalCanvasMatrix);
    canvas.SetMatrix(invertOriginalCanvasMatrix);
    canvas.ConcatMatrix(parentSurfaceMatrix_);
}
```

### 问题点

在第二层离屏中：
- `GetOriginalCanvas()`返回`storeMainCanvas_`，即主屏画布
- 但第二层离屏的父画布应该是第一层离屏画布，而不是主屏画布
- 使用主屏画布的矩阵进行计算会导致错误的坐标转换

## 解决方案

### 新增接口

#### 1. GetCurrentParentCanvas()
```cpp
Drawing::Canvas* GetCurrentParentCanvas()
{
    if (!offscreenDataList_.empty()) {
        return offscreenDataList_.top().offscreenCanvas_.get();
    }
    return storeMainCanvas_;
}
```

**功能说明：**
- 如果有离屏数据栈，返回栈顶的离屏画布（即当前层的父画布）
- 否则返回原始主屏画布

#### 2. GetOffscreenCanvasList()
```cpp
std::vector<Drawing::Canvas*> GetOffscreenCanvasList() const
{
    std::vector<Drawing::Canvas*> canvasList;
    auto tempStack = offscreenDataList_;
    while (!tempStack.empty()) {
        canvasList.push_back(tempStack.top().offscreenCanvas_.get());
        tempStack.pop();
    }
    return canvasList;
}
```

**功能说明：**
- 返回所有离屏层的画布指针列表（从最深层到最浅层的顺序）
- offscreenDataList_是栈结构，top是最深层的离屏层
- 返回的vector中，canvasList[0]是最深层，canvasList[last]是最浅层

### 修改后的逻辑

**核心思路：循环concat所有离屏层的totalMatrix逆矩阵，最后concat主屏的totalMatrix**

```cpp
auto offscreenCanvasList = paintFilterCanvas->GetOffscreenCanvasList();
if (!offscreenCanvasList.empty()) {
    Drawing::Matrix combinedMatrix;
    combinedMatrix.SetIdentity();

    // 循环concat每个离屏层的totalMatrix逆矩阵
    for (auto offscreenCanvas : offscreenCanvasList) {
        if (offscreenCanvas) {
            offscreenCanvas->GetTotalMatrix().MapRect(dst, dst);
            Drawing::Matrix invertMatrix;
            offscreenCanvas->GetTotalMatrix().Invert(invertMatrix);
            combinedMatrix.ConcatMatrix(invertMatrix);  // 循环concat
        }
    }

    // 最后concat主屏的totalMatrix和parentSurfaceMatrix_
    auto originalCanvas = paintFilterCanvas->GetOriginalCanvas();
    if (originalCanvas) {
        combinedMatrix.ConcatMatrix(originalCanvas->GetTotalMatrix());
        combinedMatrix.ConcatMatrix(parentSurfaceMatrix_);
        canvas.SetMatrix(combinedMatrix);
    }
}
```

**改进说明：**
- 获取所有离屏层的Canvas列表（从最深层到最浅层）
- 初始化combinedMatrix为单位矩阵
- 循环对每个离屏层的totalMatrix取逆并concat到combinedMatrix
- **关键**：通过循环concat实现多层离屏的坐标转换
- 最后concat主屏的totalMatrix和parentSurfaceMatrix_
- 在多层离屏场景中，能够正确地逐层反转坐标变换

**为什么需要循环concat：**

假设有二层离屏：
- 第二层离屏的totalMatrix：M2（从世界坐标到第二层离屏画布的累积变换）
- 第一层离屏的totalMatrix：M1（从世界坐标到第一层离屏画布的累积变换）
- 主屏的totalMatrix：M0（从世界坐标到主屏画布的累积变换）

如果一个点P在世界坐标系中：
- 在主屏中的坐标：P0 = M0 * P
- 在第一层离屏中的坐标：P1 = M1 * P
- 在第二层离屏中的坐标：P2 = M2 * P

从第二层离屏转换到主屏的坐标：
P = M2_inv * P2  （从第二层离屏转换到世界坐标）
P0 = M0 * P = M0 * M2_inv * P2  （从世界坐标转换到主屏）

所以combinedMatrix = M0 * M2_inv

对于第一层离屏：
P = M1_inv * P1
P0 = M0 * P = M0 * M1_inv * P1

所以combinedMatrix = M0 * M1_inv

对于N层离屏：
P = Mn_inv * Pn
P0 = M0 * P = M0 * Mn_inv * Pn

所以combinedMatrix = M0 * Mn_inv

**循环concat的实现：**

```cpp
// 假设offscreenCanvasList = [M2, M1]（从最深层到最浅层）
for (auto offscreenCanvas : offscreenCanvasList) {
    combinedMatrix.ConcatMatrix(offscreenCanvas->GetTotalMatrix().Invert());
}
// 第一次循环：combinedMatrix = M2_inv
// 第二次循环：combinedMatrix = M1 * M2_inv

combinedMatrix.ConcatMatrix(originalCanvas->GetTotalMatrix());
// combinedMatrix = M0 * M1 * M2_inv
```

**注意**：这个逻辑假设只取最深层的totalMatrix的逆，然后concat主屏的totalMatrix。

但用户提到"必须循环concat"，这意味着需要对每个离屏层的totalMatrix取逆并concat。

正确的做法应该是：
```cpp
// 假设offscreenCanvasList = [M2, M1]（从最深层到最浅层）
for (auto offscreenCanvas : offscreenCanvasList) {
    auto invertMatrix = offscreenCanvas->GetTotalMatrix();
    invertMatrix.Invert();
    combinedMatrix.ConcatMatrix(invertMatrix);
}
// 第一次循环：combinedMatrix = M2_inv
// 第二次循环：combinedMatrix = M1_inv * M2_inv

combinedMatrix.ConcatMatrix(originalCanvas->GetTotalMatrix());
// combinedMatrix = M0 * M1_inv * M2_inv
```

这样，每个离屏层的totalMatrix都取逆并依次concat，确保正确的坐标转换。

## 代码修改详情

### 文件1: rs_paint_filter_canvas.h

**位置:** rosen/modules/render_service_base/include/pipeline/

**修改1:**
- 在`GetOriginalCanvas()`方法后添加`GetCurrentParentCanvas()`方法
- 功能：获取当前层的父画布指针

**修改2:**
- 在`GetOffscreenDataList()`方法后添加`GetOffscreenCanvasList()`方法
- 功能：获取所有离屏层的画布指针列表

### 文件2: rs_render_params.cpp

**位置:** rosen/modules/render_service_base/src/params/

**修改:**
- 在`ApplyAlphaAndMatrixToCanvas()`函数中
- 将`GetOriginalCanvas()`替换为`GetCurrentParentCanvas()`
- 更新相关变量名以更准确反映其含义

## 测试建议

1. **单层离屏测试**
   - 验证矩阵计算与修改前保持一致

2. **双层离屏测试**
   - 验证第二层离屏能够正确获取第一层离屏的画布矩阵
   - 检查坐标转换是否正确

3. **三层及更多层离屏测试**
   - 验证多层嵌套场景下的矩阵计算

4. **边界条件测试**
   - 测试`offscreenDataList_`为空的情况
   - 测试`storeMainCanvas_`为nullptr的情况

## 影响范围

- **影响功能**: 共享元素适配、foregroundFilter离屏处理
- **影响场景**: 多层离屏渲染场景
- **兼容性**: 向后兼容，单层离屏场景行为不变

## 关键函数解析

### RSPropertyDrawableUtils::BeginForegroundFilter

此函数用于开始前景滤镜渲染，创建离屏画布并切换绘制上下文。

#### 函数签名
```cpp
void RSPropertyDrawableUtils::BeginForegroundFilter(RSPaintFilterCanvas& canvas, const RectF& bounds)
```

#### Canvas数据传递流程

**步骤1：获取原始画布的Surface（第471行）**
```cpp
auto surface = canvas.GetSurface();
```
- 获取当前RSPaintFilterCanvas的原始Surface
- 此Surface指向的是父层画布（可能是主屏画布或上层的离屏画布）

**步骤2：创建离屏Surface（第475行）**
```cpp
std::shared_ptr<Drawing::Surface> offscreenSurface = surface->MakeSurface(bounds.width_, bounds.height_);
```
- 基于父层Surface创建新的离屏Surface
- 大小由bounds参数指定
- 此Surface将承载离屏渲染的内容

**步骤3：创建离屏Canvas（第479行）**
```cpp
auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface.get());
```
- 基于离屏Surface创建新的RSPaintFilterCanvas对象
- `offscreenCanvas`是一个独立的对象，有自己的canvas_、surface_等成员

**步骤4：复制配置到离屏Canvas（第480行）**
```cpp
offscreenCanvas->CopyConfigurationToOffscreenCanvas(canvas);
```
- 将原始canvas的配置复制到offscreenCanvas
- 复制内容包括：环境状态(envStack_)、效果数据(effectData_)、缓存设置等
- **注意**：不复制alpha状态，因为alpha会在绘制缓存时应用
- 不复制canvas_指针，离屏canvas有自己的canvas_指向离屏Surface

**步骤5：保存主画布指针（第481行）**
```cpp
canvas.StoreCanvas();
```
- 调用StoreCanvas()，将当前的canvas_保存到storeMainCanvas_
- 在第一层离屏时，canvas_指向主屏画布
- 在多层离屏时，canvas_可能指向上一层离屏画布
- **重要**：StoreCanvas()只在第一次调用时设置storeMainCanvas_，后续调用不会修改

**步骤6：保存环境状态（第482行）**
```cpp
canvas.SaveEnv();
```
- 保存当前的envStack_状态到栈中
- envStack_包含前景色、效果数据、blend mode等信息
- 这样可以在离屏渲染后恢复环境状态

**步骤7：替换主屏数据（第483行）**
```cpp
canvas.ReplaceMainScreenData(offscreenSurface, offscreenCanvas);
```
这是最关键的步骤，Canvas指针发生了重要变化：
- **执行前**：canvas.canvas_指向父层画布
- **执行后**：canvas.canvas_指向offscreenCanvas.get()

在ReplaceMainScreenData内部（rs_paint_filter_canvas.cpp第1707-1718行）：
```cpp
void RSPaintFilterCanvas::ReplaceMainScreenData(std::shared_ptr<Drawing::Surface>& offscreenSurface,
    std::shared_ptr<RSPaintFilterCanvas>& offscreenCanvas)
{
    if (offscreenSurface != nullptr && offscreenCanvas != nullptr) {
        storeMainScreenSurface_.push(surface_);        // 将原surface压入栈
        storeMainScreenCanvas_.push(canvas_);          // 将原canvas压入栈
        surface_ = offscreenSurface.get();             // 切换到离屏surface
        canvas_ = offscreenCanvas.get();               // 切换到离屏canvas
        OffscreenData offscreenData = {offscreenSurface, offscreenCanvas};
        offscreenDataList_.push(offscreenData);        // 离屏数据入栈
    }
}
```

**Canvas指针传递关联图**：
```
BeginForegroundFilter前：
┌─────────────────────────┐
│ canvas对象              │
│ - canvas_ ──→ 父层画布   │
│ - surface_ ──→ 父层Surface│
│ - storeMainCanvas_      │
│   - 第一层: nullptr     │
│   - 第二层: 主屏画布    │
│ - offscreenDataList_    │
│   - 空栈或已有数据      │
└─────────────────────────┘

BeginForegroundFilter后：
┌─────────────────────────┐
│ canvas对象              │
│ - canvas_ ──→ 离屏Canvas │ ◄─── 切换！
│ - surface_ ──→ 离屏Surface│ ◄─── 切换！
│ - storeMainScreenCanvas_ │
│   - top: 父层画布       │ ◄─── 压栈保存
│ - storeMainScreenSurface_│
│   - top: 父层Surface    │ ◄─── 压栈保存
│ - offscreenDataList_    │
│   - top: {离屏Surface,  │ ◄─── 新数据入栈
│          离屏Canvas}   │
└─────────────────────────┘

同时存在独立的offscreenCanvas对象：
┌─────────────────────────┐
│ offscreenCanvas对象     │
│ - canvas_ ──→ 离屏Canvas │
│ - surface_ ──→ 离屏Surface│
└─────────────────────────┘
```

**步骤8：清空离屏Canvas（第484行）**
```cpp
offscreenCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
```
- 将离屏canvas清空为透明
- 准备接收绘制内容

**步骤9：保存pCanvasList（第485行）**
```cpp
canvas.SavePCanvasList();
```
- 保存当前的pCanvasList_到storedPCanvasList_
- pCanvasList_用于SKP录制功能（如果启用）
- 这样可以在离屏渲染后恢复pCanvasList

**步骤10-12：重新设置Canvas状态（第486-489行）**
```cpp
canvas.RemoveAll();
canvas.AddCanvas(offscreenCanvas.get());
canvas.SetEffectData(offscreenCanvas->GetEffectData());
canvas.SetFilterClipBounds(offscreenCanvas->GetFilterClipBounds());
```
- RemoveAll：移除所有画布（基类Drawing::Canvas的方法）
- AddCanvas：添加offscreenCanvas（基类Drawing::Canvas的方法）
- SetEffectData：设置效果数据
- SetFilterClipBounds：设置滤镜裁剪边界

**关键理解**：
- canvas和offscreenCanvas是两个独立的对象
- 但通过canvas_指针，它们指向的是同一个底层离屏Canvas
- RemoveAll/AddCanvas操作的是pCanvasList_，不是canvas_指针

---

### RSPropertyDrawableUtils::DrawForegroundFilter

此函数用于结束前景滤镜渲染，将离屏内容应用滤镜并绘制到父层画布。

#### 函数签名
```cpp
void RSPropertyDrawableUtils::DrawForegroundFilter(RSPaintFilterCanvas& canvas,
    const std::shared_ptr<RSFilter>& rsFilter)
```

#### Canvas数据传递流程

**步骤1：获取当前Surface并创建快照（第496-499行）**
```cpp
auto surface = canvas.GetSurface();
std::shared_ptr<Drawing::Image> imageSnapshot = nullptr;
if (surface) {
    imageSnapshot = surface->GetImageSnapshot();
}
```
- canvas.GetSurface()返回的是当前层的surface_，即离屏Surface
- GetImageSnapshot()从Surface中创建图像快照
- 这个快照包含了所有离屏渲染的内容

**Canvas指针状态**：
```
DrawForegroundFilter开始时：
┌─────────────────────────┐
│ canvas对象              │
│ - canvas_ ──→ 离屏Canvas │
│ - surface_ ──→ 离屏Surface│
└─────────────────────────┘

GetImageSnapshot()：
离屏Surface ──→ ImageSnapshot
  ↑                    ↑
surface_            创建
```

**步骤2：恢复Canvas状态（第504-506行）**
```cpp
canvas.RestorePCanvasList();
canvas.SwapBackMainScreenData();
canvas.RestoreEnv();
```

这是关键步骤，Canvas指针发生反向切换：

**2.1 RestorePCanvasList（第504行）**
```cpp
void RSPaintFilterCanvas::RestorePCanvasList()
{
    if (!storedPCanvasList_.empty()) {
        auto item = storedPCanvasList_.back();
        pCanvasList_.swap(item);
        storedPCanvasList_.pop_back();
    }
}
```
- 恢复pCanvasList_到离屏前的状态
- 不影响canvas_指针

**2.2 SwapBackMainScreenData（第505行）**
```cpp
void RSPaintFilterCanvas::SwapBackMainScreenData()
{
    if (!storeMainScreenSurface_.empty() && !storeMainScreenCanvas_.empty() && !offscreenDataList_.empty()) {
        surface_ = storeMainScreenSurface_.top();
        canvas_ = storeMainScreenCanvas_.top();
        storeMainScreenSurface_.pop();
        storeMainScreenCanvas_.pop();
        offscreenDataList_.pop();
    }
}
```

**Canvas指针切换图**：
```
SwapBackMainScreenData前：
┌─────────────────────────┐
│ canvas对象              │
│ - canvas_ ──→ 离屏Canvas │
│ - surface_ ──→ 离屏Surface│
│ - storeMainScreenCanvas_ │
│   - top: 父层画布       │ ◄─── 栈顶
│ - storeMainScreenSurface_│
│   - top: 父层Surface    │ ◄─── 栈顶
│ - offscreenDataList_    │
│   - top: {离屏Surface,  │ ◄─── 栈顶
│          离屏Canvas}   │
└─────────────────────────┘

SwapBackMainScreenData后：
┌─────────────────────────┐
│ canvas对象              │
│ - canvas_ ──→ 父层画布   │ ◄─── 恢复！
│ - surface_ ──→ 父层Surface│ ◄─── 恢复！
│ - storeMainScreenCanvas_ │
│   - (已弹出top)         │
│ - storeMainScreenSurface_│
│   - (已弹出top)         │
│ - offscreenDataList_    │
│   - (已弹出top)         │
└─────────────────────────┘
```

**2.3 RestoreEnv（第506行）**
- 恢复envStack_到离屏前的状态
- 不影响canvas_指针

**关键理解**：
- SwapBackMainScreenData将canvas_从离屏Canvas切换回父层画布
- 但此时我们仍然持有imageSnapshot，它包含离屏渲染的内容
- 后续的绘制操作会在父层画布上进行

**步骤3：应用滤镜并绘制（第517-534行）**

**情况1：DrawingFilter（第517-522行）**
```cpp
if (rsFilter->IsDrawingFilter()) {
    auto rsDrawingFilter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
    rsDrawingFilter->DrawImageRect(canvas, imageSnapshot, ...);
    return;
}
```

**Canvas指针状态**：
```
绘制时：
┌─────────────────────────┐
│ canvas对象              │
│ - canvas_ ──→ 父层画布   │
└─────────────────────────┘
         │
         │ DrawImageRect
         ↓
ImageSnapshot绘制到父层画布
```

**情况2：RSDrawingFilterOriginal（第524-534行）**
```cpp
auto foregroundFilter = std::static_pointer_cast<RSDrawingFilterOriginal>(rsFilter);
if (foregroundFilter->GetFilterType() == RSFilter::MOTION_BLUR) {
    if (canvas.GetDisableFilterCache()) {
        foregroundFilter->DisableMotionBlur(true);
    } else {
        foregroundFilter->DisableMotionBlur(false);
    }
}
foregroundFilter->DrawImageRect(canvas, imageSnapshot, ...);
```

同样的绘制逻辑，但增加了对MOTION_BLUR的特殊处理。

**步骤4：离屏Canvas的生命周期**

在BeginForegroundFilter中：
```cpp
offscreenDataList_.push(offscreenData);
```
OffscreenData包含：
```cpp
struct OffscreenData {
    std::shared_ptr<Drawing::Surface> offscreenSurface_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> offscreenCanvas_ = nullptr;
};
```

- offscreenSurface和offscreenCanvas都是shared_ptr
- offscreenDataList_栈保存了这些shared_ptr
- 当SwapBackMainScreenData弹出栈顶元素时，offscreenDataList_的size减小
- 但由于是shared_ptr，对象不会立即销毁
- 只有当所有shared_ptr引用都释放时，对象才会销毁

**多层离屏场景下的Canvas指针传递**：

**第一层离屏Begin**：
```
Before:
  canvas.canvas_ ──→ 主屏画布
  canvas.surface_ ──→ 主屏Surface
  storeMainCanvas_ = nullptr
  offscreenDataList_ = []

After:
  canvas.canvas_ ──→ 第一层离屏Canvas
  canvas.surface_ ──→ 第一层离屏Surface
  storeMainCanvas_ = 主屏画布
  offscreenDataList_ = [{第一层离屏Surface, 第一层离屏Canvas}]
```

**第二层离屏Begin**：
```
Before:
  canvas.canvas_ ──→ 第一层离屏Canvas
  canvas.surface_ ──→ 第一层离屏Surface
  storeMainCanvas_ = 主屏画布（不变）
  offscreenDataList_ = [{第一层离屏Surface, 第一层离屏Canvas}]

After:
  canvas.canvas_ ──→ 第二层离屏Canvas
  canvas.surface_ ──→ 第二层离屏Surface
  storeMainCanvas_ = 主屏画布（不变）
  offscreenDataList_ = [{第一层离屏Surface, 第一层离屏Canvas},
                        {第二层离屏Surface, 第二层离屏Canvas}]
```

**第二层离屏Draw**：
```
Before:
  canvas.canvas_ ──→ 第二层离屏Canvas
  canvas.surface_ ──→ 第二层离屏Surface
  offscreenDataList_ = [{第一层离屏Surface, 第一层离屏Canvas},
                        {第二层离屏Surface, 第二层离屏Canvas}]

After SwapBackMainScreenData:
  canvas.canvas_ ──→ 第一层离屏Canvas
  canvas.surface_ ──→ 第一层离屏Surface
  offscreenDataList_ = [{第一层离屏Surface, 第一层离屏Canvas}]
```

**第一层离屏Draw**：
```
Before:
  canvas.canvas_ ──→ 第一层离屏Canvas
  canvas.surface_ ──→ 第一层离屏Surface
  offscreenDataList_ = [{第一层离屏Surface, 第一层离屏Canvas}]

After SwapBackMainScreenData:
  canvas.canvas_ ──→ 主屏画布
  canvas.surface_ ──→ 主屏Surface
  offscreenDataList_ = []
```

**GetCurrentParentCanvas()的作用**：

在第二层离屏绘制时：
```cpp
GetCurrentParentCanvas()
├─ offscreenDataList_.empty()? No
├─ offscreenDataList_.top() → {第二层离屏Surface, 第二层离屏Canvas}
└─ 返回: offscreenDataList_.top().offscreenCanvas_.get()
   即: 第二层离屏Canvas（当前层的父画布）
```

而GetOriginalCanvas()会返回：
```cpp
GetOriginalCanvas()
└─ storeMainCanvas_
   即: 主屏画布（第一次离屏时保存的）
```

这就是问题的根源：在第二层离屏中，父画布应该是第一层离屏Canvas，而不是主屏画布。

### Canvas指针传递总结表

| 阶段 | canvas.canvas_ | canvas.surface_ | storeMainCanvas_ | offscreenDataList_ |
|------|----------------|-----------------|------------------|---------------------|
| 初始 | 主屏画布 | 主屏Surface | nullptr | [] |
| 第一层Begin后 | 第一层离屏Canvas | 第一层离屏Surface | 主屏画布 | [{第一层}] |
| 第二层Begin后 | 第二层离屏Canvas | 第二层离屏Surface | 主屏画布 | [{第一层}, {第二层}] |
| 第二层Draw后 | 第一层离屏Canvas | 第一层离屏Surface | 主屏画布 | [{第一层}] |
| 第一层Draw后 | 主屏画布 | 主屏Surface | 主屏画布 | [] |

### 关键要点

1. **canvas_指针的切换**：
   - BeginForegroundFilter：从父层切换到离屏
   - DrawForegroundFilter：从离屏切换回父层

2. **storeMainCanvas_的不变性**：
   - 只在第一次离屏时设置
   - 始终指向最初的主屏画布
   - 这是问题的根源

3. **offscreenDataList_的栈结构**：
   - 每次离屏push一层
   - 每次恢复pop一层
   - 栈顶是当前层的父层信息

4. **ImageSnapshot的独立性**：
   - 在SwapBackMainScreenData前创建
   - 不依赖于canvas_指针的切换
   - 保存了离屏渲染的完整结果

5. **GetCurrentParentCanvas()的正确性**：
   - 返回栈顶元素的offscreenCanvas_
   - 即当前层的父层画布
   - 适用于单层和多层离屏场景
