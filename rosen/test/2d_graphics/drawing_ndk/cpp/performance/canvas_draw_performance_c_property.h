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

#ifndef PERFORMANCE_CANVAS_DRAW_PERFORMANCE_C_PROPERTY_H
#define PERFORMANCE_CANVAS_DRAW_PERFORMANCE_C_PROPERTY_H

#include <cstdint>
#include <map>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "performance/performance.h"
#include "test_base.h"
#include "test_common.h"

#include "common/drawing_type.h"

typedef enum {
    LARGE_IMAGE_LOAD_FUNCTION_CANVAS_DRAW_IMAGE_RECT_WITH_SRC = 0,
    LARGE_IMAGE_LOAD_FUNCTION_CANVAS_DRAW_IMAGE_RECT,
    LARGE_IMAGE_LOAD_FUNCTION_CANVAS_READ_PIXELS,
    LARGE_IMAGE_LOAD_FUNCTION_MAX
} LargeImageLoadFunction;

// 超多顶点数组绘制-OH_Drawing_CanvasDrawVertices attach brush
class PerformanceCCanvasDrawVerticesBrush : public TestBase {
public:
    explicit PerformanceCCanvasDrawVerticesBrush(int attachType)
    {
        fileName_ = "PerformanceCCanvasDrawVerticesBrush";
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    }
    ~PerformanceCCanvasDrawVerticesBrush() override
    {
        delete performance_;
    };

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;

private:
    int attachType_ = DRAWING_TYPE_ATTACH_BRUSH;
    SpecialPerformance* performance_;
    // 100 width for test
    const int32_t itemWidth = 100;
    // 100 height for test
    const int32_t itemHeight = 100;
};

// 超大图片绘制性能-OH_Drawing_CanvasReadPixels attach pen and brush
// 超大图片绘制性能-OH_Drawing_CanvasReadPixels attach pen
// 超大图片绘制性能-OH_Drawing_CanvasReadPixels attach brush
class PerformanceCCanvasReadPixels : public TestBase {
public:
    explicit PerformanceCCanvasReadPixels(int attachType)
    {
        fileName_ = "PerformanceCCanvasReadPixels" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
        CreatePixelMap();
    }
    ~PerformanceCCanvasReadPixels() override
    {
        delete performance_;
        free(dstPixels_);
        OH_Drawing_RectDestroy(imageRect_);
        OH_Drawing_RectDestroy(imageRectSrc_);
        delete[] srcPixels_;
        OH_Drawing_BitmapDestroy(bitmap1_);
        OH_Drawing_ImageDestroy(image_);
        OH_Drawing_SamplingOptionsDestroy(samplingOptions_);
    };

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;

private:
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_;

    OH_Drawing_SamplingOptions* samplingOptions_ = nullptr;
    OH_Drawing_Image* image_ = nullptr;
    int32_t imageWidth_ = 1024 * 4;  // 1024, 4 for test
    int32_t imageHeight_ = 1080 * 2; // 1080, 2 for test
    // 100 width for test
    const int32_t itemWidth = 100;
    // 100 height for test
    const int32_t itemHeight = 100;
    uint32_t* srcPixels_ = nullptr;
    OH_Drawing_Bitmap* bitmap1_ = nullptr;
    OH_Drawing_Rect* imageRectSrc_ = nullptr;
    OH_Drawing_Rect* imageRect_ = nullptr;
    char* dstPixels_ = nullptr;
    void CreatePixelMap();
};

// 超大图片绘制性能-OH_Drawing_CanvasReadPixelsToBitmap attach pen and brush,attach pen,attach brush
class PerformanceCCanvasReadPixelsToBitmap : public TestBase {
public:
    explicit PerformanceCCanvasReadPixelsToBitmap(int attachType)
    {
        fileName_ = "PerformanceCCanvasReadPixelsToBitmap" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
        CreatePixelMap();
    }
    ~PerformanceCCanvasReadPixelsToBitmap() override
    {
        delete performance_;
        OH_Drawing_RectDestroy(imageRect_);
        delete[] srcPixels_;
        OH_Drawing_BitmapDestroy(bitmap1_);
        OH_Drawing_ImageDestroy(image_);
        OH_Drawing_SamplingOptionsDestroy(samplingOptions_);
    };

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;

private:
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_;

