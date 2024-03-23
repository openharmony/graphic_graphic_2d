/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_typography.h>
#include "common/log_common.h"
#include "sample_bitmap.h"

static void OnSurfaceCreatedCB(OH_NativeXComponent *component, void *window)
{
    DRAWING_LOGI("OnSurfaceCreatedCB");
    if ((component == nullptr) || (window == nullptr)) {
        DRAWING_LOGE("OnSurfaceCreatedCB: component or window is null");
        return;
    }
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(component, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("OnSurfaceCreatedCB: Unable to get XComponent id");
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

static void OnSurfaceDestroyedCB(OH_NativeXComponent *component, void *window)
{
    DRAWING_LOGI("OnSurfaceDestroyedCB");
    if ((component == nullptr) || (window == nullptr)) {
        DRAWING_LOGE("OnSurfaceDestroyedCB: component or window is null");
        return;
    }
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(component, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("OnSurfaceDestroyedCB: Unable to get XComponent id");
        return;
    }
    std::string id(idStr);
    SampleBitMap::Release(id);
}

static std::unordered_map<std::string, SampleBitMap *> g_instance;

void SampleBitMap::SetWidth(uint64_t width)
{
    width_ = width;
}

void SampleBitMap::SetHeight(uint64_t height)
{
    height_ = height;
}

void SampleBitMap::SetNativeWindow(OHNativeWindow *nativeWindow)
{
    nativeWindow_ = nativeWindow;
}

void SampleBitMap::Prepare()
{
    if (nativeWindow_ == nullptr) {
        DRAWING_LOGE("nativeWindow_ is nullptr");
        return;
    }
    // 这里的nativeWindow是从上一步骤中的回调函数中获得的
    // 通过 OH_NativeWindow_NativeWindowRequestBuffer 获取 OHNativeWindowBuffer 实例
    int ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow_, &buffer_, &fenceFd_);
    DRAWING_LOGI("request buffer ret = %{public}d", ret);
    // 通过 OH_NativeWindow_GetBufferHandleFromNative 获取 buffer 的 handle
    bufferHandle_ = OH_NativeWindow_GetBufferHandleFromNative(buffer_);
    // 使用系统mmap接口拿到bufferHandle的内存虚拟地址
    mappedAddr_ = static_cast<uint32_t *>(
        mmap(bufferHandle_->virAddr, bufferHandle_->size, PROT_READ | PROT_WRITE, MAP_SHARED, bufferHandle_->fd, 0));
    if (mappedAddr_ == MAP_FAILED) {
        DRAWING_LOGE("mmap failed");
    }
}

void SampleBitMap::DisPlay()
{
    // 画完后获取像素地址，地址指向的内存包含画布画的像素数据
    void *bitmapAddr = OH_Drawing_BitmapGetPixels(cBitmap_);
    uint32_t *value = static_cast<uint32_t *>(bitmapAddr);

    uint32_t *pixel = static_cast<uint32_t *>(mappedAddr_); // 使用mmap获取到的地址来访问内存
    if (pixel == nullptr) {
        DRAWING_LOGE("pixel is null");
        return;
    }
    if (value == nullptr) {
        DRAWING_LOGE("value is null");
        return;
    }
    uint32_t width = static_cast<uint32_t>(bufferHandle_->stride / 4);
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
        DRAWING_LOGE("munmap failed!");
    }
}

