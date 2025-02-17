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

#include "cj_color_picker.h"

#include "color_picker.h"
#include "effect_errors.h"
#include "effect_utils.h"
namespace OHOS {
namespace CJEffectKit {
using namespace OHOS::ColorManager;
using namespace OHOS::Media;
using namespace OHOS::Rosen;

const int64_t EFFECTKIT_ERROR = -1;

CJColorPicker::CJColorPicker() {}

int64_t CJColorPicker::CreateColorPicker(Media::PixelMapImpl* source, uint32_t& code)
{
    std::shared_ptr<Media::PixelMap> pixmap = source->GetRealPixelMap();
    ColorPickerCommon::CreateColorPicker(pixmap, code);
    auto native = FFIData::Create<CJColorPicker>();
    if (!native) {
        return EFFECTKIT_ERROR;
    }
    return native->GetID();
}

int64_t CJColorPicker::CreateColorPicker(Media::PixelMapImpl* source, std::vector<double> region, uint32_t& code)
{
    std::shared_ptr<Media::PixelMap> pixmap = source->GetRealPixelMap();
    ColorPickerCommon::CreateColorPicker(pixmap, region, code);
    auto native = FFIData::Create<CJColorPicker>();
    if (!native) {
        return EFFECTKIT_ERROR;
    }
    return native->GetID();
}

uint32_t CJColorPicker::GetMainColor(Color& color)
{
    return ColorPickerCommon::GetMainColor(color);
}

uint32_t CJColorPicker::GetLargestProportionColor(Color& color)
{
    return ColorPickerCommon::GetLargestProportionColor(color);
}

uint32_t CJColorPicker::GetHighestSaturationColor(Color& color)
{
    return ColorPickerCommon::GetHighestSaturationColor(color);
}

uint32_t CJColorPicker::GetAverageColor(Color& color)
{
    return ColorPickerCommon::GetAverageColor(color);
}

bool CJColorPicker::IsBlackOrWhiteOrGrayColor(uint32_t color, uint32_t& code)
{
    return ColorPickerCommon::IsBlackOrWhiteOrGrayColor(color, code);
}

std::vector<ColorManager::Color> CJColorPicker::GetTopProportionColors(double colorCount, uint32_t& code)
{
    return ColorPickerCommon::GetTopProportionColors(colorCount, code);
}

} // namespace CJEffectKit
} // namespace OHOS