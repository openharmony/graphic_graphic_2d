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

#include "brush_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"

#include "drawing_brush.h"
#include "drawing_filter.h"
#include "drawing_shader_effect.h"
#include "drawing_shadow_layer.h"
#include "drawing_types.h"
#include "draw/brush.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
    constexpr uint32_t PATH_TWO = 2;
    constexpr uint32_t PATH_TWENTY_NINE = 29;
} // namespace

void BrushFuzzTest000(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t aa = GetObject<uint32_t>();
    uint32_t color = GetObject<uint32_t>();
    uint8_t alpha = GetObject<uint8_t>();

    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Brush* brush1 = OH_Drawing_BrushCopy(nullptr);
    brush1 = OH_Drawing_BrushCopy(brush);

    OH_Drawing_BrushSetAntiAlias(nullptr, static_cast<bool>(aa % PATH_TWO));
    OH_Drawing_BrushSetAntiAlias(brush, static_cast<bool>(aa % PATH_TWO));
    OH_Drawing_BrushIsAntiAlias(nullptr);
    OH_Drawing_BrushIsAntiAlias(brush);
    OH_Drawing_BrushSetColor(nullptr, color);
    OH_Drawing_BrushSetColor(brush, color);
    OH_Drawing_BrushGetColor(nullptr);
    OH_Drawing_BrushGetColor(brush);
    OH_Drawing_BrushSetAlpha(nullptr, alpha);
    OH_Drawing_BrushSetAlpha(brush, alpha);
    OH_Drawing_BrushGetAlpha(nullptr);
    OH_Drawing_BrushGetAlpha(brush);

    OH_Drawing_Filter* Filter = OH_Drawing_FilterCreate();
    OH_Drawing_BrushSetFilter(nullptr, Filter);
    OH_Drawing_BrushSetFilter(brush, nullptr);
    OH_Drawing_BrushSetFilter(brush, Filter);
    OH_Drawing_BrushGetFilter(nullptr, Filter);
    OH_Drawing_BrushGetFilter(brush, nullptr);
    OH_Drawing_BrushGetFilter(brush, Filter);
    OH_Drawing_BrushReset(nullptr);
    OH_Drawing_BrushReset(brush);

    OH_Drawing_FilterDestroy(Filter);
    OH_Drawing_BrushDestroy(brush1);
    OH_Drawing_BrushDestroy(brush);
}

void BrushFuzzTest001(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t color = GetObject<uint32_t>();
    float blurRadius = GetObject<float>();
    float x = GetObject<float>();
    float y = GetObject<float>();
    uint32_t enum_1 = GetObject<uint32_t>();

    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();

    OH_Drawing_ShaderEffect*  ShaderEffect = OH_Drawing_ShaderEffectCreateColorShader(color);
    OH_Drawing_BrushSetShaderEffect(nullptr, ShaderEffect);
    OH_Drawing_BrushSetShaderEffect(brush, nullptr);
    OH_Drawing_BrushSetShaderEffect(brush, ShaderEffect);

    OH_Drawing_ShadowLayer* ShadowLayer = OH_Drawing_ShadowLayerCreate(blurRadius, x, y, color);
    OH_Drawing_BrushSetShadowLayer(nullptr, ShadowLayer);
    OH_Drawing_BrushSetShadowLayer(brush, nullptr);
    OH_Drawing_BrushSetShadowLayer(brush, ShadowLayer);

    OH_Drawing_BrushSetBlendMode(nullptr, static_cast<OH_Drawing_BlendMode>(enum_1 % PATH_TWENTY_NINE));
    OH_Drawing_BrushSetBlendMode(brush, static_cast<OH_Drawing_BlendMode>(enum_1 % PATH_TWENTY_NINE));
     
    OH_Drawing_ShadowLayerDestroy(ShadowLayer);
    OH_Drawing_ShaderEffectDestroy(ShaderEffect);
    OH_Drawing_BrushDestroy(brush);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::BrushFuzzTest000(data, size);
    OHOS::Rosen::Drawing::BrushFuzzTest001(data, size);
    return 0;
}