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

#include "common/rs_common_def.h"

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

std::pair<Drawing::ColorQuad, Drawing::ColorQuad> GetColorForPlaceholder(ColorPlaceholder placeholder)
{
    static const std::unordered_map<ColorPlaceholder, std::pair<Drawing::ColorQuad, Drawing::ColorQuad>>
        PLACEHOLDER_TO_COLOR {
#define COLOR_PLACEHOLDER_ENTRY(name, dark, light) \
            { ColorPlaceholder::name, { dark, light } },
#include "feature/color_picker/rs_color_placeholder_mapping_def.in"
#undef COLOR_PLACEHOLDER_ENTRY
        };
    if (auto it = PLACEHOLDER_TO_COLOR.find(placeholder); it != PLACEHOLDER_TO_COLOR.end()) {
        return it->second;
    }
    return std::make_pair(Drawing::Color::COLOR_BLACK, Drawing::Color::COLOR_WHITE);
}

Drawing::ColorQuad InterpolateColor(Drawing::ColorQuad start, Drawing::ColorQuad end, float fraction)
{
    if (ROSEN_NE(std::clamp(fraction, 0.f, 1.f), fraction)) {
        return end;
    }
    auto lerp = [](uint32_t a, uint32_t b, float t) {
        return static_cast<uint32_t>(static_cast<float>(a) + (static_cast<float>(b) - static_cast<float>(a)) * t);
    };
    auto a =
        static_cast<uint32_t>(lerp(Drawing::Color::ColorQuadGetA(start), Drawing::Color::ColorQuadGetA(end), fraction));
    auto r =
        static_cast<uint32_t>(lerp(Drawing::Color::ColorQuadGetR(start), Drawing::Color::ColorQuadGetR(end), fraction));
    auto g =
        static_cast<uint32_t>(lerp(Drawing::Color::ColorQuadGetG(start), Drawing::Color::ColorQuadGetG(end), fraction));
    auto b =
        static_cast<uint32_t>(lerp(Drawing::Color::ColorQuadGetB(start), Drawing::Color::ColorQuadGetB(end), fraction));
    return Drawing::Color::ColorQuadSetARGB(a, r, g, b);
}

} // namespace OHOS::Rosen::RSColorPickerUtils
