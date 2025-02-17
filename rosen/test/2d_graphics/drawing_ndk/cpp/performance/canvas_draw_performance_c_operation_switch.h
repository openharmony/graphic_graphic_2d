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

#ifndef PERFORMANCE_CANVAS_DRAW_PERFORMANCE_C_OPERATION_S_WITCH_H
#define PERFORMANCE_CANVAS_DRAW_PERFORMANCE_C_OPERATION_S_WITCH_H

#include <map>
#include <multimedia/image_framework/image/pixelmap_native.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include <sys/types.h>

#include "performance.h"
#include "test_base.h"
#include "test_common.h"


typedef enum {
    CANVAS_DRAW_FUNCTION_DRAW_LINE = 0,
    CANVAS_DRAW_FUNCTION_DRAW_PATH,
    CANVAS_DRAW_FUNCTION_DRAW_PIXEL_MAP_RECT,
    CANVAS_DRAW_FUNCTION_DRAW_REGION,
    CANVAS_DRAW_FUNCTION_DRAW_POINTS,
    CANVAS_DRAW_FUNCTION_DRAW_BITMAP,
    CANVAS_DRAW_FUNCTION_DRAW_BITMAP_RECT,
    CANVAS_DRAW_FUNCTION_DRAW_RECT,
    CANVAS_DRAW_FUNCTION_DRAW_CIRCLE,
    CANVAS_DRAW_FUNCTION_DRAW_OVAL,
    CANVAS_DRAW_FUNCTION_DRAW_ARC,
    CANVAS_DRAW_FUNCTION_DRAW_ROUND_RECT,
    CANVAS_DRAW_FUNCTION_DRAW_TEXT_BLOB,
    CANVAS_DRAW_FUNCTION_DRAW_SHADOW,
    CANVAS_DRAW_FUNCTION_DRAW_IMAGE_RECT_WITH_SRC,
    CANVAS_DRAW_FUNCTION_DRAW_IMAGE_RECT,
    CANVAS_DRAW_FUNCTION_MAX
} CanvasDrawFunction;

class PerformanceDrawLineAndPath : public TestBase {
public:
    explicit PerformanceDrawLineAndPath(int attachType)
    {
        fileName_ =
            "PerformanceDrawLineAndPath" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
    }
    ~PerformanceDrawLineAndPath() override
    {
        delete performance_;
    };

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;

private:
    int attachType_ = DRAWING_TYPE_ATTACH_BOTH;
    SpecialPerformance* performance_;
};

