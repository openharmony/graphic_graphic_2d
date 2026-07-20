/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RS_COLORSPACE_UTIL_H
#define RS_COLORSPACE_UTIL_H

#include "color_space.h"

#include "common/rs_macros.h"
#include "draw/canvas.h"
#include "effect/color_space.h"
#include "surface_type.h"
#ifndef ROSEN_CROSS_PLATFORM
#include <v1_0/cm_color_space.h>
#endif

#define HDIV HDI::Display::Graphic::Common::V1_0

namespace OHOS {
namespace Rosen {

enum class ApiColorSpaceName : uint32_t {
    UNKNOWN = 0,
    ADOBE_RGB_1998 = 1,
    DCI_P3 = 2,
    DISPLAY_P3 = 3,
    SRGB = 4,
    BT709 = 6,
    BT601_EBU = 7,
    BT601_SMPTE_C = 8,
    BT2020_HLG = 9,
    BT2020_PQ = 10,
    P3_HLG = 11,
    P3_PQ = 12,
    ADOBE_RGB_1998_LIMIT = 13,
    DISPLAY_P3_LIMIT = 14,
    SRGB_LIMIT = 15,
    BT709_LIMIT = 16,
    BT601_EBU_LIMIT = 17,
    BT601_SMPTE_C_LIMIT = 18,
    BT2020_HLG_LIMIT = 19,
    BT2020_PQ_LIMIT = 20,
    P3_HLG_LIMIT = 21,
    P3_PQ_LIMIT = 22,
    LINEAR_P3 = 23,
    LINEAR_SRGB = 24,
    LINEAR_BT709 = LINEAR_SRGB,
    LINEAR_BT2020 = 25,
    DISPLAY_SRGB = SRGB,
    DISPLAY_P3_SRGB = DISPLAY_P3,
    DISPLAY_P3_HLG = P3_HLG,
    DISPLAY_P3_PQ = P3_PQ,
    CUSTOM = 5,
    H_LOG = 26,
    DISPLAY_BT2020_SRGB = 27,
    TYPE_END = 28
};

class RS_EXPORT RSColorSpaceUtil {
public:
    RSColorSpaceUtil() = default;
    ~RSColorSpaceUtil() = default;

    static std::shared_ptr<Drawing::ColorSpace> ColorSpaceToDrawingColorSpace(
        ColorManager::ColorSpaceName colorSpaceName);
    static GraphicColorGamut DrawingColorSpaceToGraphicColorGamut(
        const std::shared_ptr<Drawing::ColorSpace>& colorSpace);
    static GraphicColorGamut GetColorGamutFromCanvas(const Drawing::Canvas& canvas);
    static GraphicColorGamut ColorSpaceNameToGraphicGamut(OHOS::ColorManager::ColorSpaceName name);
    static ColorManager::ColorSpaceName GraphicGamutToColorSpaceName(GraphicColorGamut gamut);
    static GraphicColorGamut SelectBigGamut(GraphicColorGamut gamut1, GraphicColorGamut gamut2);
    static GraphicColorGamut MapGamutToStandard(GraphicColorGamut gamut);
    static GraphicCM_ColorSpaceType ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName colorSpaceName);

#ifndef ROSEN_CROSS_PLATFORM
    static GraphicColorGamut PrimariesToGraphicGamut(HDIV::CM_ColorPrimaries primary);

    static bool ConvertColorGamutToSpaceType(
        const GraphicColorGamut& colorGamut, HDIV::CM_ColorSpaceType& colorSpaceType);
#endif
};

} // namespace Rosen
} // namespace OHOS
#endif // RS_COLORSPACE_UTIL_H