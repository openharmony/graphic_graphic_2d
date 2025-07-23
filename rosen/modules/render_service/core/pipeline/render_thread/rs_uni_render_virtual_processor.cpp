/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_uni_render_virtual_processor.h"

#include <ctime>
#include <parameters.h>

#include "metadata_helper.h"
#include "sync_fence.h"

#include "common/rs_optional_trace.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_logical_display_render_node_drawable.h"
#include "graphic_feature_param_manager.h"
#include "params/rs_logical_display_render_params.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"
#include "rs_uni_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "string_utils.h"

namespace OHOS {
namespace Rosen {
bool RSUniRenderVirtualProcessor::InitForRenderThread(DrawableV2::RSScreenRenderNodeDrawable& screenDrawable,
    std::shared_ptr<RSBaseRenderEngine> renderEngine)
{
    if (!RSProcessor::InitForRenderThread(screenDrawable, renderEngine)) {
        return false;
    }

    // Do expand screen if the mirror id is invalid.
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        return false;
    }
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable.GetRenderParams().get());
    if (!params) {
        return false;
    }

    virtualScreenId_ = params->GetScreenId();
    VirtualScreenStatus screenStatus = screenManager->GetVirtualScreenStatus(virtualScreenId_);
    if (screenStatus == VIRTUAL_SCREEN_PAUSE) {
        RS_LOGD("RSUniRenderVirtualProcessor::Init screenStatus is pause");
        return false;
    }

    scaleMode_ = screenManager->GetScaleMode(virtualScreenId_);
    const auto& virtualScreenInfo = params->GetScreenInfo();
    enableVisibleRect_ = virtualScreenInfo.enableVisibleRect;
    canvasRotation_ = screenManager->GetCanvasRotation(virtualScreenId_);
    if (enableVisibleRect_) {
        const auto& rect = screenManager->GetMirrorScreenVisibleRect(virtualScreenId_);
        visibleRect_ = Drawing::RectI(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
        // not support rotation for MirrorScreen visibleRect
        canvasRotation_ = screenManager->IsVisibleRectSupportRotation(virtualScreenId_);
    }
    bool mirrorScreenHDR = false;
    bool expandScreenHDR = false;
    renderFrameConfig_.colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    auto mirroredScreenDrawable =
        std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(params->GetMirrorSourceDrawable().lock());
    if (mirroredScreenDrawable) {
        auto childDrawables = params->GetDisplayDrawables();
        if (childDrawables.empty() || childDrawables.front() == nullptr) {
            RS_LOGE("RSUniRenderVirtualProcessor::InitForRenderThread: no child display in mirror screen");
            return false;
        }
        auto displayDrawable = childDrawables.front();
        auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable->GetRenderParams().get());
        auto mirroredDisplayDrawable = std::static_pointer_cast<DrawableV2::RSLogicalDisplayRenderNodeDrawable>(
            displayParams->GetMirrorSourceDrawable().lock());
        auto mirroredDisplayParams = static_cast<RSLogicalDisplayRenderParams*>(
            mirroredDisplayDrawable->GetRenderParams().get());
        auto mirroredScreenParams = static_cast<RSScreenRenderParams*>(mirroredScreenDrawable->GetRenderParams().get());
        screenRotation_ = mirroredDisplayParams->GetScreenRotation();
        screenCorrection_ = screenManager->GetScreenCorrection(mirroredDisplayParams->GetScreenId());
        if (params->GetNewColorSpace() != GRAPHIC_COLOR_GAMUT_SRGB &&
            mirroredScreenParams->GetNewColorSpace() != GRAPHIC_COLOR_GAMUT_SRGB) {
            renderFrameConfig_.colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
            RS_LOGD("RSUniRenderVirtualProcessor::Init Set virtual screen buffer colorGamut to P3.");
        }
        mirrorScreenHDR = RSHdrUtil::IsHDRCast(params, renderFrameConfig_);
        RS_LOGD("RSUniRenderVirtualProcessor::Init HDRCast mirrorScreenHDR: %{public}d", mirrorScreenHDR);
    } else {
        if (params->GetNewColorSpace() != GRAPHIC_COLOR_GAMUT_SRGB) {
            renderFrameConfig_.colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
        }
        expandScreenHDR = RSHdrUtil::IsHDRCast(params, renderFrameConfig_);
        RS_LOGD("RSUniRenderVirtualProcessor::Init HDRCast expandScreenHDR: %{public}d", expandScreenHDR);
    }

