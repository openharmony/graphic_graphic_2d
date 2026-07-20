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

#include "draw/surface.h"
#include "feature/hdr/rs_colorspace_util.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

static constexpr Drawing::CMSTransferFunction TRANSFUNC_2DOT6 = { 2.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
static constexpr Drawing::CMSMatrix3x3 MATRIX_DCI_P3 = {{
    { 0.486143,    0.323835,  0.154234  },
    { 0.226676,    0.710327,  0.0629966 },
    { 0.000800549, 0.0432385, 0.78275   },
}};

std::shared_ptr<Drawing::ColorSpace> RSColorSpaceUtil::ColorSpaceToDrawingColorSpace(
    ColorManager::ColorSpaceName colorSpaceName)
{
    switch (colorSpaceName) {
        case ColorManager::ColorSpaceName::DCI_P3:
            return Drawing::ColorSpace::CreateCustomRGB(TRANSFUNC_2DOT6, MATRIX_DCI_P3);
        case ColorManager::ColorSpaceName::DISPLAY_P3:
            return Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
        case ColorManager::ColorSpaceName::LINEAR_SRGB:
            return Drawing::ColorSpace::CreateSRGBLinear();
        case ColorManager::ColorSpaceName::SRGB:
            return Drawing::ColorSpace::CreateSRGB();
        case ColorManager::ColorSpaceName::DISPLAY_BT2020_SRGB:
        case ColorManager::ColorSpaceName::BT2020:
            return Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::REC2020);
        case ColorManager::ColorSpaceName::ADOBE_RGB:
            return Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::ADOBE_RGB);
        default:
            return Drawing::ColorSpace::CreateSRGB();
    }
}

GraphicColorGamut RSColorSpaceUtil::DrawingColorSpaceToGraphicColorGamut(
    const std::shared_ptr<Drawing::ColorSpace>& colorSpace)
{
    // Function only applicable to the target color space.
    if (colorSpace == nullptr || colorSpace->IsSRGB()) {
        return GRAPHIC_COLOR_GAMUT_SRGB;
    } else if (colorSpace->Equals(Drawing::ColorSpace::CreateRGB(
        Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3))) {
        return GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    } else if (colorSpace->Equals(Drawing::ColorSpace::CreateRGB(
        Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::REC2020))) {
        return GRAPHIC_COLOR_GAMUT_BT2020;
    }
    return GRAPHIC_COLOR_GAMUT_SRGB;
}

GraphicColorGamut RSColorSpaceUtil::GetColorGamutFromCanvas(const Drawing::Canvas& canvas)
{
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        RS_LOGE("GetColorGamutFromCanvas surface is nullptr");
        return GRAPHIC_COLOR_GAMUT_INVALID;
    }
    auto colorSpace = surface->GetImageInfo().GetColorSpace();
    return DrawingColorSpaceToGraphicColorGamut(colorSpace);
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
        {ColorSpaceName::DISPLAY_BT2020_SRGB, GRAPHIC_COLOR_GAMUT_BT2020},
    };
    if (auto itr = RS_COLORSPACE_TO_GRAPHIC_GAMUT_MAP.find(name); itr != RS_COLORSPACE_TO_GRAPHIC_GAMUT_MAP.end()) {
        return itr->second;
    }
    return GraphicColorGamut::GRAPHIC_COLOR_GAMUT_NATIVE;
}

