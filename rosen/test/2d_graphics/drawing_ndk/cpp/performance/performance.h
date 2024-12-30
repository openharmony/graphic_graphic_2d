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

#ifndef DRAWINGNDKTEST5_0_PERFORMANCE_H
#define DRAWINGNDKTEST5_0_PERFORMANCE_H
#include <cstdint>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>

#include "hilog/log.h"
#include "native_drawing/drawing_error_code.h"
#include "native_drawing/drawing_pixel_map.h"
#include "native_drawing/drawing_shadow_layer.h"
#include "test_common.h"

#include "common/drawing_type.h"
#include "common/log_common.h"

class SpecialPerformance {
public:
    SpecialPerformance()
    {
        pen_ = OH_Drawing_PenCreate();
        brush_ = OH_Drawing_BrushCreate();
        // 10.0 for test
        maskFilter_ = OH_Drawing_MaskFilterCreateBlur(NORMAL, 10.0, true);
        filter_ = OH_Drawing_FilterCreate();
        colorFilter_ = OH_Drawing_ColorFilterCreateBlendMode(0xFFFF00FF, BLEND_MODE_DST_IN);
        float intervals[] = { 1.0, 1.0 };
        // 2 for test
        pathEffect_ = OH_Drawing_CreateDashPathEffect(intervals, 2, 0);
        // 100 for test
        styleCenter_ = OH_Drawing_PointCreate(100, 100);
        uint32_t styleColors[] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF };
        // 0.5, 1.0 for test
        float pos[] = { 0, 0.5, 1.0 };
        // 100, 3 for test
        shaderEffect_ = OH_Drawing_ShaderEffectCreateRadialGradient(
            styleCenter_, 100, styleColors, pos, 3, OH_Drawing_TileMode::CLAMP); // 100, 3 for test
        // 4.0, 3, 3 for test
        shadowLayer_ = OH_Drawing_ShadowLayerCreate(4.0, 3, 3, 0xFFFFFFFF);
        SetPenAndBrushEffect();
    };
    ~SpecialPerformance()
    {
        OH_Drawing_MaskFilterDestroy(maskFilter_);
        OH_Drawing_PointDestroy(styleCenter_);
        OH_Drawing_ShadowLayerDestroy(shadowLayer_);
        OH_Drawing_ShaderEffectDestroy(shaderEffect_);
        OH_Drawing_PathEffectDestroy(pathEffect_);
        OH_Drawing_ColorFilterDestroy(colorFilter_);
        OH_Drawing_FilterDestroy(filter_);
        OH_Drawing_PenDestroy(pen_);
        OH_Drawing_BrushDestroy(brush_);
    }
    OH_Drawing_Pen* GetPen()
    {
        return pen_;
    }
    static std::string GetAttachTypeName(int type)
    {
        switch (type) {
            case DRAWING_TYPE_ATTACH_BOTH: {
                return "PenAndBrush";
            }
            case DRAWING_TYPE_ATTACH_PEN: {
                return "Pen";
            }
            case DRAWING_TYPE_ATTACH_BRUSH: {
                return "Brush";
            }
        }
        return "None";
    }

    OH_Drawing_Brush* GetBrush()
    {
        return brush_;
    }
    void AttachWithType(OH_Drawing_Canvas* canvas, int type)
    {
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_CanvasDetachPen(canvas);
        switch (type) {
            case DRAWING_TYPE_ATTACH_BOTH: {
                OH_Drawing_CanvasAttachPen(canvas, pen_);
                OH_Drawing_CanvasAttachBrush(canvas, brush_);
                break;
            }
            case DRAWING_TYPE_ATTACH_PEN: {
                OH_Drawing_CanvasAttachPen(canvas, pen_);
                break;
            }
            case DRAWING_TYPE_ATTACH_BRUSH: {
                OH_Drawing_CanvasAttachBrush(canvas, brush_);
                break;
            }
        }
    }
    void SetPenAndBrushEffect()
    {
        SetPenEffect();
        SetBrushEffect();
    };

private:
    void SetPenEffect()
    {
        OH_Drawing_PenSetAlpha(pen_, 0xF0);
        // 5 for test
        OH_Drawing_PenSetWidth(pen_, 5);
        OH_Drawing_PenSetAntiAlias(pen_, true);
        OH_Drawing_PenSetBlendMode(pen_, BLEND_MODE_SRC);
        OH_Drawing_PenLineCapStyle lineCapStyle = LINE_FLAT_CAP;
        OH_Drawing_PenSetCap(pen_, lineCapStyle);
        OH_Drawing_PenSetColor(pen_, 0xFFFF0000);
        OH_Drawing_FilterSetColorFilter(filter_, colorFilter_);
        OH_Drawing_FilterSetMaskFilter(filter_, maskFilter_);
        OH_Drawing_PenSetFilter(pen_, filter_);

        OH_Drawing_PenLineJoinStyle lineJoinStyle = LINE_ROUND_JOIN;
        OH_Drawing_PenSetJoin(pen_, lineJoinStyle);
        // 10.0 for test
        OH_Drawing_PenSetMiterLimit(pen_, 10.0);

        OH_Drawing_PenSetPathEffect(pen_, pathEffect_);
        OH_Drawing_PenSetShaderEffect(pen_, shaderEffect_);
        // 3 for test
        OH_Drawing_PenSetWidth(pen_, 3);
        OH_Drawing_PenSetShadowLayer(pen_, shadowLayer_);
    };
    void SetBrushEffect()
    {
        OH_Drawing_BrushSetAlpha(brush_, 0xF0);
        OH_Drawing_BrushSetAntiAlias(brush_, true);
        OH_Drawing_BrushSetColor(brush_, 0xFFFF0000);
        OH_Drawing_BrushSetBlendMode(brush_, BLEND_MODE_SRC);
        OH_Drawing_BrushSetFilter(brush_, filter_);
        OH_Drawing_BrushSetShaderEffect(brush_, shaderEffect_);
        OH_Drawing_BrushSetShadowLayer(brush_, shadowLayer_);
    };
    OH_Drawing_Pen* pen_ = nullptr;
    OH_Drawing_Brush* brush_ = nullptr;
    OH_Drawing_MaskFilter* maskFilter_ = nullptr;
    OH_Drawing_Point* styleCenter_ = nullptr;
    OH_Drawing_ShadowLayer* shadowLayer_ = nullptr;
    OH_Drawing_ShaderEffect* shaderEffect_ = nullptr;
    OH_Drawing_PathEffect* pathEffect_ = nullptr;
    OH_Drawing_ColorFilter* colorFilter_ = nullptr;
    OH_Drawing_Filter* filter_ = nullptr;
};

#endif // DRAWINGNDKTEST5_0_PERFORMANCE_H