void SampleBitMap::Create()
{
    uint32_t width = static_cast<uint32_t>(bufferHandle_->stride / 4);
    // 创建一个bitmap对象
    cBitmap_ = OH_Drawing_BitmapCreate();
    // 定义bitmap的像素格式
    OH_Drawing_BitmapFormat cFormat {COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    // 构造对应格式的bitmap
    OH_Drawing_BitmapBuild(cBitmap_, width, height_, &cFormat);

    // 创建一个canvas对象
    cCanvas_ = OH_Drawing_CanvasCreate();
    // 将画布与bitmap绑定，画布画的内容会输出到绑定的bitmap内存中
    OH_Drawing_CanvasBind(cCanvas_, cBitmap_);
    // 使用白色清除画布内容
    OH_Drawing_CanvasClear(cCanvas_, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
}

void SampleBitMap::DrawPathHelper()
{
    // 创建一个画刷Brush对象，Brush对象用于形状的填充
    cBrush_ = OH_Drawing_BrushCreate();
    float startX = width_ / 5; // divide w by 5 to get start x
    int distance = 50; // distance between earch point
    float startY = height_ / 4 + 2 * distance; // divide h by 4 and 2 times the distance to get start y
    startPt_ = OH_Drawing_PointCreate(startX, startY);
    float endX = width_ / 2; // divide w by 2 to get end x
    float endY = height_ / 4 + 6 * distance; // divide h by 4 and 6 times the distance to get end y
    endPt_ = OH_Drawing_PointCreate(endX, endY);
    uint32_t color[] = {0xffff0000, 0xff00ff00};
    float pos[] = {0., 1.0};
    // 创建并设置Brush的渐变效果
    uint32_t size = 2; // shader size 2
    shaderEffect_ =
        OH_Drawing_ShaderEffectCreateLinearGradient(startPt_, endPt_, color, pos, size, OH_Drawing_TileMode::CLAMP);
    OH_Drawing_BrushSetShaderEffect(cBrush_, shaderEffect_);

    // 将Brush画刷设置到canvas中
    OH_Drawing_CanvasAttachBrush(cCanvas_, cBrush_);
    // 绘制圆角矩形
    int x1 = 50; // x-axis point 1 50
    float rectY1 = height_ / 4 + 2 * distance; // divide h by 4 and 2 times the distance to get rect y1
    float rectY2 = height_ / 4 + 4 * distance; // divide h by 4 and 4 times the distance to get rect y2
    cRect_ = OH_Drawing_RectCreate(x1, rectY1, distance + width_ / 3, rectY2); // divide w by 3 to get rect x2
    int angle = 20; // round rect angle 20
    cRRect_ = OH_Drawing_RoundRectCreate(cRect_, angle, angle);
    OH_Drawing_PathReset(cPath_);
    OH_Drawing_PathAddRoundRect(cPath_, cRRect_, PATH_DIRECTION_CW);
    OH_Drawing_CanvasDrawPath(cCanvas_, cPath_);
    // 绘制黑色背景图片
    rectY1 = height_ / 2 + 2 * distance; // divide h by 2 and 2 times the distance to get rect y1
    rectY2 = height_ / 2 + 4 * distance; // divide h by 2 and 4 times the distance to get rect y2
    cRect2_ = OH_Drawing_RectCreate(x1, rectY1, distance + width_ / 3, rectY2); // divide w by 3 to get rect x2
    cImage_ = OH_Drawing_ImageCreate();
    cBitmap2_ = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    int imageY = 200; // image Y point 200
    OH_Drawing_BitmapBuild(cBitmap2_, width_ / 3, imageY, &cFormat); // divide w by 3 to get image x
    OH_Drawing_ImageBuildFromBitmap(cImage_, cBitmap2_);
    cOptions_ = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_NEAREST,
                                                 OH_Drawing_MipmapMode::MIPMAP_MODE_NEAREST);
    OH_Drawing_CanvasDrawImageRect(cCanvas_, cImage_, cRect2_, cOptions_);
}

void SampleBitMap::DrawPath()
{
    constexpr float penWidth = 10.0f; // pen width 10
    // 创建一个画笔Pen对象，Pen对象用于形状的边框线绘制
    cPen_ = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(cPen_, true);
    OH_Drawing_PenSetColor(cPen_, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xA5, 0x00));
    OH_Drawing_PenSetWidth(cPen_, penWidth);
    OH_Drawing_PenSetJoin(cPen_, LINE_ROUND_JOIN);
    // 将Pen画笔设置到canvas中
    OH_Drawing_CanvasAttachPen(cCanvas_, cPen_);
    // 绘制直线
    int x1 = 50; // x-axis point 1 50
    int lineLen = 50; // line length 50
    float y1 = height_ / 10; // divide h by 10 to get y-axis point 1
    float x2 = width_ / 4; // divide w by 4 to get x-axis point 2
    OH_Drawing_CanvasDrawLine(cCanvas_, x1, y1, x2 + lineLen, y1);
    OH_Drawing_CanvasDrawLine(cCanvas_, x2 + lineLen, y1, x1, y1 + lineLen);
    // 绘制圆形
    float centerX = width_ / 2; // divide w by 2 to get x-axis of center of the circle
    float centerY = height_ / 8; // divide h by 8 to get y-axis of center of the circle
    cCenter_ = OH_Drawing_PointCreate(centerX, centerY);
    float radius = width_ / 14; // divide w by 14 to get radius of the circle
    OH_Drawing_CanvasDrawCircle(cCanvas_, cCenter_, radius);
    // 绘制三阶贝塞尔圆滑曲线
    cPath_ = OH_Drawing_PathCreate();
    float startX = width_ / 4 * 3; // get 3 out of 4 width to get start x1
    float startY = height_ / 15; // divide h by 15 to get start y1
    OH_Drawing_PathMoveTo(cPath_, startX, startY);
    int distance = 50; // distance between earch point
    float ctrlX1 = startX + distance;
    float ctrlY1 = startY - distance; 
    float ctrlX2 = startX + 2 * distance; // 2 means twice the distance
    float ctrlY2 = startY + distance;
    float endX = startX + 3 * distance; // 3 means three times the distance
    float endY = startY;
    OH_Drawing_PathCubicTo(cPath_, ctrlX1, ctrlY1, ctrlX2, ctrlY2, endX, endY);
    OH_Drawing_CanvasDrawPath(cCanvas_, cPath_);
    // 绘制二阶贝塞尔圆滑曲线
    OH_Drawing_PathReset(cPath_);
    startY = height_ / 10; // divide h by 10 to get start y1
    OH_Drawing_PathMoveTo(cPath_, startX, startY);
    ctrlX1 = startX + distance;
    ctrlY1 = startY * 2; // 2 means twice the start y1
    endX = startX + 2 * distance; // 2 means twice the distance
    endY = startY;
    OH_Drawing_PathQuadTo(cPath_, ctrlX1, ctrlY1, endX, endY);
    OH_Drawing_CanvasDrawPath(cCanvas_, cPath_);
    // 取消描边
    OH_Drawing_CanvasDetachPen(cCanvas_);
    
    this->DrawPathHelper();
}

