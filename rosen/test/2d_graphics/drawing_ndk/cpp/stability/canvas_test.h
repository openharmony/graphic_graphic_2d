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

#ifndef STABILITY_CANVAS_TEST_H
#define STABILITY_CANVAS_TEST_H

#include <map>
#include <multimedia/image_framework/image/pixelmap_native.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_text_blob.h>

#include "test_base.h"
#include "test_common.h"

typedef enum {
    // Operation function
    OPERATION_FUNCTION_CANVAS_DRAW_LINE = 0,
    OPERATION_FUNCTION_CANVAS_DRAW_PATH,
    OPERATION_FUNCTION_CANVAS_DRAW_PIXEL_MAP_RECT,
    OPERATION_FUNCTION_CANVAS_DRAW_BACKGROUND,
    OPERATION_FUNCTION_CANVAS_DRAW_REGION,
    OPERATION_FUNCTION_CANVAS_DRAW_POINT,
    OPERATION_FUNCTION_CANVAS_DRAW_POINTS,
    OPERATION_FUNCTION_CANVAS_DRAW_BITMAP,
    OPERATION_FUNCTION_CANVAS_DRAW_BITMAP_RECT,
    OPERATION_FUNCTION_CANVAS_DRAW_RECT,
    OPERATION_FUNCTION_CANVAS_DRAW_CIRCLE,
    OPERATION_FUNCTION_CANVAS_DRAW_COLOR,
    OPERATION_FUNCTION_CANVAS_DRAW_OVAL,
    OPERATION_FUNCTION_CANVAS_DRAW_ARC,
    OPERATION_FUNCTION_CANVAS_DRAW_ROUND_RECT,
    OPERATION_FUNCTION_CANVAS_DRAW_SINGLE_CHARACTER,
    OPERATION_FUNCTION_CANVAS_DRAW_TEXT_BLOB,
    OPERATION_FUNCTION_CANVAS_CLIP_RECT,
    OPERATION_FUNCTION_CANVAS_CLIP_ROUND_RECT,
    OPERATION_FUNCTION_CANVAS_CLIP_PATH,
    OPERATION_FUNCTION_CANVAS_CLIP_REGION,
    OPERATION_FUNCTION_CANVAS_ROTATE,
    OPERATION_FUNCTION_CANVAS_TRANSLATE,
    OPERATION_FUNCTION_CANVAS_SCALE,
    OPERATION_FUNCTION_CANVAS_SKEW,
    OPERATION_FUNCTION_CANVAS_GET_WIDTH,
    OPERATION_FUNCTION_CANVAS_GET_HEIGHT,
    OPERATION_FUNCTION_CANVAS_GET_LOCAL_CLIP_BOUNDS,
    OPERATION_FUNCTION_CANVAS_GET_TOTAL_MATRIX,
    OPERATION_FUNCTION_CANVAS_CONCAT_MATRIX,
    OPERATION_FUNCTION_CANVAS_DRAW_SHADOW,
    OPERATION_FUNCTION_CANVAS_CLEAR,
    OPERATION_FUNCTION_CANVAS_SET_MATRIX,
    OPERATION_FUNCTION_CANVAS_RESET_MATRIX,
    OPERATION_FUNCTION_CANVAS_DRAW_IMAGE_RECT_WITH_SRC,
    OPERATION_FUNCTION_CANVAS_DRAW_IMAGE_RECT,
    OPERATION_FUNCTION_CANVAS_DRAW_VERTICES,
    OPERATION_FUNCTION_CANVAS_READ_PIXELS,
    OPERATION_FUNCTION_CANVAS_READ_PIXELS_TO_BITMAP,
    OPERATION_FUNCTION_CANVAS_IS_CLIP_EMPTY,
    OPERATION_FUNCTION_CANVAS_GET_IMAGE_INFO,
    OPERATION_FUNCTION_CANVAS_MAX,
} OperationFunctionCanvas;

typedef enum {
    // setting function
    SETTING_FUNCTION_CANVAS_BIND = 0,
    SETTING_FUNCTION_CANVAS_MAX
} SettingFunctionCanvas;

// CanvasCreate-CanvasDestroy循环
class StabilityCanvasCreate : public TestBase {
public:
    StabilityCanvasCreate()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityCanvasCreate";
    }
    ~StabilityCanvasCreate() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// CanvasCreate-相关配置接口全调用-CanvasDestroy循环
class StabilityCanvasInvoke : public TestBase {
public:
    StabilityCanvasInvoke()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityCanvasInvoke";
        bitmap1_ = OH_Drawing_BitmapCreate();
        OH_Drawing_BitmapFormat format = { COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_UNPREMUL };
        OH_Drawing_BitmapBuild(bitmap1_, bitmapWidth_, bitmapHeight_, &format);
    }
    ~StabilityCanvasInvoke() override
    {
        OH_Drawing_BitmapDestroy(bitmap1_);
    };

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;