    SetVirtualScreenSize(screenDrawable, screenManager);

    renderFrameConfig_.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;
    FrameContextConfig frameContextConfig = {false, false};
    frameContextConfig.isVirtual = true;
    frameContextConfig.timeOut = 0;

    producerSurface_ = screenManager->GetProducerSurface(virtualScreenId_);
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::Init for Screen(id %{public}" PRIu64 "): ProducerSurface is null!",
            virtualScreenId_);
        return false;
    }
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        renderFrame_ = renderEngine_->RequestFrame(producerSurface_, renderFrameConfig_, forceCPU_, false,
            frameContextConfig);
    }
#endif
    if (renderFrame_ == nullptr) {
        uint64_t pSurfaceUniqueId = producerSurface_->GetUniqueId();
        auto rsSurface = screenDrawable.GetVirtualSurface(pSurfaceUniqueId);
        if (rsSurface == nullptr || screenManager->GetAndResetVirtualSurfaceUpdateFlag(virtualScreenId_)) {
            RS_LOGD("RSUniRenderVirtualProcessor::Init Make rssurface from producer Screen(id %{public}" PRIu64 ")",
                virtualScreenId_);
            RS_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::Init Make rssurface from producer Screen(id %" PRIu64 ")",
                virtualScreenId_);
            rsSurface = renderEngine_->MakeRSSurface(producerSurface_, forceCPU_);
            screenDrawable.SetVirtualSurface(rsSurface, pSurfaceUniqueId);
        }
        renderFrame_ = renderEngine_->RequestFrame(
            std::static_pointer_cast<RSSurfaceOhos>(rsSurface), renderFrameConfig_, forceCPU_, false,
            frameContextConfig);
    }
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::Init for Screen(id %{public}" PRIu64 "): RenderFrame is null!",
            virtualScreenId_);
        return false;
    }

    RS_LOGD("RSUniRenderVirtualProcessor::Init, RequestFrame succeed.");
    RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::Init, RequestFrame succeed, colorSpace: %d.",
        renderFrameConfig_.colorGamut);
    uint64_t pSurfaceUniqueId = producerSurface_->GetUniqueId();
    auto rsSurface = screenDrawable.GetVirtualSurface(pSurfaceUniqueId);
    if (rsSurface != nullptr && SetColorSpaceForMetadata(rsSurface->GetColorSpace()) != GSERROR_OK) {
        RS_LOGD("RSUniRenderVirtualProcessor::SetColorSpaceForMetadata failed.");
    }
#ifdef USE_VIDEO_PROCESSING_ENGINE
    if (mirrorScreenHDR || expandScreenHDR) {
        if (RSHdrUtil::SetMetadata(RSHDRUtilConst::HDR_CAST_OUT_COLORSPACE, renderFrame_) != GSERROR_OK) {
            RS_LOGD("RSUniRenderVirtualProcessor::Init SetMetadata failed");
        }
    }
#endif
    canvas_ = renderFrame_->GetCanvas();
    if (canvas_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::Init for Screen(id %{public}" PRIu64 "): Canvas is null!",
            virtualScreenId_);
        return false;
    }

    return true;
}

