/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "drawing_color_filter.h"

#include "utils/log.h"
#include "effect/color_filter.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static ColorFilter* CastToColorFilter(OH_Drawing_ColorFilter* cColorFilter)
{
    return reinterpret_cast<ColorFilter*>(cColorFilter);
}

static ColorFilter& CastToColorFilter(OH_Drawing_ColorFilter& cColorFilter)
{
    return reinterpret_cast<ColorFilter&>(cColorFilter);
}

OH_Drawing_ColorFilter* OH_Drawing_ColorFilterCreateBlendMode(uint32_t color, OH_Drawing_BlendMode cBlendMode)
{
    return (OH_Drawing_ColorFilter*)new ColorFilter(ColorFilter::FilterType::BLEND_MODE, color,
        static_cast<BlendMode>(cBlendMode));
}

OH_Drawing_ColorFilter* OH_Drawing_ColorFilterCreateCompose(OH_Drawing_ColorFilter* colorFilter1,
    OH_Drawing_ColorFilter* colorFilter2)
{
    if (colorFilter1 == nullptr || colorFilter2 == nullptr) {
        return nullptr;
    }
    return (OH_Drawing_ColorFilter*)new ColorFilter(ColorFilter::FilterType::COMPOSE, CastToColorFilter(*colorFilter1),
        CastToColorFilter(*colorFilter2));
}

OH_Drawing_ColorFilter* OH_Drawing_ColorFilterCreateMatrix(const float matrix[20])
{
    if (matrix == nullptr) {
        LOGE("matrix is nullptr");
        return nullptr;
    }
    return (OH_Drawing_ColorFilter*)new ColorFilter(ColorFilter::FilterType::MATRIX, matrix);
}

OH_Drawing_ColorFilter* OH_Drawing_ColorFilterCreateLinearToSrgbGamma()
{
    return (OH_Drawing_ColorFilter*)new ColorFilter(ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
}

OH_Drawing_ColorFilter* OH_Drawing_ColorFilterCreateSrgbGammaToLinear()
{
    return (OH_Drawing_ColorFilter*)new ColorFilter(ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR);
}

OH_Drawing_ColorFilter* OH_Drawing_ColorFilterCreateLuma()
{
    return (OH_Drawing_ColorFilter*)new ColorFilter(ColorFilter::FilterType::LUMA);
}

void OH_Drawing_ColorFilterDestroy(OH_Drawing_ColorFilter* cColorFilter)
{
    delete CastToColorFilter(cColorFilter);
}
