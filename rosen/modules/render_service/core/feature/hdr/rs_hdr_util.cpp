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

#include <parameters.h>

#include "display_engine/rs_color_temperature.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "hdi_layer_info.h"
#include "metadata_helper.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "utils/system_properties.h"
#include "v2_2/cm_color_space.h"
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "render/rs_colorspace_convert.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr float DEFAULT_HDR_RATIO = 1.0f;
constexpr float DEFAULT_SDR_NITS = 203.0f;
constexpr float DEFAULT_SCALER = 1000.0f / 203.0f;
constexpr float GAMMA2_2 = 2.2f;
constexpr size_t MATRIX_SIZE = 9;
const std::vector<float> DEFAULT_MATRIX = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
const std::vector<uint8_t> HDR_VIVID_METADATA = {
    1, 0, 23, 93, 111, 186, 221, 240, 26, 189, 83, 29, 128, 0, 82, 142, 25, 156, 3,
    198, 204, 179, 47, 236, 32, 190, 143, 163, 252, 16, 93, 185, 106, 159, 0, 10,
    81, 199, 178, 80, 255, 217, 150, 101, 201, 144, 114, 73, 65, 127, 160, 0, 0,
    0, 72, 85, 65, 87, 0, 8, 1, 67, 65, 83, 84, 0, 1, 1
};
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
    if (CheckIsHDRSelfProcessingBuffer(surfaceBuffer)) {
        RS_TRACE_NAME_FMT("%s CheckIsHDRSelfProcessingBuffer is true", __func__);
        return HdrStatus::HDR_VIDEO;
    }
    return HdrStatus::NO_HDR;
}

// HDR self-processing layer has HDR StaticMetadata and maxContentLightLevel > 203 and has no HDR MetadataType
bool RSHdrUtil::CheckIsHDRSelfProcessingBuffer(const sptr<SurfaceBuffer>& surfaceBuffer)
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
    std::vector<uint8_t> sdrDynamicMetadata{};
    if (MetadataHelper::GetSDRDynamicMetadata(surfaceBuffer, sdrDynamicMetadata) ==
        GSERROR_OK && sdrDynamicMetadata.size() > 0) {
        return true;
    }
#endif
    return false;
}