bool RSUniRenderVirtualProcessor::UpdateMirrorInfo(DrawableV2::RSLogicalDisplayRenderNodeDrawable& displayDrawable)
{
    if (!RSProcessor::UpdateMirrorInfo(displayDrawable)) {
        return false;
    }
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable.GetRenderParams().get());
    if (!params) {
        return false;
    }
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        return false;
    }
    auto mirroredDisplayDrawable = std::static_pointer_cast<DrawableV2::RSLogicalDisplayRenderNodeDrawable>(
        params->GetMirrorSourceDrawable().lock());
    if (mirroredDisplayDrawable) {
        auto& mirroredParams = mirroredDisplayDrawable->GetRenderParams();
        if (mirroredParams) {
            auto mirroredDisplayParams = static_cast<RSLogicalDisplayRenderParams*>(mirroredParams.get());
            auto mainScreenInfo = screenManager->QueryScreenInfo(mirroredDisplayParams->GetScreenId());
            mirroredScreenWidth_ = mirroredDisplayParams->GetBounds().GetWidth();
            mirroredScreenHeight_ = mirroredDisplayParams->GetBounds().GetHeight();
            if (mirroredDisplayParams->GetNodeRotation() == ScreenRotation::ROTATION_90 ||
                mirroredDisplayParams->GetNodeRotation() == ScreenRotation::ROTATION_270) {
                std::swap(mirroredScreenWidth_, mirroredScreenHeight_);
            }
            if (mainScreenInfo.isSamplingOn) {
                mirroredScreenWidth_ *= mainScreenInfo.samplingScale;
                mirroredScreenHeight_ *= mainScreenInfo.samplingScale;
            }
            mirroredTranslateX_ = mirroredDisplayParams->GetOffsetX();
            mirroredTranslateY_ = mirroredDisplayParams->GetOffsetY();
            isMirroredDisplayRotating_ = mirroredDisplayParams->IsRotationChanged();
        }
    }
    return true;
}

void RSUniRenderVirtualProcessor::CanvasInit(DrawableV2::RSLogicalDisplayRenderNodeDrawable& displayDrawable)
{
    // Save the initial canvas state
    canvas_->Save();
    if (displayDrawable.IsFirstTimeToProcessor() || canvasRotation_ || autoBufferRotation_) {
        if (displayDrawable.IsFirstTimeToProcessor()) {
            RS_LOGI("RSUniRenderVirtualProcessor::FirstInit, id: %{public}" PRIu64 ", "
                "screen(%{public}f, %{public}f, %{public}f, %{public}f), "
                "rotation: %{public}d, correction: %{public}d, needRotation: %{public}d, scaleMode: %{public}d, "
                "autoBufferRotation: %{public}d", virtualScreenId_, virtualScreenWidth_, virtualScreenHeight_,
                mirroredScreenWidth_, mirroredScreenHeight_, screenRotation_, screenCorrection_, canvasRotation_,
                scaleMode_, autoBufferRotation_);
        }
        displayDrawable.SetOriginScreenRotation(screenRotation_);
    }
    auto rotationDiff =
        static_cast<int>(displayDrawable.GetOriginScreenRotation()) - static_cast<int>(screenCorrection_);
    auto rotationAngle = static_cast<ScreenRotation>((rotationDiff + SCREEN_ROTATION_NUM) % SCREEN_ROTATION_NUM);
    OriginScreenRotation(rotationAngle, renderFrameConfig_.width, renderFrameConfig_.height);

    RS_LOGD("RSUniRenderVirtualProcessor::CanvasInit, id: %{public}" PRIu64 ", "
        "screen(%{public}f, %{public}f, %{public}f, %{public}f), "
        "rotation: %{public}d, correction: %{public}d, needRotation: %{public}d, rotationAngle: %{public}d, "
        "scaleMode: %{public}d, autoBufferRotation: %{public}d",
        virtualScreenId_, virtualScreenWidth_, virtualScreenHeight_, mirroredScreenWidth_, mirroredScreenHeight_,
        screenRotation_, screenCorrection_, canvasRotation_, rotationAngle, scaleMode_, autoBufferRotation_);
}

int32_t RSUniRenderVirtualProcessor::GetBufferAge() const
{
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::GetBufferAge renderFrame_ is null.");
        return 0;
    }
    return renderFrame_->GetBufferAge();
}

