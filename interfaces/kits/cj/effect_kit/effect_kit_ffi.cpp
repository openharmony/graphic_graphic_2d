/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "effect_kit_ffi.h"

#include "cj_color_picker.h"
#include "cj_filter.h"
#include "effect_errors.h"
#include "effect_utils.h"
#include "media_errors.h"
#include "pixel_map_impl.h"

namespace OHOS {
namespace CJEffectKit {
using namespace OHOS::FFI;
using namespace OHOS::Media;
using namespace OHOS::Rosen;

const int64_t EFFECTKIT_ERROR = -1;

void BuildCJColor(ColorManager::Color color, CJColor& cjColor)
{
    cjColor.r = static_cast<int>(color.r * 255.0f);
    cjColor.g = static_cast<int>(color.g * 255.0f);
    cjColor.b = static_cast<int>(color.b * 255.0f);
    cjColor.a = static_cast<int>(color.a * 255.0f);
}

extern "C" {
int64_t FfiEffectKitCreateEffect(int64_t sourceId, uint32_t* errorCode)
{
    auto instance = FFIData::GetData<PixelMapImpl>(sourceId);
    if (!instance) {
        EFFECT_LOG_E("[PixelMap] instance not exist %{public}" PRId64, sourceId);
        return EFFECTKIT_ERROR;
    }
    return CJFilter::CreateEffect(instance, *errorCode);
}

void FfiEffectKitBlur(int64_t id, float radius)
{
    auto instance = FFIData::GetData<CJFilter>(id);
    if (!instance) {
        EFFECT_LOG_E("[CJFilter] instance not exist %{public}" PRId64, id);
        return;
    }
    instance->Blur(radius);
    return;
}

void FfiEffectKitInvert(int64_t id)
{
    auto instance = FFIData::GetData<CJFilter>(id);
    if (!instance) {
        EFFECT_LOG_E("[CJFilter] instance not exist %{public}" PRId64, id);
        return;
    }
    instance->Invert();
    return;
}

void FfiEffectKitBrightness(int64_t id, float bright)
{
    auto instance = FFIData::GetData<CJFilter>(id);
    if (!instance) {
        EFFECT_LOG_E("[CJFilter] instance not exist %{public}" PRId64, id);
        return;
    }
    instance->Brightness(bright);
    return;
}

void FfiEffectKitGrayscale(int64_t id)
{
    auto instance = FFIData::GetData<CJFilter>(id);
    if (!instance) {
        EFFECT_LOG_E("[CJFilter] instance not exist %{public}" PRId64, id);
        return;
    }
    instance->Grayscale();
    return;
}

void FfiEffectKitSetColorMatrix(int64_t id, CArrFloat cjColorMatrix, uint32_t* errorCode)
{
    std::vector<float> naColor;
    for (int i = 0; i < cjColorMatrix.size; i++) {
        naColor.push_back(cjColorMatrix.head[i]);
    }
    auto instance = FFIData::GetData<CJFilter>(id);
    if (!instance) {
        EFFECT_LOG_E("[CJFilter] instance not exist %{public}" PRId64, id);
        return;
    }
    instance->SetColorMatrix(naColor, *errorCode);
    return;
}

int64_t FfiEffectKitGetEffectPixelMap()
{
    std::shared_ptr<OHOS::Media::PixelMap> map = CJFilter::GetEffectPixelMap();
    if (map == nullptr) {
        return EFFECTKIT_ERROR;
    }
    auto native = FFIData::Create<PixelMapImpl>(map);
    if (!native) {
        return EFFECTKIT_ERROR;
    }
    return native->GetID();
}

int64_t FfiEffectKitCreateColorPicker(int64_t sourceId, uint32_t* errorCode)
{
    auto instance = FFIData::GetData<PixelMapImpl>(sourceId);
    if (!instance) {
        EFFECT_LOG_E("[PixelMap] instance not exist %{public}" PRId64, sourceId);
        *errorCode = static_cast<int32_t>(Rosen::ERR_INVALID_PARAM);
        return EFFECTKIT_ERROR;
    }
    return CJColorPicker::CreateColorPicker(instance, *errorCode);
}

int64_t FfiEffectKitCreateColorPickerRegion(int64_t sourceId, CArrDouble region, uint32_t* errorCode)
{
    auto instance = FFIData::GetData<PixelMapImpl>(sourceId);
    if (!instance) {
        EFFECT_LOG_E("[PixelMap] instance not exist %{public}" PRId64, sourceId);
        *errorCode = static_cast<int32_t>(Rosen::ERR_INVALID_PARAM);
        return EFFECTKIT_ERROR;
    }
    std::vector<double> cRegion;
    for (int i = 0; i < region.size; i++) {
        cRegion.push_back(region.head[i]);
    }
    return CJColorPicker::CreateColorPicker(instance, cRegion, *errorCode);
}

CJColor FfiEffectKitGetMainColor(uint32_t* errorCode)
{
    ColorManager::Color color;
    *errorCode = CJColorPicker::GetMainColor(color);
    CJColor cjColor = { 0, 0, 0, 0 };
    BuildCJColor(color, cjColor);
    return cjColor;
}

CJColor FfiEffectKitGetLargestProportionColor(uint32_t* errorCode)
{
    ColorManager::Color color;
    *errorCode = CJColorPicker::GetLargestProportionColor(color);
    CJColor cjColor = { 0, 0, 0, 0 };
    BuildCJColor(color, cjColor);
    return cjColor;
}

CJColor FfiEffectKitGetHighestSaturationColor(uint32_t* errorCode)
{
    ColorManager::Color color;
    *errorCode = CJColorPicker::GetHighestSaturationColor(color);
    CJColor cjColor = { 0, 0, 0, 0 };
    BuildCJColor(color, cjColor);
    return cjColor;
}

CJColor FfiEffectKitGetAverageColor(uint32_t* errorCode)
{
    ColorManager::Color color;
    *errorCode = CJColorPicker::GetAverageColor(color);
    CJColor cjColor = { 0, 0, 0, 0 };
    BuildCJColor(color, cjColor);
    return cjColor;
}

bool FfiEffectKitIsBlackOrWhiteOrGrayColor(uint32_t color, uint32_t* errorCode)
{
    return CJColorPicker::IsBlackOrWhiteOrGrayColor(color, *errorCode);
}

CArrCJColor FfiEffectKitGetTopProportionColors(double colorCount, uint32_t* errorCode)
{
    std::vector<ColorManager::Color> colors = CJColorPicker::GetTopProportionColors(colorCount, *errorCode);
    CArrCJColor data = { .head = nullptr, .size = 0 };
    data.size = static_cast<int64_t>(colors.size());
    if (data.size == 0) {
        return data;
    }
    CJColor* retValue = static_cast<CJColor*>(malloc(sizeof(CJColor) * data.size));
    if (retValue == nullptr) {
        EFFECT_LOG_E("malloc failed");
        return data;
    }
    for (int i = 0; i < data.size; i++) {
        CJColor cjColor = { 0, 0, 0, 0 };
        BuildCJColor(colors[i], cjColor);
        retValue[i] = cjColor;
    }
    data.head = retValue;
    return data;
}
}
} // namespace CJEffectKit
} // namespace OHOS