void SampleBitMap::DrawTextInPath()
{
    // 取消描边和填充
    OH_Drawing_CanvasDetachPen(cCanvas_);
    OH_Drawing_CanvasDetachBrush(cCanvas_);

    // 选择从左到右/左对齐等排版属性
    OH_Drawing_TypographyStyle *typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextDirection(typoStyle, TEXT_DIRECTION_LTR);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_JUSTIFY);

    // TEXT_ALIGN_JUSTIFY
    // 设置文字颜色，例如黑色
    OH_Drawing_TextStyle *txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    // 设置文字大小、字重等属性
    double fontSize = width_ / 25;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    OH_Drawing_SetTextStyleFontHeight(txtStyle, 1);
    // 设置字体类型等
    const char *fontFamilies[] = {"Roboto"};
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_NORMAL);
    OH_Drawing_SetTextStyleLocale(txtStyle, "zh");
    OH_Drawing_TypographyCreate *handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    // 设置文字内容
    const char *text = "路径描边\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography *typography = OH_Drawing_CreateTypography(handler);
    // 设置页面最大宽度
    double maxWidth = width_;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    // 设置文本在画布上绘制的起始位置
    double position[2] = {10, 10};
    // 将文本绘制到画布上
    OH_Drawing_TypographyPaint(typography, cCanvas_, position[0], position[1]);

    OH_Drawing_TypographyCreate *handler1 =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler1, txtStyle);
    // 设置文字内容
    const char *text1 = "路径填充\n";
    OH_Drawing_TypographyHandlerAddText(handler1, text1);
    OH_Drawing_TypographyHandlerPopTextStyle(handler1);
    OH_Drawing_Typography *typography1 = OH_Drawing_CreateTypography(handler1);
    // 设置页面最大宽度
    OH_Drawing_TypographyLayout(typography1, width_);
    // 将文本绘制到画布上
    int x1 = 10; // x1 10
    int y1 = height_ / 4; // divide h by 4 to get y1
    OH_Drawing_TypographyPaint(typography1, cCanvas_, x1, y1);

    OH_Drawing_TypographyCreate *handler2 =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler2, txtStyle);
    // 设置文字内容
    const char *text2 = "绘制图片\n";
    OH_Drawing_TypographyHandlerAddText(handler2, text2);
    OH_Drawing_TypographyHandlerPopTextStyle(handler2);
    OH_Drawing_Typography *typography2 = OH_Drawing_CreateTypography(handler2);
    // 设置页面最大宽度
    OH_Drawing_TypographyLayout(typography2, width_);
    // 将文本绘制到画布上
    y1 = height_ / 2; // divide h by 2 to get y1
    OH_Drawing_TypographyPaint(typography2, cCanvas_, x1, y1);
}