bool RSHdrUtil::UpdateSurfaceNodeNit(RSSurfaceRenderNode& surfaceNode, ScreenId screenId, float& scaler)
{
    if (!surfaceNode.GetRSSurfaceHandler()) {
        RS_LOGE("RSHdrUtil::UpdateSurfaceNodeNit surfaceHandler is NULL");
        return false;
    }
    const sptr<SurfaceBuffer>& surfaceBuffer = surfaceNode.GetRSSurfaceHandler()->GetBuffer();
    if (surfaceBuffer == nullptr) {
        RS_LOGE("surfaceNode.GetRSSurfaceHandler is NULL");
        return false;
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
        return false;
    }

    using namespace HDI::Display::Graphic::Common::V1_0;
    std::vector<uint8_t> hdrStaticMetadataVec;
    std::vector<uint8_t> hdrDynamicMetadataVec;
    GSError ret = GSERROR_OK;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    RSColorSpaceConvert::Instance().GetHDRStaticMetadata(surfaceBuffer, hdrStaticMetadataVec, ret);
    RSColorSpaceConvert::Instance().GetHDRDynamicMetadata(surfaceBuffer, hdrDynamicMetadataVec, ret);
#endif
    scaler = DEFAULT_SCALER;
    auto context = surfaceNode.GetContext().lock();
    if (!context) {
        RS_LOGE("RSHdrUtil::UpdateSurfaceNodeNit context is null");
        return false;
    }
    auto displayNode =
        context->GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(surfaceNode.GetLogicalDisplayNodeId());
    if (!displayNode) {
        RS_LOGE("RSHdrUtil::UpdateSurfaceNodeNit displayNode is null");
        return false;
    }
    float brightnessFactor = displayNode->GetRenderProperties().GetHDRBrightnessFactor();
    if (ROSEN_NE(surfaceNode.GetHDRBrightnessFactor(), brightnessFactor)) {
        RS_LOGD("RSHdrUtil::UpdateSurfaceNodeNit GetHDRBrightnessFactor: %{public}f, "
            "displayNode brightnessFactor: %{public}f, nodeId: %{public}" PRIu64 "",
            surfaceNode.GetHDRBrightnessFactor(), brightnessFactor, surfaceNode.GetId());
        surfaceNode.SetHDRBrightnessFactor(brightnessFactor);
        surfaceNode.SetContentDirty();
    }
    if (hdrStaticMetadataVec.size() != sizeof(HdrStaticMetadata) || hdrStaticMetadataVec.data() == nullptr) {
        RS_LOGD("hdrStaticMetadataVec is invalid");
        auto hdrStatus = CheckIsHdrSurfaceBuffer(surfaceBuffer);
        if (hdrStatus == HdrStatus::AI_HDR_VIDEO_GAINMAP || hdrStatus == HdrStatus::AI_HDR_VIDEO_GTM) {
            scaler = rsLuminance.CalScaler(1.0f, std::vector<uint8_t>{},
                surfaceNode.GetHDRBrightness() * brightnessFactor, hdrStatus);
        } else {
            scaler = surfaceNode.GetHDRBrightness() * brightnessFactor * (scaler - 1.0f) + 1.0f;
        }
    } else {
        const auto& data = *reinterpret_cast<HdrStaticMetadata*>(hdrStaticMetadataVec.data());
        scaler = rsLuminance.CalScaler(data.cta861.maxContentLightLevel, ret == GSERROR_OK ? hdrDynamicMetadataVec :
            std::vector<uint8_t>{}, surfaceNode.GetHDRBrightness() * brightnessFactor, HdrStatus::HDR_VIDEO);
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
    return true;
}

void RSHdrUtil::UpdateSelfDrawingNodesNit(RSScreenRenderNode& node)
{
    const auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
    for (const auto& selfDrawingNode : selfDrawingNodes) {
        if (!selfDrawingNode) {
            RS_LOGD("RSHdrUtil::UpdateSelfDrawingNodesNit selfDrawingNode is nullptr");
            continue;
        }
        if (!selfDrawingNode->IsOnTheTree()) {
            RS_LOGD("RSHdrUtil::UpdateSelfDrawingNodesNit node(%{public}s) is not on the tree",
                selfDrawingNode->GetName().c_str());
            continue;
        }
        auto ancestor = selfDrawingNode->GetAncestorScreenNode().lock();
        if (!ancestor) {
            RS_LOGD("RSHdrUtil::UpdateSelfDrawingNodesNit ancestor is nullptr");
            continue;
        }
        if (node.GetId() == ancestor->GetId()) {
            if (float scaler; RSHdrUtil::UpdateSurfaceNodeNit(*selfDrawingNode, node.GetScreenId(), scaler)) {
                uint32_t level = RSLuminanceControl::Get().ConvertScalerFromFloatToLevel(scaler);
                node.UpdateHeadroomMapIncrease(selfDrawingNode->GetVideoHdrStatus(), level);
            }
        }
    }
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
    CM_TransFunc srcTransFunc = CM_TransFunc::TRANSFUNC_SRGB;
    if (MetadataHelper::GetColorSpaceInfo(surfaceBuffer, srcColorSpaceInfo) == GSERROR_OK) {
        srcColorMatrix = srcColorSpaceInfo.matrix;
        srcTransFunc = srcColorSpaceInfo.transfunc;
    }
    std::vector<uint8_t> dynamicMetadataVec;
    GSError ret = GSERROR_OK;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    if (srcTransFunc == CM_TransFunc::TRANSFUNC_PQ || srcTransFunc == CM_TransFunc::TRANSFUNC_HLG) {
        RSColorSpaceConvert::Instance().GetHDRDynamicMetadata(surfaceBuffer, dynamicMetadataVec, ret);
    } else {
        RSColorSpaceConvert::Instance().GetSDRDynamicMetadata(surfaceBuffer, dynamicMetadataVec, ret);
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

void RSHdrUtil::UpdatePixelFormatAfterHwcCalc(RSScreenRenderNode& node)
{
    const auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
    for (const auto& selfDrawingNode : selfDrawingNodes) {
        if (!selfDrawingNode) {
            RS_LOGD("RSHdrUtil::UpdatePixelFormatAfterHwcCalc selfDrawingNode is nullptr");
            continue;
        }
        auto ancestor = selfDrawingNode->GetAncestorScreenNode().lock();
        if (!ancestor) {
            RS_LOGD("RSHdrUtil::UpdatePixelFormatAfterHwcCalc ancestor is nullptr");
            continue;
        }
        if (node.GetId() == ancestor->GetId()) {
            CheckPixelFormatWithSelfDrawingNode(*selfDrawingNode, node);
            bool needSetExistHWCNode = !node.GetExistHWCNode() && selfDrawingNode->IsOnTheTree() &&
                !selfDrawingNode->IsHardwareForcedDisabled();
            if (needSetExistHWCNode) {
                RS_LOGD("RSHdrUtil::UpdatePixelFormatAfterHwcCalc there exist HWCNode");
                node.SetExistHWCNode(true);
            }
        }
    }
}

void RSHdrUtil::CheckPixelFormatWithSelfDrawingNode(RSSurfaceRenderNode& surfaceNode,
    RSScreenRenderNode& screenNode)
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
    auto screenId = screenNode.GetScreenId();
    screenNode.CollectHdrStatus(surfaceNode.GetVideoHdrStatus());

    if (RSLuminanceControl::Get().IsForceCloseHdr()) {
        RS_LOGD("RSHdrUtil::CheckPixelFormatWithSelfDrawingNode node(%{public}s) forceCloseHdr.",
            surfaceNode.GetName().c_str());
        return;
    }
    if (!surfaceNode.IsHardwareForcedDisabled()) {
        RS_LOGD("RSHdrUtil::CheckPixelFormatWithSelfDrawingNode node(%{public}s) is hardware-enabled",
            surfaceNode.GetName().c_str());
        return;
    }
    if (surfaceNode.GetVideoHdrStatus() != HdrStatus::NO_HDR) {
        SetHDRParam(screenNode, surfaceNode, false);
        if (screenNode.GetIsLuminanceStatusChange()) {
            surfaceNode.SetContentDirty();
        }
        RS_LOGD("RSHdrUtil::CheckPixelFormatWithSelfDrawingNode HDRService surfaceNode %{public}s is HDR",
            surfaceNode.GetName().c_str());
    }
}


void RSHdrUtil::CheckPixelFormatForHdrEffect(RSSurfaceRenderNode& surfaceNode,
    std::shared_ptr<RSScreenRenderNode> screenNode)
{
    if (!screenNode) {
        RS_LOGE("CheckPixelFormatForHdrEffect screenNode is nullptr");
        return;
    }
    if (!RSSystemProperties::GetHdrImageEnabled()) {
        RS_LOGD("CheckPixelFormatForHdrEffect HdrImageEnabled false");
        return;
    }
    if (surfaceNode.IsHdrEffectColorGamut()) {
        RS_LOGD("CheckPixelFormatForHdrEffect IsHdrEffectColorGamut: %{public}d, id: %{public}" PRIu64 "",
            surfaceNode.IsHdrEffectColorGamut(), surfaceNode.GetId());
        screenNode->SetHasUniRenderHdrSurface(true);
        screenNode->CollectHdrStatus(HdrStatus::HDR_EFFECT);
        SetHDRParam(*screenNode, surfaceNode, true);
    }
}

bool RSHdrUtil::GetRGBA1010108Enabled()
{
    static bool isDDGR = Drawing::SystemProperties::GetSystemGraphicGpuType() == GpuApiType::DDGR;
    static bool rgba1010108 = system::GetBoolParameter("const.graphics.rgba_1010108_supported", false);
    static bool debugSwitch = system::GetBoolParameter("persist.sys.graphic.rgba_1010108.enabled", true);
    return isDDGR && rgba1010108 && debugSwitch;
}

void RSHdrUtil::SetHDRParam(RSScreenRenderNode& screenNode, RSSurfaceRenderNode& node, bool isEDR)
{
    if (screenNode.GetForceCloseHdr()) {
        RS_LOGD("RSHdrUtil::SetHDRParam curScreenNode forceclosehdr.");
        return;
    }
    auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetFirstLevelNode());
    if (firstLevelNode != nullptr && node.GetFirstLevelNodeId() != node.GetId()) {
        firstLevelNode->SetHDRPresent(true);
        if (isEDR) {
            firstLevelNode->UpdateHDRStatus(HdrStatus::HDR_EFFECT, true);
        }
        RS_LOGD("RSHdrUtil::SetHDRParam HDRService FirstLevelNode: %{public}" PRIu64 "",
            node.GetFirstLevelNodeId());
    }
    node.SetHDRPresent(true);
    if (isEDR) {
        node.UpdateHDRStatus(HdrStatus::HDR_EFFECT, true);
    }
}

void RSHdrUtil::LuminanceChangeSetDirty(RSScreenRenderNode& node)
{
    if (!node.GetIsLuminanceStatusChange()) {
        return;
    }
    auto childList = node.GetChildrenList();
    if (childList.empty()) {
        return;
    }
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (const auto& child : childList) {
        auto childPtr = child.lock();
        if (!childPtr) {
            RS_LOGE("RSHdrUtil::LuminanceChangeSetDirty child is null");
            continue;
        }
        auto displayNode = childPtr->ReinterpretCastTo<RSLogicalDisplayRenderNode>();
        if (!displayNode) {
            RS_LOGE("RSHdrUtil::LuminanceChangeSetDirty child is not displayNode");
            continue;
        }
        const auto& hdrNodeMap = displayNode->GetHDRNodeMap();
        for (const auto& [nodeId, _] : hdrNodeMap) {
            auto canvasNode = nodeMap.GetRenderNode(nodeId);
            if (!canvasNode) {
                RS_LOGD("RSHdrUtil::LuminanceChangeSetDirty canvasNode is not on the tree");
                continue;
            }
            canvasNode->SetContentDirty();
        }
    }
}

void RSHdrUtil::CheckNotifyCallback(RSContext& context, ScreenId screenId)
{
    bool needNotifyCallback = !context.IsBrightnessInfoChangeCallbackMapEmpty() &&
        RSLuminanceControl::Get().IsBrightnessInfoChanged(screenId);
    if (needNotifyCallback) {
        BrightnessInfo info = RSLuminanceControl::Get().GetBrightnessInfo(screenId);
        context.NotifyBrightnessInfoChangeCallback(screenId, info);
        RS_TRACE_NAME_FMT("%s curHeadroom:%f maxHeadroom:%f sdrNits:%f screenId:%" PRIu64 "",
            __func__, info.currentHeadroom, info.maxHeadroom, info.sdrNits, screenId);
    }
}

bool RSHdrUtil::BufferFormatNeedUpdate(const std::shared_ptr<Drawing::Surface>& cacheSurface, bool isNeedFP16)
{
    bool bufferFormatNeedUpdate = cacheSurface ? isNeedFP16 &&
        cacheSurface->GetImageInfo().GetColorType() != Drawing::ColorType::COLORTYPE_RGBA_F16 : true;
    RS_LOGD("RSHdrUtil::BufferFormatNeedUpdate: %{public}d", bufferFormatNeedUpdate);
    return bufferFormatNeedUpdate;
}

ScreenColorGamut RSHdrUtil::GetScreenColorGamut(RSScreenRenderNode& node, const sptr<RSScreenManager>& screenManager)
{
    ScreenColorGamut screenColorGamut;
    if (screenManager->GetScreenColorGamut(node.GetScreenId(), screenColorGamut) != SUCCESS) {
        RS_LOGD("RSHdrUtil::GetScreenColorGamut get screen color gamut failed.");
        return COLOR_GAMUT_INVALID;
    }
    return screenColorGamut;
}

bool RSHdrUtil::NeedUseF16Capture(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    if (!surfaceNode) {
        RS_LOGE("RSHdrUtil::NeedUseF16Capture surfaceNode is nullptr.");
        return false;
    }
    auto screenNode =
        RSBaseRenderNode::ReinterpretCast<RSScreenRenderNode>(surfaceNode->GetAncestorScreenNode().lock());
    if (!screenNode) {
        RS_LOGE("RSHdrUtil::NeedUseF16Capture surfaceNode get ancestor screenNode failed.");
        return false;
    }
    const auto screenParams = static_cast<RSScreenRenderParams*>(screenNode->GetStagingRenderParams().get());
    if (!screenParams) {
        RS_LOGE("RSHdrUtil::NeedUseF16Capture get params from screenNode failed.");
        return false;
    }
    bool isHDROn = screenParams->GetHDRPresent();
    bool isScRGBEnable = RSSystemParameters::IsNeedScRGBForP3(screenParams->GetNewColorSpace()) &&
        RSUifirstManager::Instance().GetUiFirstSwitch();
    GraphicColorGamut colorGamut = surfaceNode->IsLeashWindow() ? surfaceNode->GetFirstLevelNodeColorGamut() :
        surfaceNode->GetColorSpace();
    // When the main screen uses F16 buffer and the window color space is not sRGB,
    // the window freeze capture can use F16 format to optimize performance.
    return (isHDROn || isScRGBEnable) && colorGamut != GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
}

void RSHdrUtil::HandleVirtualScreenHDRStatus(RSScreenRenderNode& node, const sptr<RSScreenManager>& screenManager)
{
    if (node.GetCompositeType() == CompositeType::UNI_RENDER_MIRROR_COMPOSITE) {
        ScreenColorGamut screenColorGamut = RSHdrUtil::GetScreenColorGamut(node, screenManager);
        std::shared_ptr<RSScreenRenderNode> mirrorNode = node.GetMirrorSource().lock();
        if (!mirrorNode) {
            RS_LOGE("RSHdrUtil::HandleVirtualScreenHDRStatus get mirror source failed.");
            return;
        }
        bool mirrorNodeIsHDROn = RSLuminanceControl::Get().IsHdrOn(mirrorNode->GetScreenId()) &&
            mirrorNode->GetDisplayHdrStatus() != HdrStatus::NO_HDR;
        bool hdrCastColorGamut = static_cast<GraphicColorGamut>(screenColorGamut) == GRAPHIC_COLOR_GAMUT_BT2100_HLG;
        bool isNeedHDRCast = mirrorNodeIsHDROn && hdrCastColorGamut;
        RS_LOGD("RSHdrUtil::HandleVirtualScreenHDRStatus HDRCast mirrorNodeIsHDROn: %{public}d, "
            "ColorGamut: %{public}d, GetFirstFrameVirtualScreenInit: %{public}d",
            mirrorNodeIsHDROn, screenColorGamut, node.GetFirstFrameVirtualScreenInit());
        UpdateHDRCastProperties(node, isNeedHDRCast, hdrCastColorGamut);
    } else if (node.GetCompositeType() == CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        ScreenColorGamut screenColorGamut = RSHdrUtil::GetScreenColorGamut(node, screenManager);
        bool expandIsHDROn = node.GetDisplayHdrStatus() != HdrStatus::NO_HDR;
        bool hdrCastColorGamut = static_cast<GraphicColorGamut>(screenColorGamut) == GRAPHIC_COLOR_GAMUT_BT2100_HLG;
        bool isNeedHDRCast = expandIsHDROn && hdrCastColorGamut;
        RS_LOGD("RSHdrUtil::HandleVirtualScreenHDRStatus HDRCast expandIsHDROn: %{public}d, "
            "ColorGamut: %{public}d, GetFirstFrameVirtualScreenInit: %{public}d",
            expandIsHDROn, screenColorGamut, node.GetFirstFrameVirtualScreenInit());
        UpdateHDRCastProperties(node, isNeedHDRCast, hdrCastColorGamut);
    }
}

void RSHdrUtil::UpdateHDRCastProperties(RSScreenRenderNode& node, bool isNeedHDRCast, bool hdrCastColorGamut)
{
    if (node.GetFirstFrameVirtualScreenInit()) {
        node.SetFirstFrameVirtualScreenInit(false);
        if (hdrCastColorGamut) {
            node.SetFixVirtualBuffer10Bit(true);
        }
    }
    if (node.GetFixVirtualBuffer10Bit()) {
        node.SetHDRPresent(isNeedHDRCast);
    }
}

bool RSHdrUtil::IsHDRCast(RSScreenRenderParams* screenParams, BufferRequestConfig& renderFrameConfig)
{
    if (!screenParams) {
        RS_LOGD("RSHdrUtil::IsHDRCast screenParams is nullptr");
        return false;
    }
    // current version fix 1010102 format although is not hdr on
    if (screenParams->GetFixVirtualBuffer10Bit()) {
        renderFrameConfig.format = GRAPHIC_PIXEL_FMT_RGBA_1010102;
        RS_LOGD("RSHdrUtil::IsHDRCast set 1010102 buffer");
        if (screenParams->GetHDRPresent()) {
            renderFrameConfig.colorGamut = GRAPHIC_COLOR_GAMUT_BT2100_HLG;
            return true;
        }
    }
    return false;
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
bool RSHdrUtil::HDRCastProcess(std::shared_ptr<Drawing::Image>& image, Drawing::Brush& paint,
    const Drawing::SamplingOptions& sampling, std::shared_ptr<Drawing::Surface>& surface,
    RSPaintFilterCanvas* canvas)
{
    if (!canvas) {
        RS_LOGE("RSHdrUtil::HDRCastProcess canvas nullptr");
        return false;
    }
    if (!surface) {
        RS_LOGE("RSHdrUtil::HDRCastProcess surface nullptr");
        return false;
    }
    auto gpuContext = canvas->GetGPUContext();
    if (!gpuContext) {
        RS_LOGE("RSHdrUtil::HDRCastProcess gpuContext nullptr");
        return false;
    }
    auto canvasSurface = canvas->GetSurface();
    if (canvasSurface == nullptr) {
        RS_LOGE("RSHdrUtil::HDRCastProcess canvasSurface nullptr");
        return false;
    }
    // Get color space from main screen canvas
    auto colorSpace = canvasSurface->GetImageInfo().GetColorSpace();
    auto format = surface->GetImageInfo().GetColorType();
    auto textureInfo = surface->GetBackendTexture().GetTextureInfo();
    image = std::make_shared<Drawing::Image>();
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::TOP_LEFT;
    Drawing::BitmapFormat info = Drawing::BitmapFormat{ format, Drawing::ALPHATYPE_PREMUL };
    if (!image->BuildFromTexture(*gpuContext, textureInfo, origin, info, colorSpace)) {
        RS_LOGE("RSHdrUtil::HDRCastProcess BuildFromTexture fail");
        return false;
    }
    return SetHDRCastShader(image, paint, sampling);
}

bool RSHdrUtil::SetHDRCastShader(std::shared_ptr<Drawing::Image>& image, Drawing::Brush& paint,
    const Drawing::SamplingOptions& sampling)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    Media::VideoProcessingEngine::ColorSpaceConverterDisplayParameter parameter;
    if (!image) {
        RS_LOGE("RSHdrUtil::SetHDRCastShader image is nullptr.");
        return false;
    }
    auto inputShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, sampling, Drawing::Matrix());
    parameter.inputColorSpace.colorSpaceInfo = RSHDRUtilConst::HDR_CAST_IN_COLORSPACE;
    parameter.inputColorSpace.metadataType = CM_VIDEO_HDR_VIVID;
    parameter.outputColorSpace.colorSpaceInfo = RSHDRUtilConst::HDR_CAST_OUT_COLORSPACE;
    parameter.outputColorSpace.metadataType = CM_VIDEO_HDR_VIVID;
    parameter.tmoNits = RSLuminanceConst::DEFAULT_CAST_HDR_NITS;
    parameter.currentDisplayNits = RSLuminanceConst::DEFAULT_CAST_HDR_NITS;
    parameter.sdrNits = RSLuminanceConst::DEFAULT_CAST_SDR_NITS;
    parameter.disableHdrFloatHeadRoom = true; // no need to apply headroom for virtual screen

    std::shared_ptr<Drawing::ShaderEffect> outputShader;
    auto colorSpaceConverterDisplay = Media::VideoProcessingEngine::ColorSpaceConverterDisplay::Create();
    if (!colorSpaceConverterDisplay) {
        RS_LOGE("RSHdrUtil::SetHDRCastShader VPE create failed");
        return false;
    }
    auto convRet = colorSpaceConverterDisplay->Process(inputShader, outputShader, parameter);
    if (convRet != Media::VideoProcessingEngine::VPE_ALGO_ERR_OK) {
        RS_LOGE("RSHdrUtil::SetHDRCastShader failed with %{public}u.", convRet);
        return false;
    }
    if (outputShader == nullptr) {
        RS_LOGE("RSHdrUtil::SetHDRCastShader outputShader is null.");
        return false;
    }
    paint.SetShaderEffect(outputShader);
    RS_LOGD("RSHdrUtil::SetHDRCastShader succeed to set output shader.");
    return true;
}