OHOS::ColorManager::ColorSpaceName RSColorSpaceUtil::GraphicGamutToColorSpaceName(GraphicColorGamut gamut)
{
    using OHOS::ColorManager::ColorSpaceName;
    static const std::unordered_map<GraphicColorGamut, ColorSpaceName> RS_GRAPHIC_GAMUT_TO_COLORSPACE_MAP {
        {GRAPHIC_COLOR_GAMUT_STANDARD_BT601, ColorSpaceName::BT601_EBU},
        {GRAPHIC_COLOR_GAMUT_STANDARD_BT709, ColorSpaceName::BT709},
        {GRAPHIC_COLOR_GAMUT_DCI_P3, ColorSpaceName::DCI_P3},
        {GRAPHIC_COLOR_GAMUT_SRGB, ColorSpaceName::SRGB},
        {GRAPHIC_COLOR_GAMUT_ADOBE_RGB, ColorSpaceName::ADOBE_RGB},
        {GRAPHIC_COLOR_GAMUT_DISPLAY_P3, ColorSpaceName::DISPLAY_P3},
        {GRAPHIC_COLOR_GAMUT_BT2020, ColorSpaceName::BT2020},
        {GRAPHIC_COLOR_GAMUT_BT2100_PQ, ColorSpaceName::BT2020_PQ},
        {GRAPHIC_COLOR_GAMUT_BT2100_HLG, ColorSpaceName::BT2020_HLG},
    };
    if (auto itr = RS_GRAPHIC_GAMUT_TO_COLORSPACE_MAP.find(gamut); itr != RS_GRAPHIC_GAMUT_TO_COLORSPACE_MAP.end()) {
        return itr->second;
    }
    return ColorSpaceName::NONE;
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

GraphicColorGamut RSColorSpaceUtil::MapGamutToStandard(GraphicColorGamut gamut)
{
    switch (gamut) {
        case GRAPHIC_COLOR_GAMUT_ADOBE_RGB:
        case GRAPHIC_COLOR_GAMUT_DCI_P3:
        case GRAPHIC_COLOR_GAMUT_DISPLAY_P3:
            return GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
        case GRAPHIC_COLOR_GAMUT_BT2020:
        case GRAPHIC_COLOR_GAMUT_BT2100_PQ:
        case GRAPHIC_COLOR_GAMUT_BT2100_HLG:
        case GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020:
            return GRAPHIC_COLOR_GAMUT_BT2020;
        default:
            return GRAPHIC_COLOR_GAMUT_SRGB;
    }
}

GraphicCM_ColorSpaceType RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName colorSpaceName)
{
    static const std::unordered_map<ApiColorSpaceName, GraphicCM_ColorSpaceType>
        API_COLOR_SPACE_NAME_TO_CM_COLOR_SPACE_TYPE = {
            { ApiColorSpaceName::UNKNOWN, GraphicCM_ColorSpaceType::GRAPHIC_CM_COLORSPACE_NONE },
            { ApiColorSpaceName::ADOBE_RGB_1998, GraphicCM_ColorSpaceType::GRAPHIC_CM_ADOBERGB_FULL },
            { ApiColorSpaceName::DCI_P3, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_FULL },
            { ApiColorSpaceName::DISPLAY_P3, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_FULL },
            { ApiColorSpaceName::SRGB, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL },
            { ApiColorSpaceName::CUSTOM, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL },
            { ApiColorSpaceName::BT709, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT709_FULL },
            { ApiColorSpaceName::BT601_EBU, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_FULL },
            { ApiColorSpaceName::BT601_SMPTE_C, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_SMPTE_C_FULL },
            { ApiColorSpaceName::BT2020_HLG, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_HLG_FULL },
            { ApiColorSpaceName::BT2020_PQ, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_PQ_FULL },
            { ApiColorSpaceName::P3_HLG, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_HLG_FULL },
            { ApiColorSpaceName::P3_PQ, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_PQ_FULL },
            { ApiColorSpaceName::ADOBE_RGB_1998_LIMIT, GraphicCM_ColorSpaceType::GRAPHIC_CM_ADOBERGB_LIMIT },
            { ApiColorSpaceName::DISPLAY_P3_LIMIT, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_LIMIT },
            { ApiColorSpaceName::SRGB_LIMIT, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_LIMIT },
            { ApiColorSpaceName::BT709_LIMIT, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT709_LIMIT },
            { ApiColorSpaceName::BT601_EBU_LIMIT, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_LIMIT },
            { ApiColorSpaceName::BT601_SMPTE_C_LIMIT, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_SMPTE_C_LIMIT },
            { ApiColorSpaceName::BT2020_HLG_LIMIT, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_HLG_LIMIT },
            { ApiColorSpaceName::BT2020_PQ_LIMIT, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_PQ_LIMIT },
            { ApiColorSpaceName::P3_HLG_LIMIT, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_HLG_LIMIT },
            { ApiColorSpaceName::P3_PQ_LIMIT, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_PQ_LIMIT },
            { ApiColorSpaceName::LINEAR_P3, GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_P3 },
            { ApiColorSpaceName::LINEAR_SRGB, GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_SRGB },
            { ApiColorSpaceName::LINEAR_BT2020, GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_BT2020 },
            { ApiColorSpaceName::H_LOG, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL },
            { ApiColorSpaceName::DISPLAY_BT2020_SRGB, GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_BT2020_SRGB },
        };
    auto it = API_COLOR_SPACE_NAME_TO_CM_COLOR_SPACE_TYPE.find(colorSpaceName);
    return it == API_COLOR_SPACE_NAME_TO_CM_COLOR_SPACE_TYPE.end() ? GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL :
                                                                     it->second;
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

bool RSColorSpaceUtil::ConvertColorGamutToSpaceType(
    const GraphicColorGamut& colorGamut, HDIV::CM_ColorSpaceType& colorSpaceType)
{
    static const std::map<GraphicColorGamut, HDIV::CM_ColorSpaceType> COLOR_GAMUT_TO_SPACE_TYPE_MAP {
        { GRAPHIC_COLOR_GAMUT_STANDARD_BT601, HDIV::CM_BT601_EBU_FULL },
        { GRAPHIC_COLOR_GAMUT_STANDARD_BT709, HDIV::CM_BT709_FULL },
        { GRAPHIC_COLOR_GAMUT_SRGB, HDIV::CM_SRGB_FULL },
        { GRAPHIC_COLOR_GAMUT_ADOBE_RGB, HDIV::CM_ADOBERGB_FULL },
        { GRAPHIC_COLOR_GAMUT_DISPLAY_P3, HDIV::CM_P3_FULL },
        { GRAPHIC_COLOR_GAMUT_DCI_P3, HDIV::CM_P3_FULL },
        { GRAPHIC_COLOR_GAMUT_BT2020, HDIV::CM_DISPLAY_BT2020_SRGB },
        { GRAPHIC_COLOR_GAMUT_BT2100_PQ, HDIV::CM_BT2020_PQ_FULL },
        { GRAPHIC_COLOR_GAMUT_BT2100_HLG, HDIV::CM_BT2020_HLG_FULL },
        { GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020, HDIV::CM_DISPLAY_BT2020_SRGB },
    };
 
    auto it = COLOR_GAMUT_TO_SPACE_TYPE_MAP.find(colorGamut);
    if (it != COLOR_GAMUT_TO_SPACE_TYPE_MAP.end()) {
        colorSpaceType = it->second;
        return true;
    }
    return false;
}
#endif
} // namespace Rosen
} // namespace OHOS