/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <bits/alltypes.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_text_typography.h>
#include "sample_bitmap.h"
#include "common/log_common.h"

void SampleBitMap::SetWidth(uint64_t width) // 设置窗口宽度
{
    width_ = width;
}

void SampleBitMap::SetHeight(uint64_t height) //设置窗口高度
{
    height_ = height;
}

void SampleBitMap::SetNativeWindow(OHNativeWindow *nativeWindow) //设置窗口
{
    nativeWindow_ = nativeWindow;
}

void SampleBitMap::Prepare()
{
    if (nativeWindow_ == nullptr) {
        return;
    }

    // 通过 OH_NativeWindow_NativeWindowRequestBuffer 获取 OHNativeWindowBuffer 实例
    int ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow_, &buffer_, &fenceFd_);
    DRAWING_LOGI("request buffer ret = %{public}d", ret);
    // 通过 OH_NativeWindow_GetBufferHandleFromNative 获取 buffer 的 handle
    bufferHandle_ = OH_NativeWindow_GetBufferHandleFromNative(buffer_);
    // 使用系统mmap接口拿到bufferHandle的内存虚拟地址
    mappedAddr_ = static_cast<uint32_t *>(
        mmap(bufferHandle_->virAddr, bufferHandle_->size, PROT_READ | PROT_WRITE, MAP_SHARED, bufferHandle_->fd, 0));
    if (mappedAddr_ == MAP_FAILED) {
        return;
    }
}

void SampleBitMap::Create() // 创建bitmap 和 canvas 并把它们关联
{
    uint32_t width = static_cast<uint32_t>(bufferHandle_->stride / 4);
    // 创建一个bitmap对象
    cBitmap_ = OH_Drawing_BitmapCreate();
    // 定义bitmap的像素格式
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    // 构造对应格式的bitmap
    OH_Drawing_BitmapBuild(cBitmap_, width, height_, &cFormat);

    // 创建一个canvas对象
    cCanvas_ = OH_Drawing_CanvasCreate();
    // 将画布与bitmap绑定，画布画的内容会输出到绑定的bitmap内存中
    OH_Drawing_CanvasBind(cCanvas_, cBitmap_);
    // 使用白色清除画布内容
    OH_Drawing_CanvasClear(cCanvas_, OH_Drawing_ColorSetArgb(0xFF, 0xCD, 0xCD, 0x0));
}

void SampleBitMap::ConstructPath()
{
    static int blockSize = 5;
    static int x2Offset = 2;
    static int x3Offset = 3;
    static int x4Offset = 4;
    float x1 = width_ / blockSize * x2Offset;
    float y1 = height_ / blockSize;
    float x2 = width_ / blockSize * x3Offset;
    float y2 = height_ / blockSize;
    float x3 = width_ / blockSize * x3Offset;
    float y3 = height_ / blockSize * x2Offset;
    float x4 = width_ / blockSize * x4Offset;
    float y4 = height_ / blockSize * x2Offset;
    float x5 = width_ / blockSize * x4Offset;
    float y5 = height_ / blockSize * x3Offset;
    float x6 = width_ / blockSize * x3Offset;
    float y6 = height_ / blockSize * x3Offset;
    float x7 = width_ / blockSize * x3Offset;
    float y7 = height_ / blockSize * x4Offset;
    float x8 = width_ / blockSize * x2Offset;
    float y8 = height_ / blockSize * x4Offset;
    float x9 = width_ / blockSize * x2Offset;
    float y9 = height_ / blockSize * x3Offset;
    float x10 = width_ / blockSize;
    float y10 = height_ / blockSize * x3Offset;
    float x11 = width_ / blockSize;
    float y11 = height_ / blockSize * x2Offset;
    float x12 = width_ / blockSize * x2Offset;
    float y12 = height_ / blockSize * x2Offset;

    cPath_ = OH_Drawing_PathCreate();
    // 指定path的起始位置
    OH_Drawing_PathMoveTo(cPath_, x1, y1);
    // 用直线连接到目标点
    OH_Drawing_PathLineTo(cPath_, x2, y2);
    OH_Drawing_PathLineTo(cPath_, x3, y3);
    OH_Drawing_PathLineTo(cPath_, x4, y4);
    OH_Drawing_PathLineTo(cPath_, x5, y5);
    OH_Drawing_PathLineTo(cPath_, x6, y6);
    OH_Drawing_PathLineTo(cPath_, x7, y7);
    OH_Drawing_PathLineTo(cPath_, x8, y8);
    OH_Drawing_PathLineTo(cPath_, x9, y9);
    OH_Drawing_PathLineTo(cPath_, x10, y10);
    OH_Drawing_PathLineTo(cPath_, x11, y11);
    OH_Drawing_PathLineTo(cPath_, x12, y12);
    // 闭合形状，path绘制完毕
    OH_Drawing_PathClose(cPath_);
}

