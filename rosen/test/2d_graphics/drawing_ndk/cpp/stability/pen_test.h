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

#ifndef STABILITY_PEN_TEST_H
#define STABILITY_PEN_TEST_H
#include <map>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_shadow_layer.h>

#include "test_base.h"
#include "test_common.h"

typedef enum {
    SETTING_FUNCTION_PEN_SET_ANTIALIAS,
    SETTING_FUNCTION_PEN_SET_COLOR,
    SETTING_FUNCTION_PEN_SET_ALPHA,
    SETTING_FUNCTION_PEN_SET_WIDTH,
    SETTING_FUNCTION_PEN_SET_MITER_LIMIT,
    SETTING_FUNCTION_PEN_SET_CAP,
    SETTING_FUNCTION_PEN_SET_JOIN,
    SETTING_FUNCTION_PEN_SET_SHADER_EFFECT,
    SETTING_FUNCTION_PEN_SET_SHADOW_LAYER,
    SETTING_FUNCTION_PEN_SET_PATH_EFFECT,
    SETTING_FUNCTION_PEN_SET_FILTER,
    SETTING_FUNCTION_PEN_SET_BLEND_MODE,
    SETTING_FUNCTION_PEN_RESET,
    SETTING_FUNCTION_PEN_MAX,
} SettingFunctionPen;

typedef enum {
    OPERATION_FUNCTION_PEN_IS_ANTIALIAS,
    OPERATION_FUNCTION_PEN_GET_COLOR,
    OPERATION_FUNCTION_PEN_GET_ALPHA,
    OPERATION_FUNCTION_PEN_GET_WIDTH,
    OPERATION_FUNCTION_PEN_GET_MITER_LIMIT,
    OPERATION_FUNCTION_PEN_GET_CAP,
    OPERATION_FUNCTION_PEN_GET_JOIN,
    OPERATION_FUNCTION_PEN_GET_FILTER,
} OperationFunctionPen;

