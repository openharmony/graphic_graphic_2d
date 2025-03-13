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

#ifndef STABILITY_BITMAP_TEST_H
#define STABILITY_BITMAP_TEST_H
#include <map>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

typedef enum {
    // Operation function
    OPERATION_FUNCTION_BITMAP_GET_WIDTH = 0,
    OPERATION_FUNCTION_BITMAP_GET_HEIGHT,
    OPERATION_FUNCTION_BITMAP_GET_COLOR_FORMAT,
    OPERATION_FUNCTION_BITMAP_GET_ALPHA_FORMAT,
    OPERATION_FUNCTION_BITMAP_GET_PIXELS,
    OPERATION_FUNCTION_BITMAP_GET_IMAGE_INFO,
    OPERATION_FUNCTION_BITMAP_READ_PIXELS,
    OPERATION_FUNCTION_BITMAP_MAX,
} OperationFunctionBitmap;

typedef enum {
    // Operation function
    OPERATION_FUNCTION_IMAGE_GET_WIDTH = 0,
    OPERATION_FUNCTION_IMAGE_GET_HEIGHT,
    OPERATION_FUNCTION_IMAGE_GET_IMAGE_INFO,
    OPERATION_FUNCTION_IMAGE_MAX,
} OperationFunctionImage;

// BitmapCreate-BitmapBuild-相关配置接口全调用-BitmapDestroy循环调用
// 无配置接口
class StabilityBitmapCreate : public TestBase {
public:
    StabilityBitmapCreate()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityBitmapCreate";
    }
    ~StabilityBitmapCreate() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// BitmapCreateFromPixels-相关配置接口全调用-BitmapDestroy循环
// 无配置接口
class StabilityBitmapCreateFromPixels : public TestBase {
public:
    StabilityBitmapCreateFromPixels()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityBitmapCreateFromPixels";
    }
    ~StabilityBitmapCreateFromPixels() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// BitmapBuild多次循环调用
class StabilityBitmapInit : public TestBase {
public:
    StabilityBitmapInit()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityBitmapInit";
    }
    ~StabilityBitmapInit() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// Bitmap相关操作随机循环调用
class StabilityBitmapRandInvoke : public TestBase {
public:
    StabilityBitmapRandInvoke()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityBitmapRandInvoke";
    }
    ~StabilityBitmapRandInvoke() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;

private:
    void BitmapGetWidthTest(OH_Drawing_Bitmap* bitmap);
    void BitmapGetHeightTest(OH_Drawing_Bitmap* bitmap);
    void BitmapGetColorFormatTest(OH_Drawing_Bitmap* bitmap);
    void BitmapGetAlphaFormatTest(OH_Drawing_Bitmap* bitmap);
    void BitmapGetPixelsTest(OH_Drawing_Bitmap* bitmap);
    void BitmapGetImageInfoTest(OH_Drawing_Bitmap* bitmap);
    void BitmapReadPixelsTest(OH_Drawing_Bitmap* bitmap);
    using HandlerFunc = std::function<void(OH_Drawing_Bitmap*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { OPERATION_FUNCTION_BITMAP_GET_WIDTH,
            [this](OH_Drawing_Bitmap* bitmap) { return BitmapGetWidthTest(bitmap); } },
        { OPERATION_FUNCTION_BITMAP_GET_HEIGHT,
            [this](OH_Drawing_Bitmap* bitmap) { return BitmapGetHeightTest(bitmap); } },
        { OPERATION_FUNCTION_BITMAP_GET_COLOR_FORMAT,
            [this](OH_Drawing_Bitmap* bitmap) { return BitmapGetColorFormatTest(bitmap); } },
        { OPERATION_FUNCTION_BITMAP_GET_ALPHA_FORMAT,
            [this](OH_Drawing_Bitmap* bitmap) { return BitmapGetAlphaFormatTest(bitmap); } },
        { OPERATION_FUNCTION_BITMAP_GET_PIXELS,
            [this](OH_Drawing_Bitmap* bitmap) { return BitmapGetPixelsTest(bitmap); } },
        { OPERATION_FUNCTION_BITMAP_GET_IMAGE_INFO,
            [this](OH_Drawing_Bitmap* bitmap) { return BitmapGetImageInfoTest(bitmap); } },
        { OPERATION_FUNCTION_BITMAP_READ_PIXELS,
            [this](OH_Drawing_Bitmap* bitmap) { return BitmapReadPixelsTest(bitmap); } }
    };
};

// ImageCreate-ImageBuildFromBitmap-ImageDestroy循环
class StabilityImageCreate : public TestBase {
public:
    StabilityImageCreate()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityImageCreate";
    }
    ~StabilityImageCreate() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ImageCreate-相关配置接口全调用-ImageDestroy循环
// 无配置接口
class StabilityImageInvoke : public TestBase {
public:
    StabilityImageInvoke()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityImageInvoke";
    }
    ~StabilityImageInvoke() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// Image相关操作接口随机循环调用
class StabilityImageRandInvoke : public TestBase {
public:
    StabilityImageRandInvoke()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityImageRandInvoke";
    }
    ~StabilityImageRandInvoke() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;

private:
    void ImageGetWidthTest(OH_Drawing_Image* image);
    void ImageGetHeightTest(OH_Drawing_Image* image);
    void ImageGetImageInfoTest(OH_Drawing_Image* image);
    using HandlerFunc = std::function<void(OH_Drawing_Image*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { OPERATION_FUNCTION_IMAGE_GET_WIDTH, [this](OH_Drawing_Image* image) { return ImageGetWidthTest(image); } },
        { OPERATION_FUNCTION_IMAGE_GET_HEIGHT, [this](OH_Drawing_Image* image) { return ImageGetHeightTest(image); } },
        { OPERATION_FUNCTION_IMAGE_GET_IMAGE_INFO,
            [this](OH_Drawing_Image* image) { return ImageGetImageInfoTest(image); } },
    };
};

#endif // STABILITY_BITMAP_TEST_H