GSError RSUniRenderVirtualProcessor::SetColorSpaceForMetadata(GraphicColorGamut colorSpace)
{
    if (renderFrame_ == nullptr) {
        RS_LOGD("RSUniRenderVirtualProcessor::SetColorSpaceForMetadata renderFrame is null.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    auto rsSurface = renderFrame_->GetSurface();
    if (rsSurface == nullptr) {
        RS_LOGD("RSUniRenderVirtualProcessor::SetColorSpaceForMetadata surface is null.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    auto buffer = rsSurface->GetCurrentBuffer();
    if (buffer == nullptr) {
        RS_LOGD("RSUniRenderVirtualProcessor::SetColorSpaceForMetadata buffer is null.");
        return GSERROR_NO_BUFFER;
    }
    using namespace HDI::Display::Graphic::Common::V1_0;
    auto iter = COLORSPACE_TYPE.find(colorSpace);
    if (iter == COLORSPACE_TYPE.end()) {
        return GSERROR_OK;
    }
    CM_ColorSpaceInfo colorSpaceInfo;
    GSError ret = MetadataHelper::ConvertColorSpaceTypeToInfo(iter->second, colorSpaceInfo);
    if (ret != GSERROR_OK) {
        RS_LOGD("RSUniRenderVirtualProcessor::SetColorSpaceForMetadata ConvertColorSpaceTypeToInfo failed.");
        return ret;
    }
    std::vector<uint8_t> colorSpaceVec;
    if (MetadataHelper::ConvertMetadataToVec(colorSpaceInfo, colorSpaceVec) != GSERROR_OK) {
        RS_LOGD("RSUniRenderVirtualProcessor::SetColorSpaceForMetadata ConvertMetadataToVec failed.");
        return GSERROR_API_FAILED;
    }
    return buffer->SetMetadata(ATTRKEY_COLORSPACE_INFO, colorSpaceVec);
}

void RSUniRenderVirtualProcessor::SetDirtyInfo(const std::vector<RectI>& damageRegion)
{
    if (renderFrame_ == nullptr) {
        RS_LOGW("RSUniRenderVirtualProcessor::SetDirtyInfo renderFrame_ is null.");
        return;
    }
    renderFrame_->SetDamageRegion(damageRegion);
    if (SetRoiRegionToCodec(damageRegion) != GSERROR_OK) {
        RS_LOGD("RSUniRenderVirtualProcessor::SetDirtyInfo SetRoiRegionToCodec failed.");
    }
}

GSError RSUniRenderVirtualProcessor::SetRoiRegionToCodec(const std::vector<RectI>& damageRegion)
{
    if (renderFrame_ == nullptr) {
        RS_LOGD("RSUniRenderVirtualProcessor::SetRoiRegionToCodec renderFrame is null.");
        return GSERROR_INVALID_ARGUMENTS;
    }

    auto& rsSurface = renderFrame_->GetSurface();
    if (rsSurface == nullptr) {
        RS_LOGD("RSUniRenderVirtualProcessor::SetRoiRegionToCodec surface is null.");
        return GSERROR_INVALID_ARGUMENTS;
    }

    auto buffer = rsSurface->GetCurrentBuffer();
    if (buffer == nullptr) {
        RS_LOGD("RSUniRenderVirtualProcessor::SetRoiRegionToCodec buffer is null, not support get surfacebuffer.");
        return GSERROR_NO_BUFFER;
    }

    RoiRegions roiRegions;
    const RectI screenRect{0, 0, originalVirtualScreenWidth_, originalVirtualScreenHeight_};
    if (damageRegion.size() <= ROI_REGIONS_MAX_CNT) {
        for (auto rect : damageRegion) {
            rect = rect.IntersectRect(screenRect);
            if (!rect.IsEmpty()) {
                RoiRegionInfo region = RoiRegionInfo{rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight()};
                roiRegions.regions[roiRegions.regionCnt++] = region;
            }
        }
    } else {
        RectI mergedRect;
        for (auto& rect : damageRegion) {
            mergedRect = mergedRect.JoinRect(rect);
        }
        mergedRect = mergedRect.IntersectRect(screenRect);
        if (!mergedRect.IsEmpty()) {
            RoiRegionInfo region = RoiRegionInfo{mergedRect.GetLeft(), mergedRect.GetTop(),
                mergedRect.GetWidth(), mergedRect.GetHeight()};
            roiRegions.regions[roiRegions.regionCnt++] = region;
        }
    }

    std::vector<uint8_t> roiRegionsVec;
    auto ret = MetadataHelper::ConvertMetadataToVec(roiRegions, roiRegionsVec);
    if (ret != GSERROR_OK) {
        RS_LOGD("RSUniRenderVirtualProcessor::SetRoiRegionToCodec ConvertMetadataToVec failed.");
        return ret;
    }
    return buffer->SetMetadata(GrallocBufferAttr::GRALLOC_BUFFER_ATTR_BUFFER_ROI_INFO, roiRegionsVec);
}

void RSUniRenderVirtualProcessor::OriginScreenRotation(ScreenRotation screenRotation, float width, float height)
{
    if (screenRotation == ScreenRotation::ROTATION_0) {
        return;
    } else if (screenRotation == ScreenRotation::ROTATION_90) {
        canvas_->Translate(width / 2.0f, height / 2.0f);
        canvas_->Rotate(90, 0, 0); // 90 degrees
        canvas_->Translate(-(height / 2.0f), -(width / 2.0f));
    } else if (screenRotation == ScreenRotation::ROTATION_180) {
        canvas_->Rotate(180, width / 2.0f, height / 2.0f); // 180 degrees
    } else if (screenRotation == ScreenRotation::ROTATION_270) {
        canvas_->Translate(width / 2.0f, height / 2.0f);
        canvas_->Rotate(270, 0, 0); // 270 degrees
        canvas_->Translate(-(height / 2.0f), -(width / 2.0f));
    }
}

void RSUniRenderVirtualProcessor::CalculateTransform(ScreenRotation rotation)
{
    if (canvas_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::CalculateTransform: Canvas is null!");
        return;
    }

    canvas_->Save();
    ScaleMirrorIfNeed(rotation, *canvas_);
    canvasMatrix_ = canvas_->GetTotalMatrix();
}

void RSUniRenderVirtualProcessor::ScaleMirrorIfNeed(const ScreenRotation angle, RSPaintFilterCanvas& canvas)
{
    if (screenCorrection_ == ScreenRotation::ROTATION_90 ||
        screenCorrection_ == ScreenRotation::ROTATION_270) {
        std::swap(virtualScreenWidth_, virtualScreenHeight_);
    }

    if (angle == ScreenRotation::ROTATION_90 ||
        angle == ScreenRotation::ROTATION_270) {
        std::swap(virtualScreenWidth_, virtualScreenHeight_);
    }

    float mirroredScreenWidth = mirroredScreenWidth_;
    float mirroredScreenHeight = mirroredScreenHeight_;
    if (enableVisibleRect_) {
        mirroredScreenWidth = visibleRect_.GetWidth();
        mirroredScreenHeight = visibleRect_.GetHeight();
        if (mirroredScreenWidth < EPSILON || mirroredScreenHeight < EPSILON) {
            RS_LOGE("RSUniRenderVirtualProcessor::ScaleMirrorIfNeed, input is illegal.");
            return;
        }
    }

    RS_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::ScaleMirrorIfNeed:(%f, %f, %f, %f), "
        "screenCorrection:%d, oriRotation:%d, scaleMode_: %d",
        mirroredScreenWidth, mirroredScreenHeight, virtualScreenWidth_, virtualScreenHeight_,
        static_cast<int>(screenCorrection_), static_cast<int>(angle), static_cast<int>(scaleMode_));

    bool needScale = ROSEN_NE(mirroredScreenWidth, virtualScreenWidth_) ||
                     ROSEN_NE(mirroredScreenHeight, virtualScreenHeight_);
    if (needScale) {
        if (scaleMode_ == ScreenScaleMode::FILL_MODE) {
            Fill(canvas, mirroredScreenWidth, mirroredScreenHeight, virtualScreenWidth_, virtualScreenHeight_);
        } else if (scaleMode_ == ScreenScaleMode::UNISCALE_MODE) {
            UniScale(canvas, mirroredScreenWidth, mirroredScreenHeight, virtualScreenWidth_, virtualScreenHeight_);
        }
    }

    if (enableVisibleRect_ && !drawMirrorCopy_) {
        canvas.Translate(-visibleRect_.GetLeft(), -visibleRect_.GetTop());
        RS_LOGD("RSUniRenderVirtualProcessor::ScaleMirrorIfNeed: Scale With VisibleRect, "
            "mirrorScaleX_: %{public}f, mirrorScaleY_: %{public}f", mirrorScaleX_, mirrorScaleY_);
    } else {
        canvas.Translate(-mirroredTranslateX_, -mirroredTranslateY_);
    }
}

void RSUniRenderVirtualProcessor::MergeFenceForHardwareEnabledDrawables()
{
    auto& renderThreadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!renderThreadParams) {
        RS_LOGE("RSUniRenderVirtualProcessor::%{public}s renderThreadParams null!", __func__);
        return;
    }
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::%{public}s renderFrame_ null!", __func__);
        return;
    }
    auto acquireFence = renderFrame_->GetAcquireFence();
    if (!acquireFence || !acquireFence->IsValid()) {
        RS_LOGE("RSUniRenderVirtualProcessor::%{public}s acquireFence not valid!", __func__);
        return;
    }
    for (const auto& [_, __, drawable] : renderThreadParams->GetHardwareEnabledTypeDrawables()) {
        if (!drawable) {
            RS_LOGW("RSUniRenderVirtualProcessor::%{public}s drawable null!", __func__);
            continue;
        }
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(drawable);
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
        if (!surfaceParams) {
            RS_LOGW("RSUniRenderVirtualProcessor::%{public}s surfaceParams null!", __func__);
            continue;
        }
        auto buffer = surfaceParams->GetBuffer();
        if (!buffer) {
            RS_LOGI("RSUniRenderVirtualProcessor::%{public}s buffer null!", __func__);
            continue;
        }
        if (surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
            continue; // skip protected layer
        }
        RS_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::%s: fence merged id: %" PRIu64 " name: %s",
            __func__, surfaceParams->GetId(), surfaceParams->GetName().c_str());
        RS_LOGD("RSUniRenderVirtualProcessor::%{public}s: fence merged id: %{public}" PRIu64 " name: %{public}s",
            __func__, surfaceParams->GetId(), surfaceParams->GetName().c_str());
        buffer->SetAndMergeSyncFence(acquireFence);
    }
}

