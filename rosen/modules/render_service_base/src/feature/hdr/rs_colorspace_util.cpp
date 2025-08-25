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

#include <unordered_map>

#include "feature/hdr/rs_colorspace_util.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<Drawing::ColorSpace> RSColorSpaceUtil::ColorSpaceToDrawingColorSpace(
    ColorManager::ColorSpaceName colorSpaceName)
{
    switch (colorSpaceName) {
        case ColorManager::ColorSpaceName::DCI_P3:
        case ColorManager::ColorSpaceName::DISPLAY_P3:
            return Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
        case ColorManager::ColorSpaceName::LINEAR_SRGB:
            return Drawing::ColorSpace::CreateSRGBLinear();
        case ColorManager::ColorSpaceName::SRGB:
            return Drawing::ColorSpace::CreateSRGB();
        case ColorManager::ColorSpaceName::DISPLAY_BT2020_SRGB:
            return Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::REC2020);
        case ColorManager::ColorSpaceName::ADOBE_RGB:
            return Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::ADOBE_RGB);
        default:
            return Drawing::ColorSpace::CreateSRGB();
    }
}

GraphicColorGamut RSColorSpaceUtil::ColorSpaceNameToGraphicGamut(OHOS::ColorManager::ColorSpaceName name)
{
    using OHOS::ColorManager::ColorSpaceName;
    static const std::unordered_map<ColorSpaceName, GraphicColorGamut> RS_COLORSPACE_TO_GRAPHIC_GAMUT_MAP {
        {ColorSpaceName::BT601_EBU, GRAPHIC_COLOR_GAMUT_STANDARD_BT601},
        {ColorSpaceName::SMPTE_C, GRAPHIC_COLOR_GAMUT_STANDARD_BT601},
        {ColorSpaceName::BT709, GRAPHIC_COLOR_GAMUT_STANDARD_BT709},
        {ColorSpaceName::DCI_P3, GRAPHIC_COLOR_GAMUT_DCI_P3},
        {ColorSpaceName::SRGB, GRAPHIC_COLOR_GAMUT_SRGB},
        {ColorSpaceName::ADOBE_RGB, GRAPHIC_COLOR_GAMUT_ADOBE_RGB},
        {ColorSpaceName::DISPLAY_P3, GRAPHIC_COLOR_GAMUT_DISPLAY_P3},
        {ColorSpaceName::BT2020, GRAPHIC_COLOR_GAMUT_BT2020},
        {ColorSpaceName::BT2020_PQ, GRAPHIC_COLOR_GAMUT_BT2100_PQ},
        {ColorSpaceName::BT2020_HLG, GRAPHIC_COLOR_GAMUT_BT2100_HLG},
        {ColorSpaceName::DISPLAY_BT2020_SRGB, GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020},
    };
    if (auto itr = RS_COLORSPACE_TO_GRAPHIC_GAMUT_MAP.find(name); itr != RS_COLORSPACE_TO_GRAPHIC_GAMUT_MAP.end()) {
        return itr->second;
    }
    return GraphicColorGamut::GRAPHIC_COLOR_GAMUT_NATIVE;
}

GraphicColorGamut RSColorSpaceUtil::SelectBigGamut(GraphicColorGamut gamut1, GraphicColorGamut gamut2)
{
    // Only Support DISPLAY_BT2020, DISPLAY_P3, SRGB
    if (gamut1 == GRAPHIC_COLOR_GAMUT_BT2020) {
        return gamut1;
    }
    if (gamut1 == GRAPHIC_COLOR_GAMUT_DISPLAY_P3 && gamut2 == GRAPHIC_COLOR_GAMUT_SRGB) {
        return gamut1;
    }
    return gamut2;
}

#ifndef ROSEN_CROSS_PLATFORM
GraphicColorGamut RSColorSpaceUtil::PrimariesToGraphicGamut(HDIV::CM_ColorPrimaries primary)
{
    using namespace HDIV;
    switch (primary) {
        case COLORPRIMARIES_ADOBERGB:
        case COLORPRIMARIES_P3_DCI:
        case COLORPRIMARIES_P3_D65:
            return GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
        case COLORPRIMARIES_BT2020: // COLORPRIMARIES_BT2100 = COLORPRIMARIES_BT2020
            return GRAPHIC_COLOR_GAMUT_BT2020;
        default:
            return GRAPHIC_COLOR_GAMUT_SRGB;
    }
}
#endif
} // namespace Rosen
} // namespace OHOS