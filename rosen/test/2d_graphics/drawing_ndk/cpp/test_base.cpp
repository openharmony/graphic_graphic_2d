/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "test_base.h"

#include <fcntl.h>
#include <memory>
#include <multimedia/image_framework/image/pixelmap_native.h>
#include <multimedia/image_framework/image/image_packer_native.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <unordered_map>
#include "common/log_common.h"

const int SECOND_TO_NANO = 1000000000;

void TestBase::SetFileName(std::string fileName)
{
    fileName_ = fileName;
}

void TestBase::SetTestCount(uint32_t testCount)
{
    testCount_ = testCount;
}

OH_Drawing_Bitmap* TestBase::GetBitmap()
{
    return bitmap_;
}

uint32_t TestBase::GetTime()
{
    return usedTime_;
}

void TestBase::ClipCanvasToDrawSize(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 0, bitmapWidth_, bitmapHeight_);
    OH_Drawing_CanvasClipRect(canvas, rect, OH_Drawing_CanvasClipOp::INTERSECT, false);
    OH_Drawing_RectDestroy(rect);
}

void TestBase::TestFunctionCpu(napi_env env)
{
    CreateBitmapCanvas();
    OnTestFunction(bitmapCanvas_);
    BitmapCanvasToFile();
}

void TestBase::TestFunctionGpu(napi_env env)
{
    CreateGpuCanvas();
    OnTestFunction(gpuCanvas_);
    GpuCanvasToFile(env, true);
    Destroy();
}

void TestBase::TestStabilityCpu()
{
    CreateBitmapCanvas();
    OnTestStability(bitmapCanvas_);
    BitmapCanvasToFile();
}

void TestBase::TestFunctionGpu(OH_Drawing_Canvas *canvas) { OnTestFunction(canvas); }

unsigned long long TestBase::LogStart()
{
    unsigned long long start = 0;
    struct timespec tv {};
    if (clock_gettime(CLOCK_BOOTTIME, &tv) >= 0) {
        start = tv.tv_sec * SECOND_TO_NANO + tv.tv_nsec;
    }
    return start;
}

void TestBase::LogEnd(unsigned long long start)
{
    unsigned long long end = 0;
    struct timespec tv {};
    if (clock_gettime(CLOCK_BOOTTIME, &tv) >= 0) {
        end = tv.tv_sec * SECOND_TO_NANO + tv.tv_nsec;
    }
    // LOGE is to avoid log loss
    DRAWING_LOGE("DrawingApiTest TotalApiCallCount: [%{public}u]", testCount_);
    usedTime_ = end - start;
    DRAWING_LOGE("DrawingApiTest TotalApiCallTime: [%{public}u]", usedTime_);
}

void TestBase::TestPerformanceGpu(OH_Drawing_Canvas *canvas)
{
    StyleSettings(canvas, styleType_);
    auto start = LogStart();
    OnTestPerformance(canvas);
    LogEnd(start);
    StyleSettingsDestroy(canvas);
}

void TestBase::TestPerformanceCpu(napi_env env)
{
    CreateBitmapCanvas();
    StyleSettings(bitmapCanvas_, styleType_);
    auto start = LogStart();

    OnTestPerformance(bitmapCanvas_);

    LogEnd(start);
    StyleSettingsDestroy(bitmapCanvas_);
}

void TestBase::TestPerformanceGpu(napi_env env)
{
    CreateGpuCanvas();
    StyleSettings(gpuCanvas_, styleType_);
    auto start = LogStart();

    OnTestPerformance(gpuCanvas_);

    LogEnd(start);
    GpuCanvasToFile(env, false);
    StyleSettingsDestroy(gpuCanvas_);
    Destroy();
}

void TestBase::CreateBitmapCanvas()
{
    // 创建一个bitmap对象
    if (bitmap_) {
        OH_Drawing_BitmapDestroy(bitmap_);
    }
    bitmap_ = OH_Drawing_BitmapCreate();
    // 定义bitmap的像素格式
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL};
    // 构造对应格式的bitmap
    OH_Drawing_BitmapBuild(bitmap_, bitmapWidth_, bitmapHeight_, &cFormat);

    // 创建一个canvas对象
    if (bitmapCanvas_) {
        OH_Drawing_CanvasDestroy(bitmapCanvas_);
    }
    bitmapCanvas_ = OH_Drawing_CanvasCreate();
    // 将画布与bitmap绑定，画布画的内容会输出到绑定的bitmap内存中
    OH_Drawing_CanvasBind(bitmapCanvas_, bitmap_);
    // 使用白色清除画布内容
    OH_Drawing_CanvasClear(bitmapCanvas_, OH_Drawing_ColorSetArgb(backgroundA_,
        backgroundR_, backgroundG_, backgroundB_));
}

