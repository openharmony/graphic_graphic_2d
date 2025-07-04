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
#include "hdi_layer_info.h"
#include "metadata_helper.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "platform/common/rs_log.h"
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "render/rs_colorspace_convert.h"
#endif
#include "v2_2/cm_color_space.h"

namespace OHOS {
namespace Rosen {

constexpr float DEFAULT_HDR_RATIO = 1.0f;
constexpr float DEFAULT_SCALER = 1000.0f / 203.0f;
constexpr float GAMMA2_2 = 2.2f;
constexpr size_t MATRIX_SIZE = 9;
static const std::vector<float> DEFAULT_MATRIX = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
static std::shared_ptr<Drawing::RuntimeEffect> hdrHeadroomShaderEffect_;
std::unordered_set<uint8_t> aihdrMetadataTypeSet = {
    HDI::Display::Graphic::Common::V2_2::CM_VIDEO_AI_HDR,
    HDI::Display::Graphic::Common::V2_2::CM_VIDEO_AI_HDR_HIGH_LIGHT,
    HDI::Display::Graphic::Common::V2_2::CM_VIDEO_AI_HDR_COLOR_ENHANCE
};

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
    std::vector<uint8_t> metadataType{};
    if ((surfaceBuffer->GetMetadata(ATTRKEY_HDR_METADATA_TYPE, metadataType)
        == GSERROR_OK) && (metadataType.size() > 0) &&
        (aihdrMetadataTypeSet.find(metadataType[0]) != aihdrMetadataTypeSet.end())) {
        return HdrStatus::AI_HDR_VIDEO;
    }
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
    auto screenNode = context->GetNodeMap().GetRenderNode<RSScreenRenderNode>(surfaceNode.GetScreenNodeId());
    if (!screenNode) {
        RS_LOGE("RSHdrUtil::UpdateSurfaceNodeNit screenNode is null");
        return;
    }

    float brightnessFactor = screenNode->GetRenderProperties().GetHDRBrightnessFactor();
    if (ROSEN_NE(surfaceNode.GetHDRBrightnessFactor(), brightnessFactor)) {
        RS_LOGD("RSHdrUtil::UpdateSurfaceNodeNit GetHDRBrightnessFactor: %{public}f, "
            "displayNode brightnessFactor: %{public}f, nodeId: %{public}" PRIu64 "",
            surfaceNode.GetHDRBrightnessFactor(), brightnessFactor, surfaceNode.GetId());
        surfaceNode.SetHDRBrightnessFactor(brightnessFactor);
        surfaceNode.SetContentDirty();
    }
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

void RSHdrUtil::UpdatePixelFormatAfterHwcCalc(RSScreenRenderNode& node)
{
    const auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
    for (const auto& selfDrawingNode : selfDrawingNodes) {
        if (!selfDrawingNode || !selfDrawingNode->GetAncestorScreenNode().lock()) {
            RS_LOGD("RSHdrUtil::UpdatePixelFormatAfterHwcCalc selfDrawingNode or ancestoreNode is nullptr");
            continue;
        }
        auto ancestor = selfDrawingNode->GetAncestorScreenNode().lock()->ReinterpretCastTo<RSScreenRenderNode>();
        if (ancestor != nullptr && node.GetId() == ancestor->GetId()) {
            CheckPixelFormatWithSelfDrawingNode(*selfDrawingNode, node);
        }
    }
}

void RSHdrUtil::CheckPixelFormatWithSelfDrawingNode(RSSurfaceRenderNode& surfaceNode,
    RSScreenRenderNode& displayNode)
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
        RS_LOGD("RSHdrUtil::CheckPixelFormatWithSelfDrawingNode HDRService surfaceNode %{public}s is HDR",
            surfaceNode.GetName().c_str());
    }
}

bool RSHdrUtil::GetRGBA1010108Enabled()
{
    static bool isDDGR = system::GetParameter("persist.sys.graphic.GpuApitype", "1") == "2";
    static bool rgba1010108 = system::GetBoolParameter("const.graphics.rgba_1010108_supported", false);
    return isDDGR && rgba1010108;
}

void RSHdrUtil::SetHDRParam(RSSurfaceRenderNode& node, bool flag)
{
    auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetFirstLevelNode());
    if (firstLevelNode != nullptr && node.GetFirstLevelNodeId() != node.GetId()) {
        firstLevelNode->SetHDRPresent(flag);
    }
    node.SetHDRPresent(flag);
}

