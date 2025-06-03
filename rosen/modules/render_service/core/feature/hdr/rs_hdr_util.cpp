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

#include "rs_hdr_util.h"

#include "display_engine/rs_color_temperature.h"
#include "hdi_layer_info.h"
#include "metadata_helper.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "platform/common/rs_log.h"
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "render/rs_colorspace_convert.h"
#endif
#include "v2_1/cm_color_space.h"

namespace OHOS {
namespace Rosen {

constexpr float DEFAULT_HDR_RATIO = 1.0f;
constexpr float DEFAULT_SCALER = 1000.0f / 203.0f;
constexpr float GAMMA2_2 = 2.2f;
constexpr size_t MATRIX_SIZE = 9;
static const std::vector<float> DEFAULT_MATRIX = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };

HdrStatus RSHdrUtil::CheckIsHdrSurface(const RSSurfaceRenderNode& surfaceNode)
{
    if (!surfaceNode.IsOnTheTree()) {
        return HdrStatus::NO_HDR;
    }
    if (!surfaceNode.GetRSSurfaceHandler()) {
        return HdrStatus::NO_HDR;
    }
    return CheckIsHdrSurfaceBuffer(surfaceNode.GetRSSurfaceHandler()->GetBuffer());
}

HdrStatus RSHdrUtil::CheckIsHdrSurfaceBuffer(const sptr<SurfaceBuffer> surfaceBuffer)
{
    if (surfaceBuffer == nullptr) {
        return HdrStatus::NO_HDR;
    }
    if (!RSSystemProperties::GetHdrVideoEnabled()) {
        RS_LOGD("RSHdrUtil::CheckIsHdrSurfaceBuffer HDRVideoEnabled false");
        return HdrStatus::NO_HDR;
    }
#ifdef USE_VIDEO_PROCESSING_ENGINE
    std::vector<uint8_t> metadataType{};
    if (surfaceBuffer->GetMetadata(Media::VideoProcessingEngine::ATTRKEY_HDR_METADATA_TYPE, metadataType) ==
        GSERROR_OK && metadataType.size() > 0 &&
        metadataType[0] == HDI::Display::Graphic::Common::V2_1::CM_VIDEO_AI_HDR) {
        return HdrStatus::AI_HDR_VIDEO;
    }
#endif
    if (surfaceBuffer->GetFormat() != GRAPHIC_PIXEL_FMT_RGBA_1010102 &&
        surfaceBuffer->GetFormat() != GRAPHIC_PIXEL_FMT_YCBCR_P010 &&
        surfaceBuffer->GetFormat() != GRAPHIC_PIXEL_FMT_YCRCB_P010) {
        return HdrStatus::NO_HDR;
    }
    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceInfo colorSpaceInfo;
    if (MetadataHelper::GetColorSpaceInfo(surfaceBuffer, colorSpaceInfo) == GSERROR_OK) {
        if (colorSpaceInfo.transfunc == TRANSFUNC_PQ || colorSpaceInfo.transfunc == TRANSFUNC_HLG) {
            return HdrStatus::HDR_VIDEO;
        }
    }
    return HdrStatus::NO_HDR;
}

bool RSHdrUtil::CheckIsSurfaceWithMetadata(const RSSurfaceRenderNode& surfaceNode)
{
    if (!surfaceNode.IsOnTheTree()) {
        return false;
    }
    if (!surfaceNode.GetRSSurfaceHandler()) {
        return false;
    }
    if (!surfaceNode.IsYUVBufferFormat()) {
        return false;
    }
    return CheckIsSurfaceBufferWithMetadata(surfaceNode.GetRSSurfaceHandler()->GetBuffer());
}

bool RSHdrUtil::CheckIsSurfaceBufferWithMetadata(const sptr<SurfaceBuffer> surfaceBuffer)
{
    if (surfaceBuffer == nullptr) {
        return false;
    }
    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceInfo colorSpaceInfo;
    if (MetadataHelper::GetColorSpaceInfo(surfaceBuffer, colorSpaceInfo) != GSERROR_OK) {
        RS_LOGD("RSHdrUtil::CheckIsSurfaceBufferWithMetadata failed to get ColorSpaceInfo");
        return false;
    }
    // only primaries P3_D65 and BT2020 has metadata
    if (colorSpaceInfo.primaries != CM_ColorPrimaries::COLORPRIMARIES_P3_D65 &&
        colorSpaceInfo.primaries != CM_ColorPrimaries::COLORPRIMARIES_BT2020) {
        RS_LOGD("RSHdrUtil::CheckIsSurfaceBufferWithMetadata colorSpaceInfo.primaries not satisfied");
        return false;
    }
#ifdef USE_VIDEO_PROCESSING_ENGINE
    std::vector<uint8_t> videoDynamicMetadata{};
    if (MetadataHelper::GetVideoDynamicMetadata(surfaceBuffer, videoDynamicMetadata) ==
        GSERROR_OK && videoDynamicMetadata.size() > 0) {
        return true;
    }
#endif
    return false;
}

void RSHdrUtil::UpdateSurfaceNodeNit(RSSurfaceRenderNode& surfaceNode, ScreenId screenId)
{
    if (!surfaceNode.GetRSSurfaceHandler()) {
        RS_LOGE("RSHdrUtil::UpdateSurfaceNodeNit surfaceHandler is NULL");
        return;
    }
    const sptr<SurfaceBuffer>& surfaceBuffer = surfaceNode.GetRSSurfaceHandler()->GetBuffer();
    if (surfaceBuffer == nullptr) {
        RS_LOGE("surfaceNode.GetRSSurfaceHandler is NULL");
        return;
    }
    auto& rsLuminance = RSLuminanceControl::Get();
    if (surfaceNode.GetVideoHdrStatus() == HdrStatus::NO_HDR) {
        surfaceNode.SetDisplayNit(rsLuminance.GetSdrDisplayNits(screenId));
        surfaceNode.SetSdrNit(rsLuminance.GetSdrDisplayNits(screenId));
        surfaceNode.SetBrightnessRatio(rsLuminance.GetHdrBrightnessRatio(screenId, 0));
        surfaceNode.SetLayerLinearMatrix(DEFAULT_MATRIX);
        // color temperature
        if (surfaceNode.GetSdrHasMetadata()) {
            UpdateSurfaceNodeLayerLinearMatrix(surfaceNode, screenId);
        }
        return;
    }

    using namespace HDI::Display::Graphic::Common::V1_0;
    std::vector<uint8_t> hdrStaticMetadataVec;
    std::vector<uint8_t> hdrDynamicMetadataVec;
    GSError ret = GSERROR_OK;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    RSColorSpaceConvert::Instance().GetHDRStaticMetadata(surfaceBuffer, hdrStaticMetadataVec, ret);
    RSColorSpaceConvert::Instance().GetHDRDynamicMetadata(surfaceBuffer, hdrDynamicMetadataVec, ret);
#endif
    float scaler = DEFAULT_SCALER;
    auto context = surfaceNode.GetContext().lock();
    if (!context) {
        RS_LOGE("RSHdrUtil::UpdateSurfaceNodeNit context is null");
        return;
    }
    auto displayNode = context->GetNodeMap().GetRenderNode<RSDisplayRenderNode>(surfaceNode.GetDisplayNodeId());
    if (!displayNode) {
        RS_LOGE("RSHdrUtil::UpdateSurfaceNodeNit displayNode is null");
        return;
    }
    float brightnessFactor = displayNode->GetRenderProperties().GetHDRBrightnessFactor();
    if (hdrStaticMetadataVec.size() != sizeof(HdrStaticMetadata) || hdrStaticMetadataVec.data() == nullptr) {
        RS_LOGD("hdrStaticMetadataVec is invalid");
        scaler = surfaceNode.GetHDRBrightness() * brightnessFactor * (scaler - 1.0f) + 1.0f;
    } else {
        const auto& data = *reinterpret_cast<HdrStaticMetadata*>(hdrStaticMetadataVec.data());
        scaler = rsLuminance.CalScaler(data.cta861.maxContentLightLevel, ret == GSERROR_OK ?
            hdrDynamicMetadataVec : std::vector<uint8_t>{}, surfaceNode.GetHDRBrightness() * brightnessFactor);
    }

    float sdrNits = rsLuminance.GetSdrDisplayNits(screenId);
    float displayNits = rsLuminance.GetDisplayNits(screenId);

    float layerNits = std::clamp(sdrNits * scaler, sdrNits, displayNits);
    surfaceNode.SetDisplayNit(layerNits);
    surfaceNode.SetSdrNit(sdrNits);
    if (ROSEN_LE(displayNits, 0.0f)) {
        surfaceNode.SetBrightnessRatio(DEFAULT_HDR_RATIO);
    } else {
        surfaceNode.SetBrightnessRatio(std::pow(layerNits / displayNits, 1.0f / GAMMA2_2)); // gamma 2.2
    }
    // color temperature
    UpdateSurfaceNodeLayerLinearMatrix(surfaceNode, screenId);
    RS_LOGD("RSHdrUtil::UpdateSurfaceNodeNit layerNits: %{public}.2f, displayNits: %{public}.2f,"
        " sdrNits: %{public}.2f, scaler: %{public}.2f, HDRBrightness: %{public}f, brightnessFactor: %{public}f",
        layerNits, displayNits, sdrNits, scaler, surfaceNode.GetHDRBrightness(), brightnessFactor);
}

void RSHdrUtil::UpdateSurfaceNodeLayerLinearMatrix(RSSurfaceRenderNode& surfaceNode, ScreenId screenId)
{
    if (!surfaceNode.GetRSSurfaceHandler()) {
        RS_LOGE("RSHdrUtil::UpdateSurfaceNodeLayerLinearMatrix surfaceHandler is NULL");
        return;
    }
    const sptr<SurfaceBuffer>& surfaceBuffer = surfaceNode.GetRSSurfaceHandler()->GetBuffer();
    if (surfaceBuffer == nullptr) {
        RS_LOGE("surfaceNode.GetRSSurfaceHandler is NULL");
        return;
    }
    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceInfo srcColorSpaceInfo;
    CM_Matrix srcColorMatrix = CM_Matrix::MATRIX_P3;
    if (MetadataHelper::GetColorSpaceInfo(surfaceBuffer, srcColorSpaceInfo) == GSERROR_OK) {
        srcColorMatrix = srcColorSpaceInfo.matrix;
    }
    std::vector<uint8_t> dynamicMetadataVec;
    GSError ret = GSERROR_OK;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    if (srcColorMatrix == CM_Matrix::MATRIX_BT2020) {
        RSColorSpaceConvert::Instance().GetHDRDynamicMetadata(surfaceBuffer, dynamicMetadataVec, ret);
    } else {
        RSColorSpaceConvert::Instance().GetVideoDynamicMetadata(surfaceBuffer, dynamicMetadataVec, ret);
    }
#endif
    std::vector<float> layerLinearMatrix = RSColorTemperature::Get().GetLayerLinearCct(screenId,
        ret == GSERROR_OK ? dynamicMetadataVec : std::vector<uint8_t>(), srcColorMatrix);
    surfaceNode.SetLayerLinearMatrix(layerLinearMatrix);
    if (layerLinearMatrix.size() >= MATRIX_SIZE) {
        // main diagonal indices of a 3x3 matrix are 0, 4 and 8
        RS_LOGD("RSHdrUtil::UpdateSurfaceNodeLayerLinearMatrix "
            "matrix[0]: %{public}.2f, matrix[4]: %{public}.2f, matrix[8]: %{public}.2f",
            layerLinearMatrix[0], layerLinearMatrix[4], layerLinearMatrix[8]);
    }
}

void RSHdrUtil::UpdatePixelFormatAfterHwcCalc(RSDisplayRenderNode& node)
{
    const auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
    for (const auto& selfDrawingNode : selfDrawingNodes) {
        if (!selfDrawingNode || !selfDrawingNode->GetAncestorDisplayNode().lock()) {
            RS_LOGD("RSHdrUtil::UpdatePixelFormatAfterHwcCalc selfDrawingNode or ancestoreNode is nullptr");
            continue;
        }
        auto ancestor = selfDrawingNode->GetAncestorDisplayNode().lock()->ReinterpretCastTo<RSDisplayRenderNode>();
        if (ancestor != nullptr && node.GetId() == ancestor->GetId()) {
            CheckPixelFormatWithSelfDrawingNode(*selfDrawingNode, node);
        }
    }
}

void RSHdrUtil::CheckPixelFormatWithSelfDrawingNode(RSSurfaceRenderNode& surfaceNode,
    RSDisplayRenderNode& displayNode)
{
    if (!surfaceNode.IsOnTheTree()) {
        RS_LOGD("RSHdrUtil::CheckPixelFormatWithSelfDrawingNode node(%{public}s) is not on the tree",
            surfaceNode.GetName().c_str());
        return;
    }
    if (!surfaceNode.GetRSSurfaceHandler()) {
        RS_LOGD("RSHdrUtil::CheckPixelFormatWithSelfDrawingNode surfaceHandler is null");
        return;
    }
    auto screenId = displayNode.GetScreenId();
    UpdateSurfaceNodeNit(surfaceNode, screenId);
    displayNode.CollectHdrStatus(surfaceNode.GetVideoHdrStatus());
    if (RSLuminanceControl::Get().IsForceCloseHdr()) {
        RS_LOGD("RSHdrUtil::CheckPixelFormatWithSelfDrawingNode node(%{public}s) forceCloseHdr.",
            surfaceNode.GetName().c_str());
        return;
    }
    if (displayNode.GetForceCloseHdr()) {
        RS_LOGD("RSHdrUtil::CheckPixelFormatWithSelfDrawingNode node(%{public}s) forceclosehdr.",
            surfaceNode.GetName().c_str());
        return;
    }
    if (!surfaceNode.IsHardwareForcedDisabled()) {
        RS_LOGD("RSHdrUtil::CheckPixelFormatWithSelfDrawingNode node(%{public}s) is hardware-enabled",
            surfaceNode.GetName().c_str());
        return;
    }
    if (surfaceNode.GetVideoHdrStatus() != HdrStatus::NO_HDR) {
        SetHDRParam(surfaceNode, true);
        if (displayNode.GetIsLuminanceStatusChange()) {
            surfaceNode.SetContentDirty();
        }
        displayNode.SetPixelFormat(GRAPHIC_PIXEL_FMT_RGBA_1010102);
        RS_LOGD("RSHdrUtil::CheckPixelFormatWithSelfDrawingNode HDRService pixelformat is set to 1010102");
    }
}

void RSHdrUtil::SetHDRParam(RSSurfaceRenderNode& node, bool flag)
{
    auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetFirstLevelNode());
    if (firstLevelNode != nullptr && node.GetFirstLevelNodeId() != node.GetId()) {
        firstLevelNode->SetHDRPresent(flag);
    }
    node.SetHDRPresent(flag);
}

} // namespace Rosen
} // namespace OHOS