// 设置画笔的宽度和颜色
void SampleBitMap::SetPenAndBrush()
{
    constexpr float penWidth = 40.0f; // pen width 40.0

    cPen_ = OH_Drawing_PenCreate(); // 创建一个画笔Pen对象，Pen对象用于形状的边框线绘制
    OH_Drawing_PenSetAntiAlias(cPen_, true);
    OH_Drawing_PenSetColor(cPen_, OH_Drawing_ColorSetArgb(0xFF, 0x0, 0x0, 0xCD));
    OH_Drawing_PenSetWidth(cPen_, penWidth);
    OH_Drawing_PenSetJoin(cPen_, LINE_ROUND_JOIN);

    // 将Pen画笔设置到canvas中
    OH_Drawing_CanvasAttachPen(cCanvas_, cPen_);
}

void SampleBitMap::DrawPath()
{
    // 在画布上画path的形状，边框样式为pen设置，颜色填充为Brush设置
    OH_Drawing_CanvasDrawPath(cCanvas_, cPath_);
}

void SampleBitMap::DisPlay()
{
    // 画完后获取像素地址，地址指向的内存包含画布画的像素数据
    void *bitmapAddr = OH_Drawing_BitmapGetPixels(cBitmap_);
    uint32_t *value = static_cast<uint32_t *>(bitmapAddr);

    uint32_t *pixel = static_cast<uint32_t *>(mappedAddr_); // 使用mmap获取到的地址来访问内存
    if (pixel == nullptr) {
        return;
    }
    if (value == nullptr) {
        return;
    }
    uint32_t width = static_cast<uint32_t>(bufferHandle_->stride / 4); // 4 is offset
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0; y < height_; y++) {
            *pixel++ = *value++;
        }
    }
    // 设置刷新区域，如果Region中的Rect为nullptr,或者rectNumber为0，则认为OHNativeWindowBuffer全部有内容更改。
    Region region {nullptr, 0};
    // 通过OH_NativeWindow_NativeWindowFlushBuffer 提交给消费者使用，例如：显示在屏幕上。
    OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow_, buffer_, fenceFd_, region);
    // 内存使用完记得去掉内存映射
    int result = munmap(mappedAddr_, bufferHandle_->size);
    if (result == -1) {
        return;
    }
}

void SampleBitMap::Destroy()
{
    // 销毁创建的对象
    OH_Drawing_PenDestroy(cPen_);
    cPen_ = nullptr;
    OH_Drawing_PathDestroy(cPath_);
    cPath_ = nullptr;
    // 销毁canvas对象
    OH_Drawing_CanvasDestroy(cCanvas_);
    cCanvas_ = nullptr;
    // 销毁bitmap对象
    OH_Drawing_BitmapDestroy(cBitmap_);
}

napi_value SampleBitMap::NapiDrawPattern(napi_env env, napi_callback_info info)
{
    DRAWING_LOGI("NapiDrawPattern");
    if ((env == nullptr) || (info == nullptr)) {
        return nullptr;
    }

    napi_value thisArg;
    if (napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, nullptr) != napi_ok) {
        return nullptr;
    }

    napi_value exportInstance;
    if (napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        return nullptr;
    }
    DRAWING_LOGI("ID = %{public}s", idStr);
    std::string id(idStr);
    SampleBitMap *render = SampleBitMap().GetInstance(id);
    if (render != nullptr) {
        render->Prepare();
        render->Create();
        render->ConstructPath();
        render->SetPenAndBrush();
        render->DrawPath();
        render->DisPlay();
        render->Destroy();
        DRAWING_LOGI("DrawPath executed");
    }
    return nullptr;
}

