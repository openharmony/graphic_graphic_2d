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

#ifndef RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_UTILS_H
#define RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_UTILS_H

#include "draw/color.h"

namespace OHOS::Rosen {
namespace RSColorPickerUtils {
    /**
     * @brief Calculate the luminance of a color using standard coefficients.
     *
     * Uses the ITU-R BT.601 standard for luminance calculation:
     * Y = 0.299*R + 0.587*G + 0.114*B
     *
     * @param color The color to calculate luminance for
     * @return float The luminance value in range [0, 255]
     */
    float CalculateLuminance(Drawing::ColorQuad color);

    /**
     * @brief Obtain the corresponding dark color and light color based on the placeholder
     *
     * @param placeholder The placeholder to Get color for
     * @return pair of dark color and light color
     */
    std::pair<Drawing::ColorQuad, Drawing::ColorQuad> GetColorForPlaceholder(ColorPlaceholder placeholder);

    /**
     * @brief Interpolate the start color and end color to obtain the color
     *
     * @param start The start color
     * @param end The end color
     * @param fraction The interpolation ratio
     * @return Interpolated color
     */
    Drawing::ColorQuad InterpolateColor(Drawing::ColorQuad start, Drawing::ColorQuad end, float fraction);
}
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_UTILS_H