void SampleBitMap::DrawText()
{
    constexpr float penWidth = 5.0f; // pen width 5
    // 创建一个画笔Pen对象，Pen对象用于形状的边框线绘制
    cPen_ = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(cPen_, true);
    OH_Drawing_PenSetColor(cPen_, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    OH_Drawing_PenSetWidth(cPen_, penWidth);
    OH_Drawing_PenSetJoin(cPen_, LINE_ROUND_JOIN);
    // 将Pen画笔设置到canvas中
    OH_Drawing_CanvasAttachPen(cCanvas_, cPen_);

    // 选择从左到右/左对齐等排版属性
    OH_Drawing_TypographyStyle *typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextDirection(typoStyle, TEXT_DIRECTION_LTR);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_JUSTIFY);

    // TEXT_ALIGN_JUSTIFY
    // 设置文字颜色，例如黑色
    OH_Drawing_TextStyle *txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x66, 0x00, 0x00));
    // 设置文字大小、字重等属性
    double fontSize = width_ / 5;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    OH_Drawing_SetTextStyleFontHeight(txtStyle, 1);
    // 设置字体类型等
    const char *fontFamilies[] = {"Roboto"};
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_NORMAL);
    OH_Drawing_SetTextStyleLocale(txtStyle, "en");
    OH_Drawing_TypographyCreate *handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    // 设置文字内容
    const char *text = "Hello World\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography *typography = OH_Drawing_CreateTypography(handler);
    // 设置页面最大宽度
    double maxWidth = width_;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    // 设置文本在画布上绘制的起始位置
    double position[2] = {width_ / 5.0, height_ / 2.0};
    // 将文本绘制到画布上
    OH_Drawing_TypographyPaint(typography, cCanvas_, position[0], position[1]);
}

napi_value SampleBitMap::NapiDrawPattern(napi_env env, napi_callback_info info)
{
    DRAWING_LOGI("NapiDrawPattern");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiDrawPattern: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    if (napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_cb_info fail");
        return nullptr;
    }

    napi_value exportInstance;
    if (napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("NapiDrawPattern: Unable to get XComponent id");
        return nullptr;
    }
    DRAWING_LOGI("ID = %{public}s", idStr);
    std::string id(idStr);
    SampleBitMap *render = SampleBitMap().GetInstance(id);
    if (render != nullptr) {
        render->Prepare();
        render->Create();
        render->DrawPath();
        render->DrawTextInPath();
        render->DisPlay();
        DRAWING_LOGI("DrawPath executed");
    } else {
        DRAWING_LOGE("render is nullptr");
    }
    return nullptr;
}

