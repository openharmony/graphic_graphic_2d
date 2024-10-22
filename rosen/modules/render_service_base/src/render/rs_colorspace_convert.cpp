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

#include <dlfcn.h>

#include "effect/image_filter.h"
#include "luminance/rs_luminance_control.h"
#include "metadata_helper.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr float DEFAULT_SCALER = 1000.0f / 203.0f;
}; // namespace

RSColorSpaceConvert::RSColorSpaceConvert()
{
    handle_ = dlopen("libvideoprocessingengine.z.so", RTLD_LAZY);
    if (handle_ == nullptr) {
        RS_LOGW("[%{public}s]:load library failed, reason: %{public}s", __func__, dlerror());
        return;
    }
    colorSpaceConvertDisplayCreate_ = reinterpret_cast<VPEColorSpaceConvertDisplayCreate>(
        dlsym(handle_, "ColorSpaceConvertDisplayCreate"));
    if (colorSpaceConvertDisplayCreate_ == nullptr) {
        RS_LOGW("[%{public}s]:load func failed, reason: %{public}s", __func__, dlerror());
        if (dlclose(handle_) != 0) {
            ROSEN_LOGE("Could not close the handle. This indicates a leak. %{public}s", dlerror());
        }
        handle_ = nullptr;
        return;
    }
    colorSpaceConvertDisplayDestroy_ = reinterpret_cast<VPEColorSpaceConvertDisplayDestroy>(
        dlsym(handle_, "ColorSpaceConvertDisplayDestroy"));
    if (colorSpaceConvertDisplayDestroy_ == nullptr) {
        RS_LOGW("[%{public}s]:load func failed, reason: %{public}s", __func__, dlerror());
        if (dlclose(handle_) != 0) {
            ROSEN_LOGE("Could not close the handle. This indicates a leak. %{public}s", dlerror());
        }
        handle_ = nullptr;
        return;
    }
    colorSpaceConvertDisplayHandle_ = colorSpaceConvertDisplayCreate_();
    if (colorSpaceConvertDisplayHandle_ == nullptr) {
        RS_LOGE("ColorSpaceConvertDisplayCreate failed, return nullptr");
        if (dlclose(handle_) != 0) {
            ROSEN_LOGE("Could not close the handle. This indicates a leak. %{public}s", dlerror());
        }
        handle_ = nullptr;
        return;
    }
    colorSpaceConverterDisplay_ = static_cast<ColorSpaceConvertDisplayHandleImpl *>(
        colorSpaceConvertDisplayHandle_)->obj;
}

RSColorSpaceConvert::~RSColorSpaceConvert()
{
    if (colorSpaceConvertDisplayHandle_) {
        colorSpaceConvertDisplayDestroy_(colorSpaceConvertDisplayHandle_);
        colorSpaceConvertDisplayHandle_ = nullptr;
    }
    if (handle_) {
        if (dlclose(handle_) != 0) {
            ROSEN_LOGE("Could not close the handle. This indicates a leak. %{public}s", dlerror());
        }
        handle_ = nullptr;
    }
    colorSpaceConvertDisplayCreate_ = nullptr;
    colorSpaceConvertDisplayDestroy_ = nullptr;
    colorSpaceConverterDisplay_ = nullptr;
}

RSColorSpaceConvert& RSColorSpaceConvert::Instance()
{
    static RSColorSpaceConvert instance;
    return instance;
}

bool RSColorSpaceConvert::ColorSpaceConvertor(std::shared_ptr<Drawing::ShaderEffect> inputShader,
    const sptr<SurfaceBuffer>& surfaceBuffer, Drawing::Paint& paint, GraphicColorGamut targetColorSpace,
    ScreenId screenId, uint32_t dynamicRangeMode)
{
    RS_LOGD("RSColorSpaceConvertor targetColorSpace:%{public}d. screenId:%{public}" PRIu64 ". \
        dynamicRangeMode%{public}u", targetColorSpace, screenId, dynamicRangeMode);
    VPEParameter parameter;

    if (inputShader == nullptr) {
        RS_LOGE("bhdr imageShader is nullptr.");
        return false;
    }

    if (!SetColorSpaceConverterDisplayParameter(surfaceBuffer, parameter, targetColorSpace, screenId,
        dynamicRangeMode)) {
        return false;
    }
    if (dynamicRangeMode == DynamicRangeMode::STANDARD) {
        parameter.disableHeadRoom = true;
    }

    std::shared_ptr<Drawing::ShaderEffect> outputShader;
    
    if (colorSpaceConverterDisplay_ == nullptr) {
        RS_LOGE("colorSpaceConverterDisplay_ is nullptr.");
        return false;
    }
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
        RS_LOGD("bhdr GetHDRStaticMetadata failed with %{public}u.", ret);
    }

    float scaler = DEFAULT_SCALER;
    auto& rsLuminance = RSLuminanceControl::Get();
    if (parameter.staticMetadata.size() != sizeof(HdrStaticMetadata)) {
        RS_LOGD("bhdr parameter.staticMetadata size is invalid");
    } else {
        const auto& data = *reinterpret_cast<HdrStaticMetadata*>(parameter.staticMetadata.data());
        scaler = rsLuminance.CalScaler(data.cta861.maxContentLightLevel);
    }

    ret = MetadataHelper::GetHDRDynamicMetadata(surfaceBuffer, parameter.dynamicMetadata);
    if (ret != GSERROR_OK) {
        RS_LOGD("bhdr GetHDRDynamicMetadata failed with %{public}u.", ret);
    }

    // Set brightness to screen brightness when HDR Vivid, otherwise 500 nits
    float sdrNits = rsLuminance.GetSdrDisplayNits(screenId);
    float displayNits = rsLuminance.GetDisplayNits(screenId);
    parameter.tmoNits = std::clamp(sdrNits * scaler, sdrNits, displayNits);
    parameter.currentDisplayNits = displayNits;
    parameter.sdrNits = sdrNits;
    RS_LOGD("bhdr TmoNits:%{public}f. DisplayNits:%{public}f. SdrNits:%{public}f.", parameter.tmoNits,
        parameter.currentDisplayNits, parameter.sdrNits);
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