private:
    OH_Drawing_Bitmap* bitmap1_ = nullptr;
    void CanvasBindTest(OH_Drawing_Canvas* canvas);
    using HandlerFunc = std::function<void(OH_Drawing_Canvas*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { SETTING_FUNCTION_CANVAS_BIND, [this](OH_Drawing_Canvas* canvas) { return CanvasBindTest(canvas); } },
    };
};

// Save-SaveLayer-Restore-RestoreToCount循环
class StabilityCanvasSave : public TestBase {
public:
    StabilityCanvasSave()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityCanvasSave";
    }
    ~StabilityCanvasSave() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// AttachPen/Brush-draw-DetachPen/Brush循环
class StabilityCanvasAttach : public TestBase {
public:
    StabilityCanvasAttach()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityCanvasAttach";
    }
    ~StabilityCanvasAttach() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// Canvas相关操作接口随机循环调用
class StabilityCanvasRandInvoke : public TestBase {
public:
    StabilityCanvasRandInvoke()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityCanvasRandInvoke";
        // 0, 0, 256, 256 创建矩形
        src_ = OH_Drawing_RectCreate(0, 0, 256, 256);
        // 100, 100, 256, 256 指定区域
        dst_ = OH_Drawing_RectCreate(100, 100, 256, 256);
        // 100, 100 for test
        point_ = OH_Drawing_PointCreate(100, 100);
        // 5.0, 10.0 for test
        roundRect_ = OH_Drawing_RoundRectCreate(src_, 5.0, 10.0);
        font_ = OH_Drawing_FontCreate();
        textBlob_ = OH_Drawing_TextBlobCreateFromString(
            "test textBlob", font_, DrawGetEnum(TEXT_ENCODING_UTF8, TEXT_ENCODING_GLYPH_ID, rand_.nextU()));
        path_ = OH_Drawing_PathCreate();
        // 100,100,40 for test
        OH_Drawing_PathAddCircle(path_, 100, 100, 40, PATH_DIRECTION_CW);
        region_ = OH_Drawing_RegionCreate();
        OH_Drawing_RegionSetRect(region_, src_);
        matrix_ = OH_Drawing_MatrixCreate();
        CreatePixelMap();
    }
    ~StabilityCanvasRandInvoke() override;

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;

