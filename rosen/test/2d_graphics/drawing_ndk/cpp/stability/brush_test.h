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

#ifndef STABILITY_BRUSH_TEST_H
#define STABILITY_BRUSH_TEST_H
#include <map>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_shadow_layer.h>

#include "test_base.h"
#include "test_common.h"


typedef enum {
    // Operation function
    OPERATION_FUNCTION_BRUSH_IS_ANTI_ALIAS = 0,
    OPERATION_FUNCTION_BRUSH_GET_COLOR,
    OPERATION_FUNCTION_BRUSH_GET_ALPHA,
    OPERATION_FUNCTION_BRUSH_GET_FILTER,
    OPERATION_FUNCTION_BRUSH_RESET,
    OPERATION_FUNCTION_BRUSH_MAX,
} OperationFunctionBrush;

typedef enum {
    // setting function
    SETTING_FUNCTION_BRUSH_SET_ANTI_ALIAS = 0,
    SETTING_FUNCTION_BRUSH_SET_COLOR,
    SETTING_FUNCTION_BRUSH_SET_ALPHA,
    SETTING_FUNCTION_BRUSH_SET_FILTER,
    SETTING_FUNCTION_BRUSH_SET_SHADER_EFFECT,
    SETTING_FUNCTION_BRUSH_SET_SHADOW_LAYER,
    SETTING_FUNCTION_BRUSH_SET_SET_BLEND_MODE,
    SETTING_FUNCTION_BRUSH_MAX
} SettinFunctionBrush;

// BrushCreate-相关配置接口全调用-BrushDestroy循环调用
class StabilityBrushCreate : public TestBase {
public:
    StabilityBrushCreate()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityBrushCreate";
        filter_ = OH_Drawing_FilterCreate();
        effect_ = OH_Drawing_ShaderEffectCreateColorShader(rand_.nextU());
        layer_ = OH_Drawing_ShadowLayerCreate(
            rand_.nextRangeF(1, bitmapWidth_), rand_.nextF(), rand_.nextF(), rand_.nextU());
    }
    ~StabilityBrushCreate() override
    {
        OH_Drawing_FilterDestroy(filter_);
        OH_Drawing_ShaderEffectDestroy(effect_);
        OH_Drawing_ShadowLayerDestroy(layer_);
    };

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;

private:
    TestRend rand_;
    OH_Drawing_Filter* filter_ = nullptr;
    OH_Drawing_ShaderEffect* effect_ = nullptr;
    OH_Drawing_ShadowLayer* layer_ = nullptr;
    void BrushSetAntiAliasTest(OH_Drawing_Brush* brush);
    void BrushSetColorTest(OH_Drawing_Brush* brush);
    void BrushSetAlphaTest(OH_Drawing_Brush* brush);
    void BrushSetFilterTest(OH_Drawing_Brush* brush);
    void BrushSetShaderEffectTest(OH_Drawing_Brush* brush);
    void BrushSetShadowLayerTest(OH_Drawing_Brush* brush);
    void BrushSetSetBlendModeTest(OH_Drawing_Brush* brush);
    using HandlerFunc = std::function<void(OH_Drawing_Brush*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { SETTING_FUNCTION_BRUSH_SET_ANTI_ALIAS,
            [this](OH_Drawing_Brush* brush) { return BrushSetAntiAliasTest(brush); } },
        { SETTING_FUNCTION_BRUSH_SET_COLOR, [this](OH_Drawing_Brush* brush) { return BrushSetColorTest(brush); } },
        { SETTING_FUNCTION_BRUSH_SET_ALPHA, [this](OH_Drawing_Brush* brush) { return BrushSetAlphaTest(brush); } },
        { SETTING_FUNCTION_BRUSH_SET_FILTER, [this](OH_Drawing_Brush* brush) { return BrushSetFilterTest(brush); } },
        { SETTING_FUNCTION_BRUSH_SET_SHADER_EFFECT,
            [this](OH_Drawing_Brush* brush) { return BrushSetShaderEffectTest(brush); } },
        { SETTING_FUNCTION_BRUSH_SET_SHADOW_LAYER,
            [this](OH_Drawing_Brush* brush) { return BrushSetShadowLayerTest(brush); } },
        { SETTING_FUNCTION_BRUSH_SET_SET_BLEND_MODE,
            [this](OH_Drawing_Brush* brush) { return BrushSetSetBlendModeTest(brush); } },
    };
};

class StabilityBrushCopy : public TestBase {
public:
    StabilityBrushCopy()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityBrushCopy";
    }
    ~StabilityBrushCopy() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// Brush相关操作随机循环调用
class StabilityBrushRandInvoke : public TestBase {
public:
    StabilityBrushRandInvoke()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityBrushRandInvoke";
        filter_ = OH_Drawing_FilterCreate();
    }
    ~StabilityBrushRandInvoke() override
    {
        OH_Drawing_FilterDestroy(filter_);
    };

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;

private:
    TestRend rand_;
    OH_Drawing_Filter* filter_ = nullptr;
    void BrushIsAntiAliasTest(OH_Drawing_Brush* brush);
    void BrushGetColorTest(OH_Drawing_Brush* brush);
    void BrushGetAlphaTest(OH_Drawing_Brush* brush);
    void BrushGetFilterTest(OH_Drawing_Brush* brush);
    void BrushResetTest(OH_Drawing_Brush* brush);
    using HandlerFunc = std::function<void(OH_Drawing_Brush*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { OPERATION_FUNCTION_BRUSH_IS_ANTI_ALIAS,
            [this](OH_Drawing_Brush* brush) { return BrushIsAntiAliasTest(brush); } },
        { OPERATION_FUNCTION_BRUSH_GET_COLOR, [this](OH_Drawing_Brush* brush) { return BrushGetColorTest(brush); } },
        { OPERATION_FUNCTION_BRUSH_GET_ALPHA, [this](OH_Drawing_Brush* brush) { return BrushGetAlphaTest(brush); } },
        { OPERATION_FUNCTION_BRUSH_GET_FILTER, [this](OH_Drawing_Brush* brush) { return BrushGetFilterTest(brush); } },
        { OPERATION_FUNCTION_BRUSH_RESET, [this](OH_Drawing_Brush* brush) { return BrushResetTest(brush); } }
    };
};

#endif // STABILITY_BRUSH_TEST_H