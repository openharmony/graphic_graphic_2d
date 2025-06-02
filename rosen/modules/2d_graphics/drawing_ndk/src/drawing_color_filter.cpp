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

#include "drawing_canvas_utils.h"
#include "drawing_helper.h"

#include "utils/log.h"
#include "effect/color_filter.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

OH_Drawing_ColorFilter* OH_Drawing_ColorFilterCreateBlendMode(uint32_t color, OH_Drawing_BlendMode cBlendMode)
{
    NativeHandle<ColorFilter>* colorFilterHandle = new NativeHandle<ColorFilter>;
    colorFilterHandle->value = ColorFilter::CreateBlendModeColorFilter(color, static_cast<BlendMode>(cBlendMode));
    if (colorFilterHandle->value == nullptr) {
        delete colorFilterHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<ColorFilter>*, OH_Drawing_ColorFilter*>(colorFilterHandle);
}

OH_Drawing_ColorFilter* OH_Drawing_ColorFilterCreateCompose(OH_Drawing_ColorFilter* colorFilter1,
    OH_Drawing_ColorFilter* colorFilter2)
{
    if (colorFilter1 == nullptr || colorFilter2 == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    NativeHandle<ColorFilter>* colorFilterHandle1 = Helper::CastTo<OH_Drawing_ColorFilter*,
        NativeHandle<ColorFilter>*>(colorFilter1);
    NativeHandle<ColorFilter>* colorFilterHandle2 = Helper::CastTo<OH_Drawing_ColorFilter*,
        NativeHandle<ColorFilter>*>(colorFilter2);
    if (colorFilterHandle1->value == nullptr || colorFilterHandle2->value == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    NativeHandle<ColorFilter>* colorFilterHandle = new NativeHandle<ColorFilter>;
    colorFilterHandle->value = ColorFilter::CreateComposeColorFilter(*colorFilterHandle1->value,
        *colorFilterHandle2->value);
    if (colorFilterHandle->value == nullptr) {
        delete colorFilterHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<ColorFilter>*, OH_Drawing_ColorFilter*>(colorFilterHandle);
}

OH_Drawing_ColorFilter* OH_Drawing_ColorFilterCreateMatrix(const float matrix[20])
{
    if (matrix == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    const float(&arrayRef)[20] = *reinterpret_cast<const float(*)[20]>(matrix);
    NativeHandle<ColorFilter>* colorFilterHandle = new NativeHandle<ColorFilter>;
    colorFilterHandle->value = ColorFilter::CreateFloatColorFilter(arrayRef);
    if (colorFilterHandle->value == nullptr) {
        delete colorFilterHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<ColorFilter>*, OH_Drawing_ColorFilter*>(colorFilterHandle);
}

OH_Drawing_ColorFilter* OH_Drawing_ColorFilterCreateLinearToSrgbGamma()
{
    NativeHandle<ColorFilter>* colorFilterHandle = new NativeHandle<ColorFilter>;
    colorFilterHandle->value = ColorFilter::CreateLinearToSrgbGamma();
    if (colorFilterHandle->value == nullptr) {
        delete colorFilterHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<ColorFilter>*, OH_Drawing_ColorFilter*>(colorFilterHandle);
}

OH_Drawing_ColorFilter* OH_Drawing_ColorFilterCreateSrgbGammaToLinear()
{
    NativeHandle<ColorFilter>* colorFilterHandle = new NativeHandle<ColorFilter>;
    colorFilterHandle->value = ColorFilter::CreateSrgbGammaToLinear();
    if (colorFilterHandle->value == nullptr) {
        delete colorFilterHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<ColorFilter>*, OH_Drawing_ColorFilter*>(colorFilterHandle);
}

OH_Drawing_ColorFilter* OH_Drawing_ColorFilterCreateLuma()
{
    NativeHandle<ColorFilter>* colorFilterHandle = new NativeHandle<ColorFilter>;
    colorFilterHandle->value = ColorFilter::CreateLumaColorFilter();
    if (colorFilterHandle->value == nullptr) {
        delete colorFilterHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<ColorFilter>*, OH_Drawing_ColorFilter*>(colorFilterHandle);
}

OH_Drawing_ColorFilter* OH_Drawing_ColorFilterCreateLighting(uint32_t mulColor, uint32_t addColor)
{
    NativeHandle<ColorFilter>* colorFilterHandle = new NativeHandle<ColorFilter>;
    colorFilterHandle->value = ColorFilter::CreateLightingColorFilter(mulColor, addColor);
    if (colorFilterHandle->value == nullptr) {
        delete colorFilterHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<ColorFilter>*, OH_Drawing_ColorFilter*>(colorFilterHandle);
}

void OH_Drawing_ColorFilterDestroy(OH_Drawing_ColorFilter* cColorFilter)
{
    if (!cColorFilter) {
        return;
    }
    delete Helper::CastTo<OH_Drawing_ColorFilter*, NativeHandle<ColorFilter>*>(cColorFilter);
}