void RSUniRenderVirtualProcessor::PostProcess()
{
    if (renderFrame_ == nullptr || renderEngine_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::PostProcess renderFrame or renderEngine is nullptr");
        return;
    }
    auto surfaceOhos = renderFrame_->GetSurface();
    RSBaseRenderEngine::SetUiTimeStamp(renderFrame_, surfaceOhos);
    renderFrame_->Flush();
    // Merge virtual screen fence to hardware enabled drawables, preventing buffer from being released too early.
    MergeFenceForHardwareEnabledDrawables();
    RS_LOGD("RSUniRenderVirtualProcessor::PostProcess, FlushFrame succeed.");
    RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::PostProcess, FlushFrame succeed.");
}

void RSUniRenderVirtualProcessor::ProcessSurface(RSSurfaceRenderNode& node)
{
    (void)node;
    RS_LOGI("RSUniRenderVirtualProcessor::ProcessSurface() is not supported.");
}

void RSUniRenderVirtualProcessor::ProcessScreenSurfaceForRenderThread(
    DrawableV2::RSScreenRenderNodeDrawable& screenDrawable)
{
    if (!isMirror_) {
        return;
    }
    auto surfaceHandler = screenDrawable.GetRSSurfaceHandlerOnDraw();
    if (canvas_ == nullptr || surfaceHandler->GetBuffer() == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::ProcessScreenSurface: Canvas or buffer is null!");
        return;
    }
    auto params = RSUniRenderUtil::CreateBufferDrawParam(*surfaceHandler, forceCPU_);
    if (renderEngine_ == nullptr) {
        return;
    }
    params.isMirror = true;
    if (enableVisibleRect_) {
        params.srcRect = visibleRect_;
        params.dstRect = Drawing::Rect(0, 0, visibleRect_.GetWidth(), visibleRect_.GetHeight());
    }
    renderEngine_->DrawScreenNodeWithParams(*canvas_, *surfaceHandler, params);
    canvas_->Restore();
}