void SampleBitMap::Export(napi_env env, napi_value exports) // 定义并导出接口drawPattern
{
    if ((env == nullptr) || (exports == nullptr)) {
        return;
    }
    napi_property_descriptor desc[] = {
        {"drawPattern", nullptr, SampleBitMap::NapiDrawPattern, nullptr, nullptr, nullptr, napi_default, nullptr},
    };
    if (napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc) != napi_ok) {
        return;
    }
}

SampleBitMap::~SampleBitMap()
{
    // 销毁创建的对象
    OH_Drawing_PenDestroy(cPen_);
    cPen_ = nullptr;
    OH_Drawing_PathDestroy(cPath_);
    cPath_ = nullptr;
    // 销毁canvas对象
    OH_Drawing_CanvasDestroy(cCanvas_);
    cCanvas_ = nullptr;
    // 销毁bitmap对象
    OH_Drawing_BitmapDestroy(cBitmap_);
    cBitmap_ = nullptr;

    buffer_ = nullptr;
    bufferHandle_ = nullptr;
    nativeWindow_ = nullptr;
    mappedAddr_ = nullptr;
}

static std::unordered_map<std::string, SampleBitMap *> g_instance;

void SampleBitMap::Release(std::string &id) //将该id对应的窗口释放
{
    SampleBitMap *render = SampleBitMap::GetInstance(id);
    if (render != nullptr) {
        delete render;
        render = nullptr;
        g_instance.erase(g_instance.find(id));
    }
}

SampleBitMap *SampleBitMap::GetInstance(std::string &id) // 通过ComponentId 获取实例，没有新建
{
    if (g_instance.find(id) == g_instance.end()) {
        SampleBitMap *render = new SampleBitMap(id);
        g_instance[id] = render;
        return render;
    } else {
        return g_instance[id];
    }
}

static void OnSurfaceCreatedCB(OH_NativeXComponent *component,
                               void *window) // 通过component获取ComponentId，获取实例，设置窗口及其尺寸
{
    DRAWING_LOGI("OnSurfaceCreatedCB");
    if ((component == nullptr) || (window == nullptr)) {
        return;
    }
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(component, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        return;
    }
    std::string id(idStr);
    auto render = SampleBitMap::GetInstance(id);
    OHNativeWindow *nativeWindow = static_cast<OHNativeWindow *>(window);
    render->SetNativeWindow(nativeWindow);

    uint64_t width;
    uint64_t height;
    int32_t xSize = OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    if ((xSize == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) && (render != nullptr)) {
        render->SetHeight(height);
        render->SetWidth(width);
        DRAWING_LOGI("xComponent width = %{public}llu, height = %{public}llu", width, height);
    }
}

static void OnSurfaceDestroyedCB(OH_NativeXComponent *component, void *window) // 通过component获取ComponentId，销毁实例
{
    DRAWING_LOGI("OnSurfaceDestroyedCB");
    if ((component == nullptr) || (window == nullptr)) {
        return;
    }
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(component, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        return;
    }
    std::string id(idStr);
    SampleBitMap::Release(id);
}

void SampleBitMap::RegisterCallback(OH_NativeXComponent *nativeXComponent) //注册nativeXComponent 回调函数
{
    DRAWING_LOGI("register callback");
    renderCallback_.OnSurfaceCreated = OnSurfaceCreatedCB;
    renderCallback_.OnSurfaceDestroyed = OnSurfaceDestroyedCB;
    // Callback must be initialized
    renderCallback_.DispatchTouchEvent = nullptr;
    renderCallback_.OnSurfaceChanged = nullptr;
    OH_NativeXComponent_RegisterCallback(nativeXComponent, &renderCallback_);
}

