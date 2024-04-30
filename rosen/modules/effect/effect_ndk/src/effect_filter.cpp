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

#include "effect_filter.h"

#include "filter/filter.h"
#include "sk_image_chain.h"
#include "sk_image_filter_factory.h"

#include "utils/log.h"

using namespace OHOS;
using namespace Rosen;

static Filter* CastToFilter(OH_Filter* filter)
{
    return reinterpret_cast<Filter*>(filter);
}

static OH_Filter* CastToOH_Filter(Filter* filter)
{
    return reinterpret_cast<OH_Filter*>(filter);
}

EffectErrorCode OH_Filter_CreateEffect(OH_PixelmapNative* pixelmap, OH_Filter** filter)
{
    if (!pixelmap || !filter) {
        return EFFECT_BAD_PARAMETER;
    }
    *filter = CastToOH_Filter(new Filter(pixelmap->GetInnerPixelmap()));
    if (*filter == nullptr) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_Release(OH_Filter* filter)
{
    if (filter == nullptr) {
        return EFFECT_BAD_PARAMETER;
    }
    delete CastToFilter(filter);
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_Blur(OH_Filter* filter, float radius)
{
    if (!filter || !(CastToFilter(filter)->Blur(radius))) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}
EffectErrorCode OH_Filter_Brighten(OH_Filter* filter, float brightness)
{
    if (!filter || !(CastToFilter(filter)->Brightness(brightness))) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_GrayScale(OH_Filter* filter)
{
    if (!filter || !(CastToFilter(filter)->Grayscale())) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}
EffectErrorCode OH_Filter_Invert(OH_Filter* filter)
{
    if (!filter || !(CastToFilter(filter)->Invert())) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}
EffectErrorCode OH_Filter_SetColorMatrix(OH_Filter* filter, OH_Filter_ColorMatrix* matrix)
{
    if (!filter || !matrix) {
        return EFFECT_BAD_PARAMETER;
    }
    PixelColorMatrix colorMatrix;
    for (size_t i = 0; i < colorMatrix.MATRIX_SIZE; i++) {
        colorMatrix.val[i] = matrix->val[i];
    }
    if (!(CastToFilter(filter)->SetColorMatrix(colorMatrix))) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_GetEffectPixelMap(OH_Filter* filter, OH_PixelmapNative** pixelmap)
{
    if (!pixelmap || !filter) {
        return EFFECT_BAD_PARAMETER;
    }
    *pixelmap = new OH_PixelmapNative(CastToFilter(filter)->GetPixelMap());
    if (pixelmap == nullptr) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}