    OH_Drawing_SamplingOptions* samplingOptions_ = nullptr;
    OH_Drawing_Image* image_ = nullptr;
    int32_t imageWidth_ = 1024 * 4;  // 1024, 4 for test
    int32_t imageHeight_ = 1080 * 2; // 1080, 2 for test
    // 100 width for test
    const int32_t itemWidth = 100;
    // 100 height for test
    const int32_t itemHeight = 100;
    uint32_t* srcPixels_ = nullptr;
    OH_Drawing_Bitmap* bitmap1_ = nullptr;
    OH_Drawing_Rect* imageRect_ = nullptr;
    void CreatePixelMap();
};

// 超大字体包加载性能OH_Drawing_CanvasDrawImageRectWithSrc
// 超大字体包加载性能OH_Drawing_CanvasDrawImageRect
// 超大字体包加载性能OH_Drawing_CanvasReadPixels
class PerformanceCCanvasLargeImageLoad : public TestBase {
public:
    explicit PerformanceCCanvasLargeImageLoad(int attachType, LargeImageLoadFunction function)
    {
        function_ = function;
        attachType_ = attachType;
        fileName_ = "PerformanceCCanvasLargeImage" + functionNameMap_[function] +
                    SpecialPerformance::GetAttachTypeName(attachType);
        performance_ = new SpecialPerformance();
        CreatePixelMap();
    }
    ~PerformanceCCanvasLargeImageLoad() override
    {
        delete performance_;
        free(dstPixels_);
        OH_Drawing_RectDestroy(imageRectSrc_);
        delete[] srcPixels_;
        OH_Drawing_BitmapDestroy(bitmap1_);
        OH_Drawing_ImageDestroy(image_);
        OH_Drawing_SamplingOptionsDestroy(samplingOptions_);
    };

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;

private:
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    LargeImageLoadFunction function_;
    SpecialPerformance* performance_;

    OH_Drawing_SamplingOptions* samplingOptions_ = nullptr;
    OH_Drawing_Image* image_ = nullptr;
    int32_t imageWidth_ = 1024 * 4;  // 1024, 4 for test
    int32_t imageHeight_ = 1080 * 2; // 1080, 2 for test
    // 100 width for test
    const int32_t itemWidth = 100;
    // 100 height for test
    const int32_t itemHeight = 100;
    uint32_t* srcPixels_ = nullptr;
    OH_Drawing_Bitmap* bitmap1_ = nullptr;
    OH_Drawing_Rect* imageRectSrc_ = nullptr;
    char* dstPixels_ = nullptr;
    void CreatePixelMap();
    void DrawImageRectWithSrcTest(OH_Drawing_Canvas* canvas);
    void DrawImageRectTest(OH_Drawing_Canvas* canvas);
    void ReadPixelsTest(OH_Drawing_Canvas* canvas);
    static std::map<uint32_t, std::string> functionNameMap_;
    using HandlerFunc = std::function<void(OH_Drawing_Canvas*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { LARGE_IMAGE_LOAD_FUNCTION_CANVAS_DRAW_IMAGE_RECT_WITH_SRC,
            [this](OH_Drawing_Canvas* canvas) { return DrawImageRectWithSrcTest(canvas); } },
        { LARGE_IMAGE_LOAD_FUNCTION_CANVAS_DRAW_IMAGE_RECT,
            [this](OH_Drawing_Canvas* canvas) { return DrawImageRectTest(canvas); } },
        { LARGE_IMAGE_LOAD_FUNCTION_CANVAS_READ_PIXELS,
            [this](OH_Drawing_Canvas* canvas) { return ReadPixelsTest(canvas); } }
    };
};

#endif // PERFORMANCE_CANVAS_DRAW_PERFORMANCE_C_PROPERTY_H