class PerformanceDrawOpsWitch : public TestBase {
public:
    explicit PerformanceDrawOpsWitch(int attachType)
    {
        fileName_ = "PerformanceDrawOpsWitch";
        fileName_ = "PerformanceDrawOpsWitch" + SpecialPerformance::GetAttachTypeName(attachType);
        attachType_ = attachType;
        performance_ = new SpecialPerformance();
        path_ = OH_Drawing_PathCreate();
        region_ = OH_Drawing_RegionCreate();
        point_ = OH_Drawing_PointCreate(itemWidth * 3 + 10, itemHeight * 2 + 30); // for test
                                                                                  //  draw arc
        arcRect_ = OH_Drawing_RectCreate(
            itemWidth * 5, itemHeight * 2 + 10, itemWidth * 5 + 50, itemHeight * 2 + 100); // for test
        roundRectRect_ = OH_Drawing_RectCreate(
            itemWidth * 6, itemHeight * 2 + 10, itemWidth * 6 + 50, itemHeight * 2 + 60); // for test
        // draw textblob
        font_ = OH_Drawing_FontCreate();
        // 18 for test
        OH_Drawing_FontSetTextSize(font_, 18);
        textBlob_ = OH_Drawing_TextBlobCreateFromString("font test你好 @", font_, TEXT_ENCODING_UTF8);
        shadowPath_ = OH_Drawing_PathCreate();
        // 10,1.8 for test
        OH_Drawing_PathArcTo(shadowPath_, 0, 0, 10, 10, 0, 1.8);

        CreatePixelMap();
    }
    ~PerformanceDrawOpsWitch() override
    {
        delete performance_;
        OH_Drawing_PathDestroy(path_);
        OH_Drawing_RegionDestroy(region_);
        OH_Drawing_RectDestroy(imageRect_);
        OH_Drawing_RectDestroy(imageRectSrc_);
        delete[] srcPixels_;
        OH_Drawing_BitmapDestroy(bitmap1_);
        OH_Drawing_ImageDestroy(image_);
        OH_Drawing_SamplingOptionsDestroy(samplingOptions_);
        OH_Drawing_PixelMapDissolve(pixelMap_);
        OH_PixelmapNative_Release(pixelMapNative_);
        OH_PixelmapInitializationOptions_Release(createOps_);
        OH_Drawing_PointDestroy(point_);
        OH_Drawing_RectDestroy(arcRect_);
        OH_Drawing_RectDestroy(roundRectRect_);
        OH_Drawing_FontDestroy(font_);
        OH_Drawing_TextBlobDestroy(textBlob_);
        OH_Drawing_PathDestroy(shadowPath_);
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
    int32_t xOffset_ = 0;
    u_int32_t* srcPixels_ = nullptr;
    OH_Drawing_Bitmap* bitmap1_ = nullptr;
    OH_Drawing_Rect* imageRectSrc_ = nullptr;
    OH_Drawing_Rect* imageRect_ = nullptr;
    OH_PixelmapNative* pixelMapNative_ = nullptr;
    OH_Pixelmap_InitializationOptions* createOps_ = nullptr;
    OH_Drawing_PixelMap* pixelMap_ = nullptr;
    OH_Drawing_Path* path_ = nullptr;
    OH_Drawing_Region* region_ = nullptr;
    OH_Drawing_Point* point_ = nullptr;
    OH_Drawing_Rect* arcRect_ = nullptr;
    OH_Drawing_Rect* roundRectRect_ = nullptr;
    OH_Drawing_Font* font_ = nullptr;
    OH_Drawing_TextBlob* textBlob_ = nullptr;
    OH_Drawing_Path* shadowPath_ = nullptr;
    void CreatePixelMap();
    void DrawLineTest(OH_Drawing_Canvas* canvas);
    void DrawPathTest(OH_Drawing_Canvas* canvas);
    void DrawPixelMapRect(OH_Drawing_Canvas* canvas);
    void DrawRegion(OH_Drawing_Canvas* canvas);
    void DrawPoints(OH_Drawing_Canvas* canvas);
    void DrawBitmap(OH_Drawing_Canvas* canvas);
    void DrawBitmapRect(OH_Drawing_Canvas* canvas);
    void DrawRect(OH_Drawing_Canvas* canvas);
    void DrawCircle(OH_Drawing_Canvas* canvas);
    void DrawOval(OH_Drawing_Canvas* canvas);
    void DrawArc(OH_Drawing_Canvas* canvas);
    void DrawRoundRect(OH_Drawing_Canvas* canvas);
    void DrawTextBlob(OH_Drawing_Canvas* canvas);
    void DrawShadow(OH_Drawing_Canvas* canvas);
    void DrawImageRectWithSrc(OH_Drawing_Canvas* canvas);
    void DrawImageRect(OH_Drawing_Canvas* canvas);
    using HandlerFunc = std::function<void(OH_Drawing_Canvas*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { CANVAS_DRAW_FUNCTION_DRAW_LINE, [this](OH_Drawing_Canvas* canvas) { return DrawLineTest(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_PATH, [this](OH_Drawing_Canvas* canvas) { return DrawPathTest(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_PIXEL_MAP_RECT,
            [this](OH_Drawing_Canvas* canvas) { return DrawPixelMapRect(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_REGION, [this](OH_Drawing_Canvas* canvas) { return DrawRegion(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_POINTS, [this](OH_Drawing_Canvas* canvas) { return DrawPoints(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_BITMAP, [this](OH_Drawing_Canvas* canvas) { return DrawBitmap(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_BITMAP_RECT, [this](OH_Drawing_Canvas* canvas) { return DrawBitmapRect(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_RECT, [this](OH_Drawing_Canvas* canvas) { return DrawRect(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_CIRCLE, [this](OH_Drawing_Canvas* canvas) { return DrawCircle(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_OVAL, [this](OH_Drawing_Canvas* canvas) { return DrawOval(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_ARC, [this](OH_Drawing_Canvas* canvas) { return DrawArc(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_ROUND_RECT, [this](OH_Drawing_Canvas* canvas) { return DrawRoundRect(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_TEXT_BLOB, [this](OH_Drawing_Canvas* canvas) { return DrawTextBlob(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_SHADOW, [this](OH_Drawing_Canvas* canvas) { return DrawShadow(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_IMAGE_RECT_WITH_SRC,
            [this](OH_Drawing_Canvas* canvas) { return DrawImageRectWithSrc(canvas); } },
        { CANVAS_DRAW_FUNCTION_DRAW_IMAGE_RECT, [this](OH_Drawing_Canvas* canvas) { return DrawImageRect(canvas); } },
    };
};

#endif // PERFORMANCE_CANVAS_DRAW_PERFORMANCE_C_OPERATION_S_WITCH_H