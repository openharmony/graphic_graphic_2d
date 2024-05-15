/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "render/rs_colorspace_convert.h"

#include "effect/image_filter.h"
#include "luminance/rs_luminance_control.h"
#include "metadata_helper.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSColorSpaceConvert::RSColorSpaceConvert()
{
    colorSpaceConverterDisplay_ = VPEConvert::Create();
}

RSColorSpaceConvert::~RSColorSpaceConvert()
{}

RSColorSpaceConvert RSColorSpaceConvert::Instance()
{
    static RSColorSpaceConvert instance;
    return instance;
}

bool RSColorSpaceConvert::ColorSpaceConvertor(std::shared_ptr<Drawing::ShaderEffect> inputShader,
    const sptr<SurfaceBuffer>& surfaceBuffer, Drawing::Paint& paint, GraphicColorGamut targetColorSpace,
    ScreenId screenId, uint32_t dynamicRangeMode)
{
    RS_LOGD("RSColorSpaceConvertor targetColorSpace:%{public}d. screenId:%{public}lu. dynamicRangeMode%{public}d",
        targetColorSpace, screenId, dynamicRangeMode);
    VPEParameter parameter;

    if (inputShader == nullptr) {
        RS_LOGE("bhdr imageShader is nullptr.");
        return false;
    }

    if (!SetColorSpaceConverterDisplayParameter(surfaceBuffer, parameter, targetColorSpace, screenId,
        dynamicRangeMode)) {
        return false;
    }

    std::shared_ptr<Drawing::ShaderEffect> outputShader;
    auto convRet = colorSpaceConverterDisplay_->Process(inputShader, outputShader, parameter);
    if (convRet != Media::VideoProcessingEngine::VPE_ALGO_ERR_OK) {
        RS_LOGE("bhdr failed with %{public}u.", convRet);
        return false;
    }
    if (outputShader == nullptr) {
        RS_LOGE("bhdr outputShader is nullptr.");
        return false;
    }
    paint.SetShaderEffect(outputShader);
    return true;
}

bool RSColorSpaceConvert::SetColorSpaceConverterDisplayParameter(const sptr<SurfaceBuffer>& surfaceBuffer,
    VPEParameter& parameter, GraphicColorGamut targetColorSpace, ScreenId screenId, uint32_t dynamicRangeMode)
{
    using namespace HDIV;

    GSError ret = MetadataHelper::GetColorSpaceInfo(surfaceBuffer, parameter.inputColorSpace.colorSpaceInfo);
    if (ret != GSERROR_OK) {
        RS_LOGE("bhdr GetColorSpaceInfo failed with %{public}u.", ret);
        return false;
    }
    if (!ConvertColorGamutToSpaceInfo(targetColorSpace, parameter.outputColorSpace.colorSpaceInfo)) {
        return false;
    }
    CM_HDR_Metadata_Type hdrMetadataType = CM_METADATA_NONE;
    ret = MetadataHelper::GetHDRMetadataType(surfaceBuffer, hdrMetadataType);
    if (ret != GSERROR_OK) {
        RS_LOGD("bhdr GetHDRMetadataType failed with %{public}u.", ret);
    }
    parameter.inputColorSpace.metadataType = hdrMetadataType;
    parameter.outputColorSpace.metadataType = hdrMetadataType;

    ret = MetadataHelper::GetHDRStaticMetadata(surfaceBuffer, parameter.staticMetadata);
    if (ret != GSERROR_OK) {
        RS_LOGE("bhdr GetHDRStaticMetadata failed with %{public}u.", ret);
    }
    ret = MetadataHelper::GetHDRDynamicMetadata(surfaceBuffer, parameter.dynamicMetadata);
    if (ret != GSERROR_OK) {
        RS_LOGE("bhdr GetHDRDynamicMetadata failed with %{public}u.", ret);
    }

    // Set brightness to screen brightness when HDR Vivid, otherwise 500 nits
    parameter.tmoNits = RSLuminanceControl::Get().GetHdrTmoNits(screenId, dynamicRangeMode);
    parameter.currentDisplayNits = RSLuminanceControl::Get().GetHdrDisplayNits(screenId);
    RS_LOGD("bhdr TmoNits:%{public}f. DisplayNits:%{public}f.", parameter.tmoNits, parameter.currentDisplayNits);
    return true;
}

bool RSColorSpaceConvert::ConvertColorGamutToSpaceInfo(const GraphicColorGamut& colorGamut,
    HDIV::CM_ColorSpaceInfo& colorSpaceInfo)
{
    using namespace HDIV;
    static const std::map<GraphicColorGamut, CM_ColorSpaceType> RS_TO_COMMON_COLOR_SPACE_TYPE_MAP {
        {GRAPHIC_COLOR_GAMUT_STANDARD_BT601, CM_BT601_EBU_FULL},
        {GRAPHIC_COLOR_GAMUT_STANDARD_BT709, CM_BT709_FULL},
        {GRAPHIC_COLOR_GAMUT_SRGB, CM_SRGB_FULL},
        {GRAPHIC_COLOR_GAMUT_ADOBE_RGB, CM_ADOBERGB_FULL},
        {GRAPHIC_COLOR_GAMUT_DISPLAY_P3, CM_P3_FULL},
        {GRAPHIC_COLOR_GAMUT_BT2020, CM_DISPLAY_BT2020_SRGB},
        {GRAPHIC_COLOR_GAMUT_BT2100_PQ, CM_BT2020_PQ_FULL},
        {GRAPHIC_COLOR_GAMUT_BT2100_HLG, CM_BT2020_HLG_FULL},
        {GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020, CM_DISPLAY_BT2020_SRGB},
    };

    CM_ColorSpaceType colorSpaceType = CM_COLORSPACE_NONE;
    if (RS_TO_COMMON_COLOR_SPACE_TYPE_MAP.find(colorGamut) != RS_TO_COMMON_COLOR_SPACE_TYPE_MAP.end()) {
        colorSpaceType = RS_TO_COMMON_COLOR_SPACE_TYPE_MAP.at(colorGamut);
    }

    GSError ret = MetadataHelper::ConvertColorSpaceTypeToInfo(colorSpaceType, colorSpaceInfo);
    if (ret != GSERROR_OK) {
        RS_LOGE("bhdr ConvertColorSpaceTypeToInfo failed with %{public}u.", ret);
        return false;
    }

    return true;
}

} // namespace Rosen
} // namespace OHOS
