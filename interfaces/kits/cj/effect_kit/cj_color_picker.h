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
#ifndef CJ_COLOR_PICKER_H
#define CJ_COLOR_PICKER_H

#include <vector>

#include "cj_common_ffi.h"
#include "color.h"
#include "color_picker.h"
#include "color_picker_common.h"
#include "pixel_map_impl.h"

namespace OHOS {
namespace CJEffectKit {
class FFI_EXPORT CJColorPicker : public OHOS::FFI::FFIData {
    DECL_TYPE(CJColorPicker, OHOS::FFI::FFIData)
public:
    CJColorPicker();
    static int64_t CreateColorPicker(Media::PixelMapImpl* source, uint32_t& code);
    static int64_t CreateColorPicker(Media::PixelMapImpl* source, std::vector<double> region, uint32_t& code);
    static uint32_t GetMainColor(ColorManager::Color& color);
    static uint32_t GetLargestProportionColor(ColorManager::Color& color);
    static uint32_t GetHighestSaturationColor(ColorManager::Color& color);
    static uint32_t GetAverageColor(ColorManager::Color& color);
    static bool IsBlackOrWhiteOrGrayColor(uint32_t color, uint32_t& code);
    static std::vector<ColorManager::Color> GetTopProportionColors(double colorCount, uint32_t& code);
};
} // namespace CJEffectKit
} // namespace OHOS

#endif /* CJ_COLOR_PICKER_H */