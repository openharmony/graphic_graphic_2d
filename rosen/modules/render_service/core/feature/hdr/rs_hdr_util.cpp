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
#include "platform/common/rs_log.h"
#include "utils/system_properties.h"
#include "v2_2/cm_color_space.h"
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "render/rs_colorspace_convert.h"
#endif

namespace OHOS {
namespace Rosen {

constexpr float DEFAULT_HDR_RATIO = 1.0f;
constexpr float DEFAULT_SCALER = 1000.0f / 203.0f;
constexpr float GAMMA2_2 = 2.2f;
constexpr size_t MATRIX_SIZE = 9;
static const std::vector<float> DEFAULT_MATRIX = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
const std::vector<uint8_t> HDR_VIVID_METADATA = {
    1, 0, 23, 93, 111, 186, 221, 240, 26, 189, 83, 29, 128, 0, 82, 142, 25, 156, 3,
    198, 204, 179, 47, 236, 32, 190, 143, 163, 252, 16, 93, 185, 106, 159, 0, 10,
    81, 199, 178, 80, 255, 217, 150, 101, 201, 144, 114, 73, 65, 127, 160, 0, 0
};
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
        if (!selfDrawingNode) {
            RS_LOGD("RSHdrUtil::UpdatePixelFormatAfterHwcCalc selfDrawingNode is nullptr");
            continue;
        }
        auto ancestorPtr = selfDrawingNode->GetAncestorScreenNode().lock();
        if (!ancestorPtr) {
            RS_LOGD("RSHdrUtil::UpdatePixelFormatAfterHwcCalc ancestor is nullptr");
            continue;
        }
        auto ancestor = ancestorPtr->ReinterpretCastTo<RSScreenRenderNode>();
        if (!ancestor) {
            RS_LOGD("RSHdrUtil::UpdatePixelFormatAfterHwcCalc ancestor is not screen node");
            continue;
        }
        if (node.GetId() == ancestor->GetId()) {
            CheckPixelFormatWithSelfDrawingNode(*selfDrawingNode, node);
        }
        bool needSetExistHWCNode = !ancestor->GetExistHWCNode() && selfDrawingNode->IsOnTheTree() &&
            !selfDrawingNode->IsHardwareForcedDisabled() && node.GetId() == ancestor->GetId();
        if (needSetExistHWCNode) {
            RS_LOGD("RSHdrUtil::UpdatePixelFormatAfterHwcCalc there exist HWCNode");
            ancestor->SetExistHWCNode(true);
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
    UpdateSurfaceNodeNit(surfaceNode, screenId);
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
        SetHDRParam(screenNode, surfaceNode, true);
        if (screenNode.GetIsLuminanceStatusChange() && !screenNode.GetForceCloseHdr()) {
            surfaceNode.SetContentDirty();
        }
        RS_LOGD("RSHdrUtil::CheckPixelFormatWithSelfDrawingNode HDRService surfaceNode %{public}s is HDR",
            surfaceNode.GetName().c_str());
    }
}

bool RSHdrUtil::GetRGBA1010108Enabled()
{
    static bool isDDGR = Drawing::SystemProperties::GetSystemGraphicGpuType() == GpuApiType::DDGR;
    static bool rgba1010108 = system::GetBoolParameter("const.graphics.rgba_1010108_supported", false);
    static bool debugSwitch = system::GetBoolParameter("persist.sys.graphic.rgba_1010108.enabled", true);
    return isDDGR && rgba1010108 && debugSwitch;
}

void RSHdrUtil::SetHDRParam(RSScreenRenderNode& screenNode, RSSurfaceRenderNode& node, bool flag)
{
    if (screenNode.GetForceCloseHdr()) {
        RS_LOGD("RSHdrUtil::SetHDRParam curScreenNode forceclosehdr.");
        return;
    }
    auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetFirstLevelNode());
    if (firstLevelNode != nullptr && node.GetFirstLevelNodeId() != node.GetId()) {
        firstLevelNode->SetHDRPresent(flag);
    }
    node.SetHDRPresent(flag);
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
    auto screenNode = std::static_pointer_cast<RSScreenRenderNode>(surfaceNode->GetAncestorScreenNode().lock());
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
    image = std::make_shared<Drawing::Image>();
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::TOP_LEFT;
    Drawing::BitmapFormat info = Drawing::BitmapFormat{ Drawing::COLORTYPE_RGBA_F16, Drawing::ALPHATYPE_PREMUL };
    image->BuildFromTexture(*gpuContext, surface->GetBackendTexture().GetTextureInfo(),
        origin, info, colorSpace);
    return SetHDRCastShader(image, paint, sampling);
}

bool RSHdrUtil::SetHDRCastShader(std::shared_ptr<Drawing::Image>& image, Drawing::Brush& paint,
    const Drawing::SamplingOptions& sampling)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    Media::VideoProcessingEngine::ColorSpaceConverterDisplayParameter parameter;
    if (!image) {
        RS_LOGE("RSHdrUtil::SetHDRCastShader image is nullptr");
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
        RS_LOGE("RSHDRUtil::SetHDRCastShader VPE create failed");
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

GSError RSHdrUtil::SetMetadata(const HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorspaceInfo,
    std::unique_ptr<RSRenderFrame>& renderFrame)
{
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
    Media::VideoProcessingEngine::HdrStaticMetadata staticMetadata;
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
    std::vector<uint8_t> metadata;
    metadata.resize(sizeof(colorspaceInfo));
    errno_t errRet = memcpy_s(metadata.data(), metadata.size(), &colorspaceInfo, sizeof(colorspaceInfo));
    if (errRet != EOK) {
        RS_LOGD("RSHdrUtil::SetMetadata colorspace failed %{public}d", errRet);
        return GSERROR_OUT_OF_RANGE;
    }
    RS_LOGD("RSHdrUtil::SetMetadata SetMetadata end");
    // Metadata will overwrite the metadata in SetColorSpaceForMetadata, BT2020 is wider than P3
    return buffer->SetMetadata(Media::VideoProcessingEngine::ATTRKEY_COLORSPACE_INFO, metadata);
}

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