GSError RSHdrUtil::EraseHDRMetadataKey(std::unique_ptr<RSRenderFrame>& renderFrame)
{
    if (renderFrame == nullptr) {
        RS_LOGD("RSHdrUtil::EraseHDRMetadataKey renderFrame is null.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    auto rsSurface = renderFrame->GetSurface();
    if (rsSurface == nullptr) {
        RS_LOGD("RSHdrUtil::EraseHDRMetadataKey surface is null.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    auto buffer = rsSurface->GetCurrentBuffer();
    if (buffer == nullptr) {
        RS_LOGD("RSHdrUtil::EraseHDRMetadataKey buffer is null.");
        return GSERROR_NO_BUFFER;
    }
    auto ret = buffer->EraseMetadataKey(ATTRKEY_HDR_STATIC_METADATA);
    if (ret != GSERROR_OK) {
        RS_LOGD("RSHdrUtil::EraseHDRMetadataKey ATTRKEY_HDR_STATIC_METADATA ret = %{public}d", ret);
        return ret;
    }
    ret = buffer->EraseMetadataKey(ATTRKEY_HDR_DYNAMIC_METADATA);
    if (ret != GSERROR_OK) {
        RS_LOGD("RSHdrUtil::EraseHDRMetadataKey ATTRKEY_HDR_DYNAMIC_METADATA ret = %{public}d", ret);
    }
    return ret;
}

GSError RSHdrUtil::SetMetadata(const HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorspaceInfo,
    std::unique_ptr<RSRenderFrame>& renderFrame)
{
    RS_TRACE_NAME_FMT("RSHdrUtil::SetMetadata HDRCast");
    if (renderFrame == nullptr) {
        RS_LOGD("RSHdrUtil::SetMetadata renderFrame is null.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    auto rsSurface = renderFrame->GetSurface();
    if (rsSurface == nullptr) {
        RS_LOGD("RSHdrUtil::SetMetadata surface is null.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    auto buffer = rsSurface->GetCurrentBuffer();
    if (buffer == nullptr) {
        RS_LOGD("RSHdrUtil::SetMetadata buffer is null.");
        return GSERROR_NO_BUFFER;
    }
    using namespace HDI::Display::Graphic::Common::V1_0;
    HdrStaticMetadata staticMetadata;
    staticMetadata.cta861.maxContentLightLevel = RSLuminanceConst::DEFAULT_CAST_HDR_NITS;
    auto ret = MetadataHelper::SetHDRStaticMetadata(buffer, staticMetadata);
    if (ret != GSERROR_OK) {
        RS_LOGD("RSHdrUtil::SetMetadata SetHDRStaticMetadata failed %{public}d", ret);
        return ret;
    }
    ret = MetadataHelper::SetHDRDynamicMetadata(buffer, HDR_VIVID_METADATA);
    if (ret != GSERROR_OK) {
        RS_LOGD("RSHdrUtil::SetMetadata SetHDRDynamicMetadata failed %{public}d", ret);
        return ret;
    }
    ret = MetadataHelper::SetColorSpaceInfo(buffer, colorspaceInfo);
    if (ret != GSERROR_OK) {
        RS_LOGD("RSHdrUtil::SetMetadata SetColorSpaceInfo failed %{public}d", ret);
        return ret;
    }
    RS_LOGD("RSHdrUtil::SetMetadata SetMetadata end");
    return ret;
}

GSError RSHdrUtil::SetMetadata(SurfaceBuffer* buffer,
    const HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorspaceInfo,
    const HDI::Display::Graphic::Common::V1_0::CM_HDR_Metadata_Type& value)
{
    if (buffer == nullptr) {
        RS_LOGE("RSHdrUtil::SetMetadata failed buffer nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }
    std::vector<uint8_t> metadataType;
    std::vector<uint8_t> colorSpaceMetadata;
    metadataType.resize(sizeof(value));
    colorSpaceMetadata.resize(sizeof(colorspaceInfo));
    errno_t ret = memcpy_s(metadataType.data(), metadataType.size(), &value, sizeof(value));
    if (ret != EOK) {
        RS_LOGE("RSHdrUtil::SetMetadata metadataType memcpy_s failed error number: %{public}d", ret);
        return GSERROR_INVALID_OPERATING;
    }
    ret = memcpy_s(colorSpaceMetadata.data(), colorSpaceMetadata.size(), &colorspaceInfo, sizeof(colorspaceInfo));
    if (ret != EOK) {
        RS_LOGE("RSHdrUtil::SetMetadata colorSpaceMetadata memcpy_s failed error number: %{public}d", ret);
        return GSERROR_INVALID_OPERATING;
    }
    GSError setValueErr =
        buffer->SetMetadata(Media::VideoProcessingEngine::ATTRKEY_HDR_METADATA_TYPE, metadataType);
    if (setValueErr != GSERROR_OK) {
        RS_LOGE("RSHdrUtil::SetMetadata set metadataType failed with %{public}d", setValueErr);
        return setValueErr;
    }
    GSError setColorSpaceErr =
        buffer->SetMetadata(Media::VideoProcessingEngine::ATTRKEY_COLORSPACE_INFO, colorSpaceMetadata);
    if (setColorSpaceErr != GSERROR_OK) {
        RS_LOGE("RSHdrUtil::SetMetadata set colorSpaceMetadata failed with %{public}d", setColorSpaceErr);
        return setColorSpaceErr;
    }
    RS_LOGD("RSHdrUtil::SetMetadata set type %{public}d,"
        " set colorSpace %{public}d-%{public}d-%{public}d-%{public}d", value,
        static_cast<int>(colorspaceInfo.primaries), static_cast<int>(colorspaceInfo.transfunc),
        static_cast<int>(colorspaceInfo.matrix), static_cast<int>(colorspaceInfo.range));
    return GSERROR_OK;
}
#endif

} // namespace Rosen
} // namespace OHOS