napi_value SampleBitMap::NapiDrawText(napi_env env, napi_callback_info info)
{
    DRAWING_LOGI("NapiDrawText");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiDrawPattern: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    if (napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_cb_info fail");
        return nullptr;
    }

    napi_value exportInstance;
    if (napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("NapiDrawPattern: Unable to get XComponent id");
        return nullptr;
    }
    DRAWING_LOGI("ID = %{public}s", idStr);
    std::string id(idStr);
    SampleBitMap *render = SampleBitMap().GetInstance(id);
    if (render != nullptr) {
        render->Prepare();
        render->Create();
        render->DrawText();
        render->DisPlay();
        DRAWING_LOGI("DrawText executed");
    } else {
        DRAWING_LOGE("render is nullptr");
    }
    return nullptr;
}

SampleBitMap::~SampleBitMap()
{
    // 销毁创建的对象
    OH_Drawing_BrushDestroy(cBrush_);
    cBrush_ = nullptr;
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
    OH_Drawing_BitmapDestroy(cBitmap2_);
    cBitmap2_ = nullptr;
    OH_Drawing_RectDestroy(cRect_);
    cRect_ = nullptr;
    OH_Drawing_RectDestroy(cRect2_);
    cRect2_ = nullptr;
    OH_Drawing_ShaderEffectDestroy(shaderEffect_);
    shaderEffect_ = nullptr;
    OH_Drawing_PointDestroy(startPt_);
    startPt_ = nullptr;
    OH_Drawing_PointDestroy(endPt_);
    endPt_ = nullptr;
    OH_Drawing_PointDestroy(cCenter_);
    cCenter_ = nullptr;
    OH_Drawing_ImageDestroy(cImage_);
    cImage_ = nullptr;
    OH_Drawing_SamplingOptionsDestroy(cOptions_);
    cOptions_ = nullptr;
    OH_Drawing_RoundRectDestroy(cRRect_);
    cRRect_ = nullptr;

    buffer_ = nullptr;
    bufferHandle_ = nullptr;
    nativeWindow_ = nullptr;
    mappedAddr_ = nullptr;
}

void SampleBitMap::Release(std::string &id)
{
    SampleBitMap *render = SampleBitMap::GetInstance(id);
    if (render != nullptr) {
        delete render;
        render = nullptr;
        g_instance.erase(g_instance.find(id));
    }
}

void SampleBitMap::Export(napi_env env, napi_value exports)
{
    if ((env == nullptr) || (exports == nullptr)) {
        DRAWING_LOGE("Export: env or exports is null");
        return;
    }
    napi_property_descriptor desc[] = {
        {"drawPattern", nullptr, SampleBitMap::NapiDrawPattern, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"drawText", nullptr, SampleBitMap::NapiDrawText, nullptr, nullptr, nullptr, napi_default, nullptr}};
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    if (napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc) != napi_ok) {
        DRAWING_LOGE("Export: napi_define_properties failed");
    }
}

void SampleBitMap::RegisterCallback(OH_NativeXComponent *nativeXComponent)
{
    DRAWING_LOGI("register callback");
    renderCallback_.OnSurfaceCreated = OnSurfaceCreatedCB;
    renderCallback_.OnSurfaceDestroyed = OnSurfaceDestroyedCB;
    // Callback must be initialized
    renderCallback_.DispatchTouchEvent = nullptr;
    renderCallback_.OnSurfaceChanged = nullptr;
    OH_NativeXComponent_RegisterCallback(nativeXComponent, &renderCallback_);
}

SampleBitMap *SampleBitMap::GetInstance(std::string &id)
{
    if (g_instance.find(id) == g_instance.end()) {
        SampleBitMap *render = new SampleBitMap(id);
        g_instance[id] = render;
        return render;
    } else {
        return g_instance[id];
    }
}
