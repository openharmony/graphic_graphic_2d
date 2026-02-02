/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "feature/color_picker/rs_color_picker_utils.h"

namespace OHOS::Rosen::RSColorPickerUtils {
namespace {
constexpr float RED_LUMINANCE_COEFF = 0.299f;
constexpr float GREEN_LUMINANCE_COEFF = 0.587f;
constexpr float BLUE_LUMINANCE_COEFF = 0.114f;
} // namespace

float CalculateLuminance(Drawing::ColorQuad color)
{
    auto red = Drawing::Color::ColorQuadGetR(color);
    auto green = Drawing::Color::ColorQuadGetG(color);
    auto blue = Drawing::Color::ColorQuadGetB(color);
    return red * RED_LUMINANCE_COEFF + green * GREEN_LUMINANCE_COEFF + blue * BLUE_LUMINANCE_COEFF;
}

} // namespace OHOS::Rosen::RSColorPickerUtils