void RSUniRenderVirtualProcessor::Fill(RSPaintFilterCanvas& canvas,
    float mainWidth, float mainHeight, float mirrorWidth, float mirrorHeight)
{
    if (mainWidth > 0 && mainHeight > 0) {
        mirrorScaleX_ = mirrorWidth / mainWidth;
        mirrorScaleY_ = mirrorHeight / mainHeight;
        canvas.Scale(mirrorScaleX_, mirrorScaleY_);
    }
}

void RSUniRenderVirtualProcessor::UniScale(RSPaintFilterCanvas& canvas,
    float mainWidth, float mainHeight, float mirrorWidth, float mirrorHeight)
{
    if (ROSEN_LE(mainWidth, 0) || ROSEN_LE(mainHeight, 0) || ROSEN_LE(mirrorWidth, 0) || ROSEN_LE(mirrorHeight, 0)) {
        return;
    }
    float startX = 0.0f;
    float startY = 0.0f;
    mirrorScaleX_ = mirrorWidth / mainWidth;
    mirrorScaleY_ = mirrorHeight / mainHeight;
    if (mirrorScaleY_ < mirrorScaleX_) {
        mirrorScaleX_ = mirrorScaleY_;
        startX = (mirrorWidth / mirrorScaleX_ - mainWidth) / 2; // 2 for calc X
    } else {
        mirrorScaleY_ = mirrorScaleX_;
        startY = (mirrorHeight / mirrorScaleY_ - mainHeight) / 2; // 2 for calc Y
    }

    if (EnableSlrScale() && !isMirroredDisplayRotating_) {
        if (slrManager_ == nullptr) {
            slrManager_ = std::make_shared<RSSLRScaleFunction>(virtualScreenWidth_, virtualScreenHeight_,
                mirroredScreenWidth_, mirroredScreenHeight_);
        } else {
            slrManager_->CheckOrRefreshScreen(virtualScreenWidth_, virtualScreenHeight_,
                mirroredScreenWidth_, mirroredScreenHeight_);
        }
        slrManager_->CheckOrRefreshColorSpace(renderFrameConfig_.colorGamut);
        slrManager_->CanvasScale(canvas);
        RS_LOGD("RSUniRenderVirtualProcessor::UniScale: Scale With SLR, color is %{public}d.",
            renderFrameConfig_.colorGamut);
        return;
    }

    canvas.Scale(mirrorScaleX_, mirrorScaleY_);
    canvas.Translate(startX, startY);
}