std::shared_ptr<Drawing::ShaderEffect> RSHdrUtil::MakeHdrHeadroomShader(float hrRatio,
    std::shared_ptr<Drawing::ShaderEffect> imageShader)
{
    static constexpr char prog[] = R"(
        uniform half hrRatio;
        uniform shader imageShader;

        mat3 Mat_RGB_P3toBT709 = mat3(
            1.224939741445,  -0.042056249524,  -0.019637688845,
            -0.224939599120,   1.042057784075,  -0.078636557327,
            -0.000001097215,   0.000000329788,   1.098273664879
        );

        mat3 MAT_RGB_P3tosRGB_VPE = mat3(1.22494, -0.0420569, -0.0196376,
                            -0.22494, 1.04206, -0.078636,
                            0.0, 0.0, 1.09827);
        
        half4 main(float2 coord)
        {
            vec4 color = imageShader.eval(coord);
            vec3 linearColor = sign(color.rgb) * pow(abs(color.rgb), vec3(2.2f));
            linearColor = MAT_RGB_P3tosRGB_VPE * linearColor;
            vec3 hdr = sign(linearColor) * pow(abs(linearColor), vec3(1.0f / 2.2f));
            hdr = hdr * hrRatio;
            return vec4(hdr, 1.0);
        }
    )";
    if (hdrHeadroomShaderEffect_ == nullptr) {
        hdrHeadroomShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (hdrHeadroomShaderEffect_ == nullptr) {
            ROSEN_LOGE("MakeDynamicDimShader::RuntimeShader effect error\n");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::ShaderEffect> children[] = {imageShader};
    size_t childCount = 1;
    auto data = std::make_shared<Drawing::Data>();
    data->BuildWithCopy(&hrRatio, sizeof(hrRatio));

    return hdrHeadroomShaderEffect_->MakeShader(data, children, childCount, nullptr, false);
}

void RSHdrUtil::HandleVirtualScreenHDRStatus(RSScreenRenderNode& node, const sptr<RSScreenManager>& screenManager)
{
    if (node.GetCompositeType() == CompositeType::UNI_RENDER_MIRROR_COMPOSITE) {
        ScreenColorGamut screenColorGamut;
        if (screenManager->GetScreenColorGamut(node.GetScreenId(), screenColorGamut) != SUCCESS) {
            RS_LOGD("RSHdrUtil::HandleVirtualScreenHDRStatus get screen color gamut failed.");
            return;
        }
        std::shared_ptr<RSScreenRenderNode> mirrorNode = node.GetMirrorSource().lock();
        if (!mirrorNode) {
            RS_LOGE("RSHdrUtil::HandleVirtualScreenHDRStatus get mirror source failed.");
            return;
        }
        bool mirrorNodeIsHdrOn = RSLuminanceControl::Get().IsHdrOn(mirrorNode->GetScreenId()) &&
            mirrorNode->GetDisplayHdrStatus() != HdrStatus::NO_HDR;
        bool isNeedHDRCast = (node.IsFirstFrameOfInit() || node.GetEnabledHDRCast()) && mirrorNodeIsHdrOn &&
            static_cast<GraphicColorGamut>(screenColorGamut) == GRAPHIC_COLOR_GAMUT_BT2100_HLG;
        RS_LOGD("RSHdrUtil::HandleVirtualScreenHDRStatus HDRCast mirrorNodeIsHdrOn: %{public}d, "
            "ColorGamut: %{public}d, IsFirstFrameOfInit: %{public}d, GetEnabledHDRCast: %{public}d",
            mirrorNodeIsHdrOn, screenColorGamut, node.IsFirstFrameOfInit(), node.GetEnabledHDRCast());
        if (isNeedHDRCast) {
            node.SetHDRPresent(true);
            node.SetEnabledHDRCast(true);
        }
        node.SetFirstFrameOfInit(false);
    } else if (node.GetCompositeType() == CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        ScreenColorGamut screenColorGamut;
        if (screenManager->GetScreenColorGamut(node.GetScreenId(), screenColorGamut) != SUCCESS) {
            RS_LOGD("RSHdrUtil::HandleVirtualScreenHDRStatus get screen color gamut failed.");
            return;
        }
        bool isNeedHDRCast = (node.IsFirstFrameOfInit() || node.GetEnabledHDRCast()) &&
            static_cast<GraphicColorGamut>(screenColorGamut) == GRAPHIC_COLOR_GAMUT_BT2100_HLG;
        RS_LOGD("RSHdrUtil::HandleVirtualScreenHDRStatus HDRCast ColorGamut: %{public}d, "
            "IsFirstFrameOfInit: %{public}d, GetEnabledHDRCast: %{public}d",
            screenColorGamut, node.IsFirstFrameOfInit(), node.GetEnabledHDRCast());
        if (isNeedHDRCast) {
            node.SetHDRPresent(true);
            node.SetEnabledHDRCast(true);
        }
        node.SetFirstFrameOfInit(false);
    }
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
GSError RSHdrUtil::SetMetadata(SurfaceBuffer* buffer,
    const HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorspaceInfo, uint32_t value)
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