void TestBase::CreateGpuCanvas()
{
    OH_Drawing_GpuContextOptions options{false};
    imageInfo_ = {static_cast<int32_t>(bitmapWidth_), static_cast<int32_t>(bitmapHeight_),
        COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL};
    gpuContext_ = OH_Drawing_GpuContextCreateFromGL(options);
    surface_ = OH_Drawing_SurfaceCreateFromGpuContext(gpuContext_, true, imageInfo_);
    gpuCanvas_ = OH_Drawing_SurfaceGetCanvas(surface_);
    OH_Drawing_CanvasClear(gpuCanvas_, OH_Drawing_ColorSetArgb(backgroundA_, backgroundR_, backgroundG_, backgroundB_));
}

void TestBase::BitmapCanvasToFile()
{
    DRAWING_LOGE("BitmapCanvasToFile");
    //创建pixmap
    OH_Pixelmap_InitializationOptions *createOps = nullptr;
    auto ret = OH_PixelmapInitializationOptions_Create(&createOps);
    if (ret != IMAGE_SUCCESS || !createOps) {
        DRAWING_LOGE("BitmapCanvasToFile OH_PixelmapInitializationOptions_Create failed %{public}d", ret);
        return;
    }

    OH_PixelmapInitializationOptions_SetWidth(createOps, bitmapWidth_);
    OH_PixelmapInitializationOptions_SetHeight(createOps, bitmapHeight_);
    OH_PixelmapInitializationOptions_SetPixelFormat(createOps, 3); // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetSrcPixelFormat(createOps, 3); // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetAlphaType(createOps, 2); // 2 is ALPHA_FORMAT_PREMUL

    size_t bufferSize = bitmapWidth_ * bitmapHeight_ * 4;
    void *bitmapAddr = OH_Drawing_BitmapGetPixels(bitmap_);
    OH_PixelmapNative *pixelMap = nullptr;
    ret = OH_PixelmapNative_CreatePixelmap((uint8_t *)bitmapAddr, bufferSize, createOps, &pixelMap);
    if (ret != IMAGE_SUCCESS || pixelMap == nullptr) {
        DRAWING_LOGE(" failed to OH_PixelMap_CreatePixelMap width = %{public}u, height = %{public}u",
            bitmapWidth_, bitmapHeight_);
        OH_PixelmapInitializationOptions_Release(createOps);
        return;
    }

    Pixmap2RawFile(bitmapAddr, bufferSize);
    Pixmap2ImageFile(pixelMap);

    OH_PixelmapNative_Release(pixelMap);
    OH_PixelmapInitializationOptions_Release(createOps);
}

void TestBase::GpuCanvasToFile(napi_env env, bool saveFile)
{
    DRAWING_LOGI("GpuCanvasToFile");
    //创建pixmap
    OH_Pixelmap_InitializationOptions *createOps = nullptr;
    auto ret = OH_PixelmapInitializationOptions_Create(&createOps);
    if (ret != IMAGE_SUCCESS || !createOps) {
        DRAWING_LOGE("GpuCanvasToFile OH_PixelmapInitializationOptions_Create failed %{public}d", ret);
        return;
    }

    OH_PixelmapInitializationOptions_SetWidth(createOps, bitmapWidth_);
    OH_PixelmapInitializationOptions_SetHeight(createOps, bitmapHeight_);
    OH_PixelmapInitializationOptions_SetPixelFormat(createOps, 3);  // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetSrcPixelFormat(createOps, 3); // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetAlphaType(createOps, 2); // 2 is ALPHA_FORMAT_PREMUL

    size_t bufferSize = bitmapWidth_ * bitmapHeight_ * 4;
    if (dstPixels_) {
        free(dstPixels_);
        dstPixels_ = nullptr;
    }
    dstPixels_ = malloc(bitmapWidth_ * bitmapHeight_ * 4); // 4 for rgba
    if (dstPixels_ == nullptr) {
        DRAWING_LOGE("dstPixels_ malloc failed");
        OH_PixelmapInitializationOptions_Release(createOps);
        return;
    }
    bool output = OH_Drawing_CanvasReadPixels(gpuCanvas_, &imageInfo_, dstPixels_, 4 * bitmapWidth_, 0, 0);
    if (!output) {
        DRAWING_LOGE("read pixels failed");
        OH_PixelmapInitializationOptions_Release(createOps);
        return;
    }
    if (bitmap_) {
        OH_Drawing_BitmapDestroy(bitmap_);
    }
    bitmap_ = OH_Drawing_BitmapCreateFromPixels(&imageInfo_, dstPixels_, 4 * bitmapWidth_); // 4 for rgba

    OH_PixelmapNative *pixelMap = nullptr;
    ret = OH_PixelmapNative_CreatePixelmap((uint8_t *)dstPixels_, bufferSize, createOps, &pixelMap);
    if (ret != IMAGE_SUCCESS || pixelMap == nullptr) {
        DRAWING_LOGE("failed to CreatePixelMap width = %{public}u, height = %{public}u", bitmapWidth_, bitmapHeight_);
        return;
    }

    if (saveFile) {
        Pixmap2RawFile(dstPixels_, bufferSize);
        Pixmap2ImageFile(pixelMap);
    }
}