bool RSUniRenderVirtualProcessor::EnableSlrScale()
{
    float slrScale = std::min(mirrorScaleX_, mirrorScaleY_);
    if (MultiScreenParam::IsSlrScaleEnabled() && RSSystemProperties::GetSLRScaleEnabled() &&
        (slrScale < SLR_SCALE_THR_HIGH) && !enableVisibleRect_ && drawMirrorCopy_) {
        return true;
    }
    return false;
}

void RSUniRenderVirtualProcessor::ProcessCacheImage(Drawing::Image& cacheImage)
{
    if (canvas_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::ProcessCacheImage: Canvas is null!");
        return;
    }
    if (EnableSlrScale() && slrManager_) {
        slrManager_->ProcessCacheImage(*canvas_, cacheImage);
        RS_LOGD("RSUniRenderVirtualProcessor::ProcessCacheImage: Darw With SLR.");
        return;
    }
    RSUniRenderUtil::ProcessCacheImage(*canvas_, cacheImage);
}

void RSUniRenderVirtualProcessor::CanvasClipRegionForUniscaleMode(const Drawing::Matrix& visibleClipRectMatrix,
    const ScreenInfo& mainScreenInfo)
{
    if (canvas_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::CanvasClipRegion: Canvas is null!");
        return;
    }
    if (scaleMode_ != ScreenScaleMode::UNISCALE_MODE) {
        return;
    }
    Drawing::Rect rect(mirroredTranslateX_, mirroredTranslateY_,
        mirroredTranslateX_ + mirroredScreenWidth_, mirroredTranslateY_ + mirroredScreenHeight_);
    // SLR scaling does not scale canvas, get scale matrix from slrManager_ if SLR scaling is enabled.
    auto matrix = EnableSlrScale() && slrManager_ ? slrManager_->GetScaleMatrix() : canvas_->GetTotalMatrix();
    if (enableVisibleRect_) {
        if (drawMirrorCopy_) {
            rect = Drawing::Rect(0, 0, visibleRect_.GetWidth(), visibleRect_.GetHeight());
        } else {
            rect = visibleRect_;
            if (mainScreenInfo.isSamplingOn && !RSSystemProperties::GetSLRScaleEnabled()) {
                // If SLR scaling is not enabled, apply visibleClipRectMatrix to rect to exclude sampling operation.
                matrix = visibleClipRectMatrix;
            }
        }
    }
    matrix.MapRect(rect, rect);
    Drawing::RectI rectI = {rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom()};
    Drawing::Region clipRegion;
    clipRegion.SetRect(rectI);
    canvas_->ClipRegion(clipRegion);
    RS_LOGD("RSUniRenderVirtualProcessor::CanvasClipRegionForUniscaleMode, clipRect: %{public}s",
        rectI.ToString().c_str());
}

