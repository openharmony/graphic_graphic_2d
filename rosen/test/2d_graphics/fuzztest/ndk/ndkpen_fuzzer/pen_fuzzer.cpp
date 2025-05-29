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

#include "pen_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_filter.h"
#include "drawing_matrix.h"
#include "drawing_path.h"
#include "drawing_path_effect.h"
#include "drawing_pen.h"
#include "drawing_rect.h"
#include "drawing_shader_effect.h"
#include "drawing_shadow_layer.h"
#include "drawing_types.h"
#include "draw/brush.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
    constexpr uint32_t MAX_ARRAY_SIZE = 5000;
    constexpr uint32_t PATH_CONST = 2;
    constexpr uint32_t PATH_THREE = 3;
    constexpr uint32_t PATH_TWENTY_NINE = 29;
} // namespace

void PenFuzzTest000(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t aa = GetObject<uint32_t>();
    uint32_t color = GetObject<uint32_t>();
    uint8_t alpha = GetObject<uint8_t>();
    float width = GetObject<float>();
    float miter = GetObject<float>();

    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();

    OH_Drawing_PenSetAntiAlias(nullptr, static_cast<bool>(aa % PATH_CONST));
    OH_Drawing_PenSetAntiAlias(pen, static_cast<bool>(aa % PATH_CONST));
    OH_Drawing_PenIsAntiAlias(nullptr);
    OH_Drawing_PenIsAntiAlias(pen);

    OH_Drawing_PenSetColor(nullptr, color);
    OH_Drawing_PenSetColor(pen, color);
    OH_Drawing_PenGetColor(nullptr);
    OH_Drawing_PenGetColor(pen);

    OH_Drawing_PenSetAlpha(nullptr, alpha);
    OH_Drawing_PenSetAlpha(pen, alpha);
    OH_Drawing_PenGetAlpha(nullptr);
    OH_Drawing_PenGetAlpha(pen);

    OH_Drawing_PenSetWidth(nullptr, width);
    OH_Drawing_PenSetWidth(pen, width);
    OH_Drawing_PenGetWidth(nullptr);
    OH_Drawing_PenGetWidth(pen);

    OH_Drawing_PenSetMiterLimit(nullptr, miter);
    OH_Drawing_PenSetMiterLimit(pen, miter);
    OH_Drawing_PenGetMiterLimit(nullptr);
    OH_Drawing_PenGetMiterLimit(pen);

    OH_Drawing_PenReset(nullptr);
    OH_Drawing_PenReset(pen);

    OH_Drawing_PenDestroy(pen);
}

void PenFuzzTest001(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t enum_1 = GetObject<uint32_t>();
    uint32_t color = GetObject<uint32_t>();

    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();

    OH_Drawing_PenSetCap(nullptr, static_cast<OH_Drawing_PenLineCapStyle>(enum_1 % PATH_THREE));
    OH_Drawing_PenSetCap(pen, static_cast<OH_Drawing_PenLineCapStyle>(enum_1 % PATH_THREE));
    OH_Drawing_PenGetCap(nullptr);
    OH_Drawing_PenGetCap(pen);

    OH_Drawing_PenSetJoin(nullptr, static_cast<OH_Drawing_PenLineJoinStyle>(enum_1 % PATH_THREE));
    OH_Drawing_PenSetJoin(pen, static_cast<OH_Drawing_PenLineJoinStyle>(enum_1 % PATH_THREE));
    OH_Drawing_PenGetJoin(nullptr);
    OH_Drawing_PenGetJoin(pen);

    OH_Drawing_ShaderEffect* cShaderEffect = OH_Drawing_ShaderEffectCreateColorShader(color);
    OH_Drawing_PenSetShaderEffect(nullptr, cShaderEffect);
    OH_Drawing_PenSetShaderEffect(pen, nullptr);
    OH_Drawing_PenSetShaderEffect(pen, cShaderEffect);
        
    OH_Drawing_ShaderEffectDestroy(cShaderEffect);
    OH_Drawing_PenDestroy(pen);
}