// PenCreate-PenDestroy循环
class StabilityPenCreate : public TestBase {
public:
    StabilityPenCreate()
    {
        fileName_ = "StabilityPenCreate";
    }
    ~StabilityPenCreate() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// PenCopy-PenDestroy循环
class StabilityPenCopy : public TestBase {
public:
    StabilityPenCopy()
    {
        fileName_ = "StabilityPenCopy";
    }
    ~StabilityPenCopy() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// PenCreate-相关配置接口全调用-PenDestroy循环-PenAllFunc
class StabilityPenAllFunc : public TestBase {
public:
    StabilityPenAllFunc()
    {
        fileName_ = "StabilityPenAllFunc";
        colorShaderEffect_ = OH_Drawing_ShaderEffectCreateColorShader(rand_.nextULessThan(0xffffffff));
        startPt_ = OH_Drawing_PointCreate(rand_.nextRangeF(0, bitmapWidth_), rand_.nextRangeF(0, bitmapHeight_));
        endPt_ = OH_Drawing_PointCreate(rand_.nextRangeF(0, bitmapWidth_), rand_.nextRangeF(0, bitmapHeight_));

        linearGradientEffect_ = OH_Drawing_ShaderEffectCreateLinearGradient(
            startPt_, endPt_, Color_, pos_, 2, DrawGetEnum(CLAMP, DECAL, rand_.nextU()));
        matrix_ =
            OH_Drawing_MatrixCreateTranslation(rand_.nextRangeF(0, bitmapWidth_), rand_.nextRangeF(0, bitmapHeight_));
        OH_Drawing_Point2D startPt1 = { rand_.nextRangeF(0, bitmapWidth_), rand_.nextRangeF(0, bitmapHeight_) };
        OH_Drawing_Point2D endPt1 = { rand_.nextRangeF(0, bitmapWidth_), rand_.nextRangeF(0, bitmapHeight_) };
        linearGradientEffectLocal_ = OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(
            &startPt1, &endPt1, Color_, pos_, 2, DrawGetEnum(CLAMP, DECAL, rand_.nextU()), matrix_);
        centerPt_ = OH_Drawing_PointCreate(rand_.nextRangeF(0, bitmapWidth_), rand_.nextRangeF(0, bitmapHeight_));
        // 5 OH_Drawing_TileMode enum max
        radialGradientEffect_ = OH_Drawing_ShaderEffectCreateRadialGradient(
            centerPt_, radius_, Color_, pos_, 2, DrawGetEnum(CLAMP, DECAL, rand_.nextU())); // 2 for test
        radialGradientEffectLocal_ = OH_Drawing_ShaderEffectCreateRadialGradientWithLocalMatrix(
            &startPt1, radius_, Color_, pos_, 2, DrawGetEnum(CLAMP, DECAL, rand_.nextU()), matrix_); // 2 for test

        sweepGradientEffect_ = OH_Drawing_ShaderEffectCreateSweepGradient(
            startPt_, Color_, pos_, 2, DrawGetEnum(CLAMP, DECAL, rand_.nextU())); // 2 for test

        image_ = OH_Drawing_ImageCreate();
        option_ = OH_Drawing_SamplingOptionsCreate(DrawGetEnum(FILTER_MODE_NEAREST, FILTER_MODE_LINEAR, rand_.nextU()),
            DrawGetEnum(MIPMAP_MODE_NONE, MIPMAP_MODE_LINEAR, rand_.nextU()));
        imageShaderEffect_ = OH_Drawing_ShaderEffectCreateImageShader(
            image_, DrawGetEnum(CLAMP, DECAL, rand_.nextU()), DrawGetEnum(CLAMP, DECAL, rand_.nextU()), option_,
            matrix_);

        float startRadius = rand_.nextRangeF(0, bitmapWidth_);
        float endRadius = rand_.nextRangeF(0, bitmapWidth_);

        twoPointConicalGradientEffect_ = OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(&startPt1, startRadius,
            &endPt1, endRadius, Color_, pos_, 2, DrawGetEnum(CLAMP, DECAL, rand_.nextU()), matrix_); // 2 for test

        shadowLayer_ = OH_Drawing_ShadowLayerCreate(rand_.nextRangeF(1, 10), rand_.nextRangeF(-5, 10),
            rand_.nextRangeF(-5, 10), rand_.nextULessThan(0xffffffff)); // 1, 10, -5, 10 for test
        float intervals = rand_.nextRangeF(1, 30);                      // 1, 30 for test
        pathEffect_ = OH_Drawing_CreateDashPathEffect(
            &intervals, rand_.nextULessThan(10), rand_.nextRangeF(1, 10)); // 10, 1, 10 for test
        filter_ = OH_Drawing_FilterCreate();
    }
    ~StabilityPenAllFunc() override
    {
        OH_Drawing_ShaderEffectDestroy(colorShaderEffect_);
        OH_Drawing_PointDestroy(startPt_);
        OH_Drawing_PointDestroy(endPt_);
        OH_Drawing_ShaderEffectDestroy(linearGradientEffect_);
        OH_Drawing_MatrixDestroy(matrix_);
        OH_Drawing_ShaderEffectDestroy(linearGradientEffectLocal_);
        OH_Drawing_PointDestroy(centerPt_);

        OH_Drawing_ShaderEffectDestroy(radialGradientEffect_);

        OH_Drawing_ShaderEffectDestroy(radialGradientEffectLocal_);
        OH_Drawing_ShaderEffectDestroy(sweepGradientEffect_);
        OH_Drawing_ImageDestroy(image_);
        OH_Drawing_SamplingOptionsDestroy(option_);

        OH_Drawing_ShaderEffectDestroy(imageShaderEffect_);
        OH_Drawing_ShaderEffectDestroy(twoPointConicalGradientEffect_);
        OH_Drawing_ShadowLayerDestroy(shadowLayer_);
        OH_Drawing_PathEffectDestroy(pathEffect_);
        OH_Drawing_FilterDestroy(filter_);
    };

private:
    TestRend rand_;
    const float pos_[2] = { 0.0f, 1.0f };
    uint32_t Color_[2] = { rand_.nextULessThan(0xffffffff), rand_.nextULessThan(0xffffffff) };
    float radius_ = rand_.nextRangeF(0, bitmapWidth_);
    OH_Drawing_ShaderEffect* colorShaderEffect_ = nullptr;
    OH_Drawing_Point* startPt_ = nullptr;
    OH_Drawing_Point* endPt_ = nullptr;
    OH_Drawing_ShaderEffect* linearGradientEffect_ = nullptr;
    OH_Drawing_Matrix* matrix_ = nullptr;
    OH_Drawing_ShaderEffect* linearGradientEffectLocal_;
    OH_Drawing_Point* centerPt_ = nullptr;
    OH_Drawing_ShaderEffect* radialGradientEffect_ = nullptr;
    OH_Drawing_ShaderEffect* radialGradientEffectLocal_ = nullptr;
    OH_Drawing_ShaderEffect* sweepGradientEffect_ = nullptr;
    OH_Drawing_Image* image_ = nullptr;
    OH_Drawing_SamplingOptions* option_ = nullptr;
    OH_Drawing_ShaderEffect* imageShaderEffect_ = nullptr;
    OH_Drawing_ShaderEffect* twoPointConicalGradientEffect_ = nullptr;
    OH_Drawing_ShadowLayer* shadowLayer_ = nullptr;
    OH_Drawing_PathEffect* pathEffect_ = nullptr;
    OH_Drawing_Filter* filter_ = nullptr;

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
    void PenSetAntiAliasTest(OH_Drawing_Pen* pen);
    void PenSetColorTest(OH_Drawing_Pen* pen);
    void PenSetAlphaTest(OH_Drawing_Pen* pen);
    void PenSetWidthTest(OH_Drawing_Pen* pen);
    void PenSetMiterLimitTest(OH_Drawing_Pen* pen);
    void PenSetCapTest(OH_Drawing_Pen* pen);
    void PenSetJoinTest(OH_Drawing_Pen* pen);
    void PenSetShaderEffectTest(OH_Drawing_Pen* pen);
    void PenSetShadowLayerTest(OH_Drawing_Pen* pen);
    void PenSetPathEffectTest(OH_Drawing_Pen* pen);
    void PenSetFilterTest(OH_Drawing_Pen* pen);
    void PenSetBlendModeTest(OH_Drawing_Pen* pen);
    void PenResetTest(OH_Drawing_Pen* pen);
    using HandlerFunc = std::function<void(OH_Drawing_Pen* pen)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { SETTING_FUNCTION_PEN_SET_ANTIALIAS, [this](OH_Drawing_Pen* pen) { return PenSetAntiAliasTest(pen); } },
        { SETTING_FUNCTION_PEN_SET_COLOR, [this](OH_Drawing_Pen* pen) { return PenSetColorTest(pen); } },
        { SETTING_FUNCTION_PEN_SET_ALPHA, [this](OH_Drawing_Pen* pen) { return PenSetAlphaTest(pen); } },
        { SETTING_FUNCTION_PEN_SET_WIDTH, [this](OH_Drawing_Pen* pen) { return PenSetWidthTest(pen); } },
        { SETTING_FUNCTION_PEN_SET_MITER_LIMIT, [this](OH_Drawing_Pen* pen) { return PenSetMiterLimitTest(pen); } },
        { SETTING_FUNCTION_PEN_SET_CAP, [this](OH_Drawing_Pen* pen) { return PenSetCapTest(pen); } },
        { SETTING_FUNCTION_PEN_SET_JOIN, [this](OH_Drawing_Pen* pen) { return PenSetJoinTest(pen); } },
        { SETTING_FUNCTION_PEN_SET_SHADER_EFFECT, [this](OH_Drawing_Pen* pen) { return PenSetShaderEffectTest(pen); } },
        { SETTING_FUNCTION_PEN_SET_SHADOW_LAYER, [this](OH_Drawing_Pen* pen) { return PenSetShadowLayerTest(pen); } },
        { SETTING_FUNCTION_PEN_SET_PATH_EFFECT, [this](OH_Drawing_Pen* pen) { return PenSetPathEffectTest(pen); } },
        { SETTING_FUNCTION_PEN_SET_FILTER, [this](OH_Drawing_Pen* pen) { return PenSetFilterTest(pen); } },
        { SETTING_FUNCTION_PEN_SET_BLEND_MODE, [this](OH_Drawing_Pen* pen) { return PenSetBlendModeTest(pen); } },
        { SETTING_FUNCTION_PEN_RESET, [this](OH_Drawing_Pen* pen) { return PenResetTest(pen); } },
    };
};

// Pen相关操作接口随机循环调用-PenRandomFunc
class StabilityPenRandomFunc : public TestBase {
public:
    StabilityPenRandomFunc()
    {
        fileName_ = "StabilityPenRandomFunc";
    }
    ~StabilityPenRandomFunc() override {};

private:
    OH_Drawing_Filter* filter_ = nullptr;
    OH_Drawing_Path* path_ = nullptr;
    OH_Drawing_Path* srcPath_ = nullptr;
    OH_Drawing_Path* dstPath_ = nullptr;
    OH_Drawing_Rect* rect_ = nullptr;
    OH_Drawing_Matrix* matrix_ = nullptr;

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
    void PenIsAntiAliasTest(OH_Drawing_Pen* pen);
    void PenGetColorTest(OH_Drawing_Pen* pen);
    void PenGetAlphaTest(OH_Drawing_Pen* pen);
    void PenGetWidthTest(OH_Drawing_Pen* pen);
    void PenGetMiterLimitTest(OH_Drawing_Pen* pen);
    void PenGetCapTest(OH_Drawing_Pen* pen);
    void PenGetJoinTest(OH_Drawing_Pen* pen);
    void PenGetFilterTest(OH_Drawing_Pen* pen);
    using HandlerFunc = std::function<void(OH_Drawing_Pen* pen)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { OPERATION_FUNCTION_PEN_IS_ANTIALIAS, [this](OH_Drawing_Pen* pen) { return PenIsAntiAliasTest(pen); } },
        { OPERATION_FUNCTION_PEN_GET_COLOR, [this](OH_Drawing_Pen* pen) { return PenGetColorTest(pen); } },
        { OPERATION_FUNCTION_PEN_GET_ALPHA, [this](OH_Drawing_Pen* pen) { return PenGetAlphaTest(pen); } },
        { OPERATION_FUNCTION_PEN_GET_WIDTH, [this](OH_Drawing_Pen* pen) { return PenGetWidthTest(pen); } },
        { OPERATION_FUNCTION_PEN_GET_MITER_LIMIT, [this](OH_Drawing_Pen* pen) { return PenGetMiterLimitTest(pen); } },
        { OPERATION_FUNCTION_PEN_GET_CAP, [this](OH_Drawing_Pen* pen) { return PenGetCapTest(pen); } },
        { OPERATION_FUNCTION_PEN_GET_JOIN, [this](OH_Drawing_Pen* pen) { return PenGetJoinTest(pen); } },
        { OPERATION_FUNCTION_PEN_GET_FILTER, [this](OH_Drawing_Pen* pen) { return PenGetFilterTest(pen); } },
    };
};

#endif // STABILITY_PEN_TEST_H