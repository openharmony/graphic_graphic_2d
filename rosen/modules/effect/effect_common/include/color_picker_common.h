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
#ifndef COLOR_PICKER_COMMON_H
#define COLOR_PICKER_COMMON_H

#include <vector>

#include "color.h"
#include "color_picker.h"

namespace OHOS {
namespace Rosen {
class ColorPickerCommon {
public:
    static std::shared_ptr<Rosen::ColorPickerCommon> CreateColorPicker(
        const std::shared_ptr<Media::PixelMap>& pixmap, uint32_t& errorCode);
    static std::shared_ptr<Rosen::ColorPickerCommon> CreateColorPicker(
        const std::shared_ptr<Media::PixelMap>& pixmap, std::vector<double> region, uint32_t& errorCode);
    static uint32_t GetMainColor(ColorManager::Color& color);
    static uint32_t GetLargestProportionColor(ColorManager::Color& color);
    static uint32_t GetHighestSaturationColor(ColorManager::Color& color);
    static uint32_t GetAverageColor(ColorManager::Color& color);
    static bool IsBlackOrWhiteOrGrayColor(uint32_t color, uint32_t& errorCode);
    static std::vector<ColorManager::Color> GetTopProportionColors(double colorCount, uint32_t& errorCode);
    std::shared_ptr<ColorPicker> nativeColorPicker_;
    std::shared_ptr<ColorPicker> sColorPicker_;
    static thread_local std::shared_ptr<ColorPickerCommon> sConstructor_;
};
} // namespace Rosen
} // namespace OHOS

#endif /* COLOR_PICKER_COMMON_H */