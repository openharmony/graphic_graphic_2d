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

#ifndef TEST_BASE_H
#define TEST_BASE_H

#include <bits/alltypes.h>
#include <chrono>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_gpu_context.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_surface.h>
#include <string>
#include "napi/native_api.h"

#define DEFAULT_WIDTH 720
#define DEFAULT_HEIGHT 720
#define DEFAULT_TESTCOUNT 1
#define DEFAULT_BACKGROUND_COLOR 0xFFFFFFFF

class TestBase {
public:
    TestBase(){};
    explicit TestBase(int styleType):styleType_(styleType){};
    virtual ~TestBase()
    {
        if (dstPixels_) {
            free(dstPixels_);
            dstPixels_ = nullptr;
        }
        StyleSettingsDestroy(nullptr);
    };

    void SetFileName(std::string fileName);
    void SetTestCount(uint32_t testCount);
    OH_Drawing_Bitmap* GetBitmap();
    uint32_t GetTime();
    void Destroy();

    void ClipCanvasToDrawSize(OH_Drawing_Canvas *canvas);
    void TestFunctionCpu(napi_env env);
    void TestFunctionGpu(OH_Drawing_Canvas* canvas);
    void TestFunctionGpu(napi_env env);
    void TestPerformanceCpu(napi_env env);
    void TestPerformanceGpu(OH_Drawing_Canvas* canvas);
    void TestPerformanceGpu(napi_env env);
    // CPU drawing function test, save drawing results to image (filename_)
    virtual void OnTestFunction(OH_Drawing_Canvas* canvas) {};
    // CPU drawing performance test, execute critical interface testCount_ times repeatedly
    virtual void OnTestPerformance(OH_Drawing_Canvas* canvas) {};

    enum {                      // 公共的pen，brush，filter等配置,在执行性能用例前设置
        DRAW_STYLE_NONE = 0, // 无配置
        DRAW_STYLE_COMPLEX,  // 最复杂的配置，会将所有配置加上，得出近似最恶劣的性能数据
    };
    int styleType_ = DRAW_STYLE_NONE;
    void StyleSettings(OH_Drawing_Canvas* canvas, int32_t type);
    void StyleSettingsDestroy(OH_Drawing_Canvas *canvas);
    
protected:
    // cpu bitmap canvas
    void CreateBitmapCanvas();
    void CreateGpuCanvas();
    void BitmapCanvasToFile(napi_env env);
    void GpuCanvasToFile(napi_env env);
    void Pixmap2File(napi_env env, napi_value pixelMap);

    std::chrono::high_resolution_clock::time_point LogStart();
    void LogEnd(std::chrono::high_resolution_clock::time_point start);

    void* dstPixels_ = nullptr;

    //pixmap to file
    OH_Drawing_Bitmap* bitmap_ = nullptr;
    OH_Drawing_Canvas* bitmapCanvas_ = nullptr;
    OH_Drawing_Canvas *gpuCanvas_ = nullptr;
    OH_Drawing_GpuContext *gpuContext_ = nullptr;
    OH_Drawing_Surface *surface_ = nullptr;
    OH_Drawing_Image_Info imageInfo_;
    
    // bitmapcanvas cfg
    uint32_t bitmapWidth_ = DEFAULT_WIDTH;
    uint32_t bitmapHeight_ = DEFAULT_HEIGHT;
    uint32_t background_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundA_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundR_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundG_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundB_ = DEFAULT_BACKGROUND_COLOR;
    std::string fileName_ = "default";
    uint32_t testCount_ = DEFAULT_TESTCOUNT;
    uint32_t usedTime_ = 0;

    OH_Drawing_Brush* styleBrush_ = nullptr;
    OH_Drawing_Pen* stylePen_ = nullptr;
    OH_Drawing_MaskFilter* styleMask_ = nullptr;
    OH_Drawing_Filter* styleFilter_ = nullptr;
    OH_Drawing_Point* styleCenter_ = nullptr;
    OH_Drawing_ShaderEffect* styleEffect_ = nullptr;
    OH_Drawing_PathEffect* stylePathEffect_ = nullptr;
};

#endif // TEST_BASE_H