private:
    TestRend rand_;

    OH_Drawing_Rect* src_ = nullptr;
    OH_Drawing_Rect* dst_ = nullptr;
    OH_Drawing_Point* point_ = nullptr;
    OH_Drawing_RoundRect* roundRect_ = nullptr;
    OH_Drawing_Font* font_ = nullptr;
    OH_Drawing_TextBlob* textBlob_ = nullptr;
    OH_Drawing_Path* path_;
    OH_Drawing_Region* region_;
    OH_Drawing_Matrix* matrix_;
    OH_Pixelmap_InitializationOptions* createOps_ = nullptr;
    OH_Drawing_SamplingOptions* samplingOptions_ = nullptr;
    OH_Drawing_PixelMap* pixelMap_ = nullptr;
    OH_PixelmapNative* pixelMapNative_ = nullptr;
    OH_Drawing_Bitmap* bitmap1_ = nullptr;
    OH_Drawing_Image* image_ = nullptr;
    void* bitmapAddr_ = nullptr;
    void CreatePixelMap();
    void CanvasDrawLineTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawPathTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawPixelMapRectTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawBackgroundTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawRegionTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawPointTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawPointsTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawBitmapTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawBitmapRectTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawRectTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawCircleTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawColorTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawOvalTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawArcTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawRoundRectTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawSingleCharacterTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawTextBlobTest(OH_Drawing_Canvas* canvas);
    void CanvasClipRectTest(OH_Drawing_Canvas* canvas);
    void CanvasClipRoundRectTest(OH_Drawing_Canvas* canvas);
    void CanvasClipPathTest(OH_Drawing_Canvas* canvas);
    void CanvasClipRegionTest(OH_Drawing_Canvas* canvas);
    void CanvasRotateTest(OH_Drawing_Canvas* canvas);
    void CanvasTranslateTest(OH_Drawing_Canvas* canvas);
    void CanvasScaleTest(OH_Drawing_Canvas* canvas);
    void CanvasSkewTest(OH_Drawing_Canvas* canvas);
    void CanvasGetWidthTest(OH_Drawing_Canvas* canvas);
    void CanvasGetHeightTest(OH_Drawing_Canvas* canvas);
    void CanvasGetLocalClipBoundsTest(OH_Drawing_Canvas* canvas);
    void CanvasGetTotalMatrixTest(OH_Drawing_Canvas* canvas);
    void CanvasConcatMatrixTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawShadowTest(OH_Drawing_Canvas* canvas);
    void CanvasClearTest(OH_Drawing_Canvas* canvas);
    void CanvasSetMatrixTest(OH_Drawing_Canvas* canvas);
    void CanvasResetMatrixTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawImageRectWithSrcTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawImageRectTest(OH_Drawing_Canvas* canvas);
    void CanvasDrawVerticesTest(OH_Drawing_Canvas* canvas);
    void CanvasReadPixelsTest(OH_Drawing_Canvas* canvas);
    void CanvasReadPixelsToBitmapTest(OH_Drawing_Canvas* canvas);
    void CanvasIsClipEmptyTest(OH_Drawing_Canvas* canvas);
    void CanvasGetImageInfoTest(OH_Drawing_Canvas* canvas);
    using HandlerFunc = std::function<void(OH_Drawing_Canvas*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { OPERATION_FUNCTION_CANVAS_DRAW_LINE,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawLineTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_PATH,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawPathTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_PIXEL_MAP_RECT,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawPixelMapRectTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_BACKGROUND,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawBackgroundTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_REGION,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawRegionTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_POINT,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawPointTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_POINTS,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawPointsTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_BITMAP,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawBitmapTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_BITMAP_RECT,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawBitmapRectTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_RECT,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawRectTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_CIRCLE,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawCircleTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_COLOR,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawColorTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_OVAL,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawOvalTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_ARC, [this](OH_Drawing_Canvas* canvas) { return CanvasDrawArcTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_ROUND_RECT,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawRoundRectTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_SINGLE_CHARACTER,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawSingleCharacterTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_TEXT_BLOB,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawTextBlobTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_CLIP_RECT,
            [this](OH_Drawing_Canvas* canvas) { return CanvasClipRectTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_CLIP_ROUND_RECT,
            [this](OH_Drawing_Canvas* canvas) { return CanvasClipRoundRectTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_CLIP_PATH,
            [this](OH_Drawing_Canvas* canvas) { return CanvasClipPathTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_CLIP_REGION,
            [this](OH_Drawing_Canvas* canvas) { return CanvasClipRegionTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_ROTATE, [this](OH_Drawing_Canvas* canvas) { return CanvasRotateTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_TRANSLATE,
            [this](OH_Drawing_Canvas* canvas) { return CanvasTranslateTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_SCALE, [this](OH_Drawing_Canvas* canvas) { return CanvasScaleTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_SKEW, [this](OH_Drawing_Canvas* canvas) { return CanvasSkewTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_GET_WIDTH,
            [this](OH_Drawing_Canvas* canvas) { return CanvasGetWidthTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_GET_HEIGHT,
            [this](OH_Drawing_Canvas* canvas) { return CanvasGetHeightTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_GET_LOCAL_CLIP_BOUNDS,
            [this](OH_Drawing_Canvas* canvas) { return CanvasGetLocalClipBoundsTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_GET_TOTAL_MATRIX,
            [this](OH_Drawing_Canvas* canvas) { return CanvasGetTotalMatrixTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_CONCAT_MATRIX,
            [this](OH_Drawing_Canvas* canvas) { return CanvasConcatMatrixTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_SHADOW,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawShadowTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_CLEAR, [this](OH_Drawing_Canvas* canvas) { return CanvasClearTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_SET_MATRIX,
            [this](OH_Drawing_Canvas* canvas) { return CanvasSetMatrixTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_RESET_MATRIX,
            [this](OH_Drawing_Canvas* canvas) { return CanvasResetMatrixTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_IMAGE_RECT_WITH_SRC,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawImageRectWithSrcTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_IMAGE_RECT,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawImageRectTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_DRAW_VERTICES,
            [this](OH_Drawing_Canvas* canvas) { return CanvasDrawVerticesTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_READ_PIXELS,
            [this](OH_Drawing_Canvas* canvas) { return CanvasReadPixelsTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_READ_PIXELS_TO_BITMAP,
            [this](OH_Drawing_Canvas* canvas) { return CanvasReadPixelsToBitmapTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_IS_CLIP_EMPTY,
            [this](OH_Drawing_Canvas* canvas) { return CanvasIsClipEmptyTest(canvas); } },
        { OPERATION_FUNCTION_CANVAS_GET_IMAGE_INFO,
            [this](OH_Drawing_Canvas* canvas) { return CanvasGetImageInfoTest(canvas); } }
    };
};
#endif // STABILITY_CANVAS_TEST_H