void RSUniRenderVirtualProcessor::ProcessRcdSurface(RSRcdSurfaceRenderNode& node)
{
    RS_LOGI("RSUniRenderVirtualProcessor::ProcessRcdSurface() is not supported.");
}

bool RSUniRenderVirtualProcessor::CheckIfBufferSizeNeedChange(
    ScreenRotation firstBufferRotation, ScreenRotation curBufferRotation)
{
    auto rotationDiff = static_cast<int>(firstBufferRotation) - static_cast<int>(curBufferRotation);
    return rotationDiff % 2;    // %2, If the difference is odd, buffer's width and height need a swap.
}

void RSUniRenderVirtualProcessor::SetVirtualScreenSize(
    DrawableV2::RSScreenRenderNodeDrawable& screenNodeDrawble, const sptr<RSScreenManager>& screenManager)
{
    auto virtualScreenInfo = screenManager->QueryScreenInfo(virtualScreenId_);
    autoBufferRotation_ = screenManager->GetVirtualScreenAutoRotation(virtualScreenId_);
    if (autoBufferRotation_) {
        auto rotationDiff = static_cast<int>(screenRotation_) - static_cast<int>(screenCorrection_);
        auto curBufferRotation =
            static_cast<ScreenRotation>((rotationDiff + SCREEN_ROTATION_NUM) % SCREEN_ROTATION_NUM);
        ScreenRotation firstBufferRotation = screenNodeDrawble.GetFirstBufferRotation();
        if (firstBufferRotation == ScreenRotation::INVALID_SCREEN_ROTATION) {
            screenNodeDrawble.SetFirstBufferRotation(curBufferRotation);
            RS_LOGI("RSUniRenderVirtualProcessor::%{public}s, set firstBufferRotation: %{public}d,"
                "width: %{public}" PRIu32 ", height: %{public}" PRIu32, __func__, static_cast<int>(curBufferRotation),
                renderFrameConfig_.width, renderFrameConfig_.height);
        } else if (CheckIfBufferSizeNeedChange(firstBufferRotation, curBufferRotation)) {
            std::swap(renderFrameConfig_.width, renderFrameConfig_.height);
            std::swap(virtualScreenInfo.width, virtualScreenInfo.height);
            RS_LOGI("RSUniRenderVirtualProcessor::%{public}s, swap buffer width and height, width: %{public}" PRIu32
                ", height: %{public}" PRIu32, __func__, renderFrameConfig_.width, renderFrameConfig_.height);
            RS_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::%s: swap buffer width and height, "
                "width: %" PRIu32 "height: %" PRIu32, __func__, renderFrameConfig_.width, renderFrameConfig_.height);
        }
    }

    virtualScreenWidth_ = static_cast<float>(virtualScreenInfo.width);
    virtualScreenHeight_ = static_cast<float>(virtualScreenInfo.height);
    originalVirtualScreenWidth_ = virtualScreenWidth_;
    originalVirtualScreenHeight_ = virtualScreenHeight_;
}
} // namespace Rosen
} // namespace OHOS