void PenFuzzTest002(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    float blurRadius = GetObject<float>();
    float x = GetObject<float>();
    float y = GetObject<float>();
    uint32_t color = GetObject<uint32_t>();
    uint32_t cBlendMode = GetObject<uint32_t>();
    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();

    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_ShadowLayer* cShadowlayer = OH_Drawing_ShadowLayerCreate(blurRadius, x, y, color);
    OH_Drawing_PenSetShadowLayer(nullptr, cShadowlayer);
    OH_Drawing_PenSetShadowLayer(pen, nullptr);
    OH_Drawing_PenSetShadowLayer(pen, cShadowlayer);

    OH_Drawing_Filter* cFilter = OH_Drawing_FilterCreate();
    OH_Drawing_PenSetFilter(nullptr, cFilter);
    OH_Drawing_PenSetFilter(pen, nullptr);
    OH_Drawing_PenSetFilter(pen, cFilter);
    OH_Drawing_PenGetFilter(nullptr, cFilter);
    OH_Drawing_PenGetFilter(pen, nullptr);
    OH_Drawing_PenGetFilter(pen, cFilter);

    OH_Drawing_PenSetBlendMode(nullptr,  static_cast<OH_Drawing_BlendMode>(cBlendMode % PATH_TWENTY_NINE));
    OH_Drawing_PenSetBlendMode(pen,  static_cast<OH_Drawing_BlendMode>(cBlendMode % PATH_TWENTY_NINE));

    OH_Drawing_Path* src = OH_Drawing_PathCreate();
    OH_Drawing_Path* dst = OH_Drawing_PathCreate();
    OH_Drawing_Rect* cRect = OH_Drawing_RectCreate(left, top, right, bottom);
    OH_Drawing_Matrix* cMatrix = OH_Drawing_MatrixCreate();
    OH_Drawing_PenGetFillPath(nullptr, src, dst, cRect, cMatrix);
    OH_Drawing_PenGetFillPath(pen, nullptr, dst, cRect, cMatrix);
    OH_Drawing_PenGetFillPath(pen, src, nullptr, cRect, cMatrix);
    OH_Drawing_PenGetFillPath(pen, src, dst, cRect, cMatrix);

    OH_Drawing_RectDestroy(cRect);
    OH_Drawing_MatrixDestroy(cMatrix);
    OH_Drawing_PathDestroy(src);
    OH_Drawing_PathDestroy(dst);
    OH_Drawing_FilterDestroy(cFilter);
    OH_Drawing_ShadowLayerDestroy(cShadowlayer);
    OH_Drawing_PenDestroy(pen);
}

void PenFuzzTest003(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    float phase = GetObject<float>();

    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Pen* pen1 = OH_Drawing_PenCopy(nullptr);
    pen1 = OH_Drawing_PenCopy(pen);

    uint32_t size_temp = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    uint32_t size_path = size_temp - (size_temp % PATH_CONST) + PATH_CONST;
    float* intervals = new float[size_path];
    for (size_t i = 0; i < size_path; i++) {
        intervals[i] = GetObject<float>();
    }
    OH_Drawing_PathEffect* PathEffect = OH_Drawing_CreateDashPathEffect(intervals, size_path, phase);
    OH_Drawing_PenSetPathEffect(nullptr, PathEffect);
    OH_Drawing_PenSetPathEffect(pen, nullptr);
    OH_Drawing_PenSetPathEffect(pen, PathEffect);
    if (intervals != nullptr) {
        delete[] intervals;
        intervals = nullptr;
    }
    OH_Drawing_PathEffectDestroy(PathEffect);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PenDestroy(pen1);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::PenFuzzTest000(data, size);
    OHOS::Rosen::Drawing::PenFuzzTest001(data, size);
    OHOS::Rosen::Drawing::PenFuzzTest002(data, size);
    OHOS::Rosen::Drawing::PenFuzzTest003(data, size);
    return 0;
}
