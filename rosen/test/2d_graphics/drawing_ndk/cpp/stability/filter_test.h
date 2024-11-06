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

#ifndef STABILITY_FILTER_TEST_H
#define STABILITY_FILTER_TEST_H
#include <map>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_image_filter.h>
#include <native_drawing/drawing_mask_filter.h>

#include "test_base.h"

typedef enum {
    // Operation function
    OPERATION_FUNCTION_FILTER_GET_COLOR_FILTER = 0,
    OPERATION_FUNCTION_FILTER_MAX,
} OperationFunctionFilter;

typedef enum {
    // setting function
    SETTING_FUNCTION_FILTER_SET_IMAGE_FILTER = 0,
    SETTING_FUNCTION_FILTER_SET_MASK_FILTER,
    SETTING_FUNCTION_FILTER_SET_COLOR_FILTER,
    SETTING_FUNCTION_FILTER_MAX
} SettingFunctionFilter;

// FilterCreate-FilterDestroy循环
class StabilityFilterCreate : public TestBase {
public:
    StabilityFilterCreate()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityFilterCreate";
    }
    ~StabilityFilterCreate() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// FilterCreate-相关配置接口全调用-FilterDestroy循环
class StabilityFilterInvoke : public TestBase {
public:
    StabilityFilterInvoke()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityFilterInvoke";

        colorFilter_ = OH_Drawing_ColorFilterCreateSrgbGammaToLinear();
        imageFilter_ = OH_Drawing_ImageFilterCreateFromColorFilter(colorFilter_, nullptr);
        float sigma = 10.f; // 10.f 高斯模糊的标准偏差
        // 2 for test,rand true or false
        OH_Drawing_MaskFilter* maskFilter = OH_Drawing_MaskFilterCreateBlur(OH_Drawing_BlurType::NORMAL, sigma, true);
    }
    ~StabilityFilterInvoke() override
    {
        OH_Drawing_ColorFilterDestroy(colorFilter_);
        OH_Drawing_ImageFilterDestroy(imageFilter_);
        OH_Drawing_MaskFilterDestroy(maskFilter_);
    };

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;

private:
    OH_Drawing_ColorFilter* colorFilter_ = nullptr;
    OH_Drawing_ImageFilter* imageFilter_ = nullptr;
    OH_Drawing_MaskFilter* maskFilter_ = nullptr;
    void FilterSetImageFilterTest(OH_Drawing_Filter* filter);
    void FilterSetMaskFilterTest(OH_Drawing_Filter* filter);
    void FilterSetColorFilterTest(OH_Drawing_Filter* filter);
    using HandlerFunc = std::function<void(OH_Drawing_Filter*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { OPERATION_FUNCTION_FILTER_GET_COLOR_FILTER,
            [this](OH_Drawing_Filter* filter) { return FilterSetImageFilterTest(filter); } },
        { SETTING_FUNCTION_FILTER_SET_MASK_FILTER,
            [this](OH_Drawing_Filter* filter) { return FilterSetMaskFilterTest(filter); } },
        { SETTING_FUNCTION_FILTER_SET_COLOR_FILTER,
            [this](OH_Drawing_Filter* filter) { return FilterSetColorFilterTest(filter); } },
    };
};

// Filter相关操作接口随机循环调用
class StabilityFilterRandInvoke : public TestBase {
public:
    StabilityFilterRandInvoke()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityFilterRandInvoke";
        colorFilter_ = OH_Drawing_ColorFilterCreateSrgbGammaToLinear();
    }
    ~StabilityFilterRandInvoke() override
    {
        OH_Drawing_ColorFilterDestroy(colorFilter_);
    };

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;

private:
    OH_Drawing_ColorFilter* colorFilter_ = nullptr;
    void FilterGetColorFilterTest(OH_Drawing_Filter* filter);
    using HandlerFunc = std::function<void(OH_Drawing_Filter*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { OPERATION_FUNCTION_FILTER_GET_COLOR_FILTER,
            [this](OH_Drawing_Filter* filter) { return FilterGetColorFilterTest(filter); } },
    };
};

// ImageFilterCreateBlur-ImageFilterDestroy循环
class StabilityImageFilterCreateBlur : public TestBase {
public:
    StabilityImageFilterCreateBlur()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityImageFilterCreateBlur";
    }
    ~StabilityImageFilterCreateBlur() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ImageFilterCreateFromColorFilter-ImageFilterDestroy循环
class StabilityImageFilterCreateFromColorFilter : public TestBase {
public:
    StabilityImageFilterCreateFromColorFilter()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityImageFilterCreateFromColorFilter";
    }
    ~StabilityImageFilterCreateFromColorFilter() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

#endif // STABILITY_FILTER_TEST_H