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

#include "rs_base_hdr_util.h"

#include <parameters.h>

#include "metadata_helper.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "render/rs_colorspace_convert.h"
#endif
#include "v2_2/cm_color_space.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float DEFAULT_SDR_NITS = 203.0f;
constexpr float GAMMA2_2 = 2.2f;
}

inline HdrStatus CheckAIHDRType(uint8_t metadataType)
{
    switch (metadataType) {
        case HDI::Display::Graphic::Common::V2_2::CM_VIDEO_AI_HDR:
            return HdrStatus::AI_HDR_VIDEO_GTM;
        case HDI::Display::Graphic::Common::V2_2::CM_VIDEO_AI_HDR_HIGH_LIGHT:
            return HdrStatus::AI_HDR_VIDEO_GAINMAP;
        case HDI::Display::Graphic::Common::V2_2::CM_VIDEO_AI_HDR_COLOR_ENHANCE:
            return HdrStatus::AI_HDR_VIDEO_GAINMAP;
        default:
            return HdrStatus::NO_HDR;
    }
}

// HDR self-processing layer has HDR StaticMetadata and maxContentLightLevel > 203 and has no HDR MetadataType
bool RSBaseHdrUtil::CheckIsHDRSelfProcessingBuffer(const sptr<SurfaceBuffer>& surfaceBuffer)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    std::vector<uint8_t> hdrStaticMetadataVec;
    GSError ret = GSERROR_OK;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    RSColorSpaceConvert::Instance().GetHDRStaticMetadata(surfaceBuffer, hdrStaticMetadataVec, ret);
#endif
    bool hdrStaticMetadataExist = hdrStaticMetadataVec.data() != nullptr &&
        hdrStaticMetadataVec.size() == sizeof(HdrStaticMetadata);
    if (!hdrStaticMetadataExist) {
        return false;
    }
    CM_HDR_Metadata_Type hdrMetadataType = CM_METADATA_NONE;
    ret = MetadataHelper::GetHDRMetadataType(surfaceBuffer, hdrMetadataType);
    bool noneHDRMetadataType = ret != GSERROR_OK || hdrMetadataType == CM_METADATA_NONE;
    if (!noneHDRMetadataType) {
        return false;
    }
    CM_ColorSpaceInfo colorSpaceInfo;
    bool isHDRVideo = MetadataHelper::GetColorSpaceInfo(surfaceBuffer, colorSpaceInfo) == GSERROR_OK &&
        (colorSpaceInfo.transfunc == TRANSFUNC_PQ || colorSpaceInfo.transfunc == TRANSFUNC_HLG);
    if (isHDRVideo) {
        return false;
    }
    const auto& data = *reinterpret_cast<HdrStaticMetadata*>(hdrStaticMetadataVec.data());
    return ROSEN_GNE(data.cta861.maxContentLightLevel, DEFAULT_SDR_NITS);
}

bool RSBaseHdrUtil::CheckIsSurfaceBufferWithMetadata(const sptr<SurfaceBuffer> surfaceBuffer)
{
    if (surfaceBuffer == nullptr) {
        return false;
    }
    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceInfo colorSpaceInfo;
    if (MetadataHelper::GetColorSpaceInfo(surfaceBuffer, colorSpaceInfo) != GSERROR_OK) {
        RS_LOGD("RSBaseHdrUtil::CheckIsSurfaceBufferWithMetadata failed to get ColorSpaceInfo");
        return false;
    }
    // only primaries P3_D65 and BT2020 has metadata
    if (colorSpaceInfo.primaries != CM_ColorPrimaries::COLORPRIMARIES_P3_D65 &&
        colorSpaceInfo.primaries != CM_ColorPrimaries::COLORPRIMARIES_BT2020) {
        RS_LOGD("RSBaseHdrUtil::CheckIsSurfaceBufferWithMetadata colorSpaceInfo.primaries not satisfied");
        return false;
    }
#ifdef USE_VIDEO_PROCESSING_ENGINE
    std::vector<uint8_t> sdrDynamicMetadata{};
    if (MetadataHelper::GetSDRDynamicMetadata(surfaceBuffer, sdrDynamicMetadata) ==
        GSERROR_OK && sdrDynamicMetadata.size() > 0) {
        return true;
    }
#endif
    return false;
}