void TestBase::Pixmap2RawFile(void *pixelmap, uint32_t pixelMapSize)
{
    // 打开需要输出的文件（请确保应用有权限访问这个路径）
    std::string path = "/data/storage/el2/base/files/" + fileName_ + ".dat";
    auto err = remove(path.c_str());
    if (err) {
        DRAWING_LOGE("failed to remove path: %{public}s", path.c_str());
    }
    int fd = open(path.c_str(), O_RDWR | O_CREAT);
    if (fd <= 0) {
        DRAWING_LOGE("failed to open fd = %{public}d path = %{public}s", fd, path.c_str());
        return;
    }

    auto result = write(fd, pixelmap, pixelMapSize);
    if (result != pixelMapSize) {
        DRAWING_LOGE("failed to Pixmap2RawFile");
    }
    // 关闭输出文件
    close(fd);
}

void TestBase::Pixmap2ImageFile(OH_PixelmapNative* pixelMap)
{
    OH_ImagePackerNative *packer = nullptr;
    auto result = OH_ImagePackerNative_Create(&packer);
    if (result != IMAGE_SUCCESS || !packer) {
        DRAWING_LOGE("failed to OH_ImagePackerNative_Create");
        return;
    }

    OH_PackingOptions *opts = nullptr;
    result = OH_PackingOptions_Create(&opts);
    if (result != IMAGE_SUCCESS || !opts) {
        DRAWING_LOGE("failed to OH_PackingOptions_Create");
        return;
    }

    char format[] = "image/png";
    Image_MimeType imageMimeType { format, strlen(format) };
    OH_PackingOptions_SetMimeType(opts, &imageMimeType);
    uint32_t quality = 100;
    OH_PackingOptions_SetQuality(opts, quality);

    // 打开需要输出的文件（请确保应用有权限访问这个路径）
    std::string path = "/data/storage/el2/base/files/" + fileName_ + ".png";
    auto err = remove(path.c_str());
    if (err) {
        DRAWING_LOGE("failed to remove path: %{public}s", path.c_str());
    }
    int fd = open(path.c_str(), O_RDWR | O_CREAT);
    if (fd <= 0) {
        DRAWING_LOGE("failed to open fd = %{public}d path = %{public}s", fd, path.c_str());
        OH_PackingOptions_Release(opts);
        OH_ImagePackerNative_Release(packer);
        return;
    }

    // 开始对输入source进行编码过程，返回result为 IMAGE_RESULT_SUCCESS则编码成功
    result = OH_ImagePackerNative_PackToFileFromPixelmap(packer, opts, pixelMap, fd);
    if (result != IMAGE_SUCCESS) {
        DRAWING_LOGE("failed to OH_ImagePackerNative_PackToFileFromImageSource");
        close(fd);
        OH_PackingOptions_Release(opts);
        OH_ImagePackerNative_Release(packer);
        return;
    }

    // 关闭输出文件
    close(fd);
    OH_PackingOptions_Release(opts);
    OH_ImagePackerNative_Release(packer);
}

void TestBase::Destroy()
{
    if (surface_) {
        OH_Drawing_SurfaceDestroy(surface_);
        surface_ = nullptr;
    }
    if (gpuContext_) {
        OH_Drawing_GpuContextDestroy(gpuContext_);
        gpuContext_ = nullptr;
    }
}

void TestBase::StyleSettings(OH_Drawing_Canvas* canvas, int32_t type)
{
    if (canvas == nullptr) {
        return;
    }
    StyleSettingsDestroy(canvas);
    if (type == DRAW_STYLE_COMPLEX) {
        styleBrush_ = OH_Drawing_BrushCreate();
        OH_Drawing_BrushSetColor(styleBrush_, 0xFFFF0000);
        OH_Drawing_CanvasAttachBrush(canvas, styleBrush_);
    }
}

void TestBase::StyleSettingsDestroy(OH_Drawing_Canvas *canvas)
{
    if (canvas != nullptr) {
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasDetachBrush(canvas);
    }
    if (stylePen_ != nullptr) {
        OH_Drawing_PenDestroy(stylePen_);
        stylePen_ = nullptr;
    }
    if (styleBrush_ != nullptr) {
        OH_Drawing_BrushDestroy(styleBrush_);
        styleBrush_ = nullptr;
    }
    if (styleMask_ != nullptr) {
        OH_Drawing_MaskFilterDestroy(styleMask_);
        styleMask_ = nullptr;
    }
    if (styleFilter_ != nullptr) {
        OH_Drawing_FilterDestroy(styleFilter_);
        styleFilter_ = nullptr;
    }
    if (styleCenter_ != nullptr) {
        OH_Drawing_PointDestroy(styleCenter_);
        styleCenter_ = nullptr;
    }
    if (styleEffect_ != nullptr) {
        OH_Drawing_ShaderEffectDestroy(styleEffect_);
        styleEffect_ = nullptr;
    }
    if (stylePathEffect_ != nullptr) {
        OH_Drawing_PathEffectDestroy(stylePathEffect_);
        stylePathEffect_ = nullptr;
    }
}

uint32_t TestBase::GetBitmapWidth()
{
    return bitmapWidth_;
}

uint32_t TestBase::GetBitmapHeight()
{
    return bitmapHeight_;
}