bool RSBaseHdrUtil::CheckIsSurfaceBufferWithAiHdrMetadata(const sptr<SurfaceBuffer> surfaceBuffer)
{
    if (surfaceBuffer == nullptr) {
        return false;
    }
    if (!RSSystemProperties::GetHdrVideoEnabled()) {
        RS_LOGD("RSBaseHdrUtil::CheckIsSurfaceBufferWithAiHdrMetadata HDRVideoEnabled false");
        return false;
    }
#ifdef USE_VIDEO_PROCESSING_ENGINE
    std::vector<uint8_t> metadataType{};
    bool isHdrStatus = surfaceBuffer->GetMetadata(ATTRKEY_HDR_METADATA_TYPE, metadataType) == GSERROR_OK &&
        !metadataType.empty();
    if (isHdrStatus && (CheckAIHDRType(metadataType[0]) != HdrStatus::NO_HDR)) {
        return true;
    }
#endif
    return false;
}

HdrStatus RSBaseHdrUtil::CheckIsHdrSurfaceBuffer(const sptr<SurfaceBuffer> surfaceBuffer)
{
    if (surfaceBuffer == nullptr) {
        return HdrStatus::NO_HDR;
    }
    if (!RSSystemProperties::GetHdrVideoEnabled()) {
        RS_LOGD("RSBaseHdrUtil::CheckIsHdrSurfaceBuffer HDRVideoEnabled false");
        return HdrStatus::NO_HDR;
    }
#ifdef USE_VIDEO_PROCESSING_ENGINE
    std::vector<uint8_t> metadataType{};
    if (surfaceBuffer->GetMetadata(ATTRKEY_HDR_METADATA_TYPE, metadataType) == GSERROR_OK && !metadataType.empty()) {
        auto AIHDRStatus = CheckAIHDRType(metadataType[0]);
        if (AIHDRStatus != HdrStatus::NO_HDR) {
            return AIHDRStatus;
        }
    }
#endif
    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceInfo colorSpaceInfo;
    if (MetadataHelper::GetColorSpaceInfo(surfaceBuffer, colorSpaceInfo) == GSERROR_OK) {
        if (colorSpaceInfo.transfunc == TRANSFUNC_PQ || colorSpaceInfo.transfunc == TRANSFUNC_HLG) {
            return HdrStatus::HDR_VIDEO;
        }
    }
    if (RSBaseHdrUtil::CheckIsHDRSelfProcessingBuffer(surfaceBuffer)) {
        RS_TRACE_NAME_FMT("%s CheckIsHDRSelfProcessingBuffer is true", __func__);
        return HdrStatus::HDR_VIDEO;
    }
    return HdrStatus::NO_HDR;
}

void RSBaseHdrUtil::SetBufferHDRParam(BufferDrawParam& params, const RSLayerPtr& layer)
{
#ifdef USE_VIDEO_PROCESSING_ENGINE
    params.sdrNits = layer->GetSdrNit();
    params.tmoNits = layer->GetDisplayNit();
    params.displayNits = params.tmoNits / std::pow(layer->GetBrightnessRatio(), GAMMA2_2); // gamma 2.2
    // color temperature
    params.layerLinearMatrix = layer->GetLayerLinearMatrix();
    bool isSurfaceBufferWithMetadata = false;
    if (CheckIsHdrSurfaceBuffer(layer->GetBuffer()) == HdrStatus::NO_HDR) {
        params.brightnessRatio = layer->GetBrightnessRatio();
        isSurfaceBufferWithMetadata = CheckIsSurfaceBufferWithMetadata(layer->GetBuffer());
    } else {
        params.isHdrRedraw = true;
    }
    if (isSurfaceBufferWithMetadata ||
        CheckIsSurfaceBufferWithAiHdrMetadata(layer->GetBuffer())) {
        params.hasMetadata = true;
    }
#endif
}

bool RSBaseHdrUtil::GetRGBA1010108Enabled()
{
    static bool isDDGR = Drawing::SystemProperties::GetSystemGraphicGpuType() == GpuApiType::DDGR;
    static bool rgba1010108 = system::GetBoolParameter("const.graphics.rgba_1010108_supported", false);
    static bool debugSwitch = system::GetBoolParameter("persist.sys.graphic.rgba_1010108.enabled", true);
    return isDDGR && rgba1010108 && debugSwitch;
}
} // namespace Rosen
} // namespace OHOS