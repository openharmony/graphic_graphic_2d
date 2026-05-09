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
#include "feature/hdr/rs_hdr_util.h"
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

    auto params = static_cast<RSScreenRenderParams*>(screenDrawable.GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSUniRenderVirtualProcessor::%{public}s: ScreenParams is nullptr", __func__);
        return false;
    }

    virtualScreenId_ = params->GetScreenId();
    const auto& screenProperty = params->GetScreenProperty();
    auto screenStatus = screenProperty.GetVirtualScreenStatus();
    if (screenStatus == VIRTUAL_SCREEN_PAUSE) {
        RS_LOGD("RSUniRenderVirtualProcessor::Init screenStatus is pause");
        return false;
    }

    auto scaleModeDFX = static_cast<ScreenScaleMode>(RSSystemProperties::GetVirtualScreenScaleModeDFX());
    scaleMode_ = (scaleModeDFX == ScreenScaleMode::INVALID_MODE) ? screenProperty.GetScaleMode() : scaleModeDFX;
    enableVisibleRect_ = screenProperty.GetEnableVisibleRect();
    canvasRotation_ = screenProperty.GetCanvasRotation();
    if (enableVisibleRect_) {
        const auto& rect = screenProperty.GetVisibleRect();
        visibleRect_ = Drawing::RectI(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
        // set rotation for MirrorScreen visibleRect
        canvasRotation_ = screenProperty.IsVisibleRectSupportRotation();
    }
    bool mirrorScreenHDR = false;
    bool expandScreenHDR = false;
    renderFrameConfig_.colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    auto mirroredScreenDrawable =
        std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(params->GetMirrorSourceDrawable().lock());
    auto virtualScreenColorSpace = params->GetNewColorSpace();
    if (mirroredScreenDrawable) {
        mirroredScreenNodeId_ = mirroredScreenDrawable->GetId();
        auto childDrawables = params->GetDisplayDrawables();
        if (childDrawables.empty() || childDrawables.front() == nullptr) {
            RS_LOGE("RSUniRenderVirtualProcessor::InitForRenderThread: no child display in mirror screen");
            return false;
        }
        auto displayDrawable = childDrawables.front();
        auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable->GetRenderParams().get());
        auto mirroredDisplayDrawable = std::static_pointer_cast<DrawableV2::RSLogicalDisplayRenderNodeDrawable>(
            displayParams->GetMirrorSourceDrawable().lock());
        if (!mirroredDisplayDrawable) {
            RS_LOGE("RSUniRenderVirtualProcessor::InitForRenderThread: no mirrored display drawable");
            return false;
        }
        auto mirroredDisplayParams = static_cast<RSLogicalDisplayRenderParams*>(
            mirroredDisplayDrawable->GetRenderParams().get());
        auto mirroredScreenParams = static_cast<RSScreenRenderParams*>(mirroredScreenDrawable->GetRenderParams().get());
        screenRotation_ = mirroredDisplayParams->GetScreenRotation();
        screenCorrection_ = mirroredScreenParams->GetScreenProperty().GetScreenCorrection();
        auto mainScreenColorSpace = mirroredScreenParams->GetNewColorSpace();
        auto virtualScreenFixedP3 = virtualScreenColorSpace == GRAPHIC_COLOR_GAMUT_NATIVE;
        if (virtualScreenFixedP3 || (virtualScreenColorSpace != GRAPHIC_COLOR_GAMUT_SRGB &&
            mainScreenColorSpace != GRAPHIC_COLOR_GAMUT_SRGB)) {
            renderFrameConfig_.colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
            RS_LOGD("RSUniRenderVirtualProcessor::Init Set virtual screen buffer colorGamut to P3.");
        }
        mirrorScreenHDR = RSHdrUtil::IsHDRCast(params, renderFrameConfig_);
        RS_LOGD("RSUniRenderVirtualProcessor::Init HDRCast mirrorScreenHDR: %{public}d", mirrorScreenHDR);
    } else {
        if (virtualScreenColorSpace != GRAPHIC_COLOR_GAMUT_SRGB) {
            renderFrameConfig_.colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
        }
        expandScreenHDR = RSHdrUtil::IsHDRCast(params, renderFrameConfig_);
        RS_LOGD("RSUniRenderVirtualProcessor::Init HDRCast expandScreenHDR: %{public}d", expandScreenHDR);
    }

    SetVirtualScreenSize(screenDrawable);

    renderFrameConfig_.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;
    FrameContextConfig frameContextConfig = FrameContextConfig(false);
    frameContextConfig.isVirtual = true;
    frameContextConfig.timeOut = 0;

    RequestFramesForAllSurfaces(screenDrawable);
    if (surfaceFrames_.empty()) {
        RS_LOGE("RSUniRenderVirtualProcessor::Init for Screen(id %{public}" PRIu64 "): "
            "No valid surfaces in multi-surface configs!",
            virtualScreenId_);
        return false;
    }
    
    canvas_ = surfaceFrames_[primarySurfaceIndex_].canvas;
    if (canvas_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::Init for Screen(id %{public}" PRIu64 "): Canvas is null!",
            virtualScreenId_);
        return false;
    }

    RS_LOGD("RSUniRenderVirtualProcessor::Init, RequestFrame succeed.");
    RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::Init, RequestFrame succeed, colorSpace: %d.",
        renderFrameConfig_.colorGamut);
#ifndef ROSEN_CROSS_PLATFORM
    // Apply color space to all surfaces
    if (!surfaceFrames_.empty() && surfaceFrames_[primarySurfaceIndex_].frame) {
        auto rsSurface = surfaceFrames_[primarySurfaceIndex_].frame->GetSurface();
        if (rsSurface) {
            if (SetColorSpaceForMetadata(rsSurface->GetColorSpace()) != GSERROR_OK) {
                RS_LOGE("RSUniRenderVirtualProcessor::SetColorSpaceForMetadata failed");
            }
        }
    }
#endif
#ifdef USE_VIDEO_PROCESSING_ENGINE
    // Apply HDR metadata to all surfaces
    bool isHDRCast = mirrorScreenHDR || expandScreenHDR;
    for (size_t i = 0; i < surfaceFrames_.size(); ++i) {
        auto& frame = surfaceFrames_[i].frame;
        if (frame) {
            RSHdrUtil::EraseHdrMetadataKey(frame);
            if (RSHdrUtil::SetMetadata(RSHDRUtilConst::HDR_CAST_OUT_COLORSPACE, frame, isHDRCast) != GSERROR_OK) {
                RS_LOGD("RSUniRenderVirtualProcessor::Init SetMeadata failed");
            }
        }
        auto& canvas = surfaceFrames_[i].canvas;
        if (canvas) {
            if (isHDRCast) {
                canvas->SetHDREnabledVirtualScreen(true);
                canvas->SetHdrOn(true);
            } else {
                canvas->SetOnMultipleScreen(true);
            }
        }
    }
#endif

    return true;
}

void RSUniRenderVirtualProcessor::CancelCurrentFrame()
{
    for (auto& surfaceFrame : surfaceFrames_) {
        if (surfaceFrame.frame) {
            surfaceFrame.frame->CancelCurrentFrame();
        }
    }
}

sptr<SyncFence> RSUniRenderVirtualProcessor::GetFrameAcquireFence()
{
    if (primarySurfaceIndex_ >= surfaceFrames_.size() ||
        surfaceFrames_[primarySurfaceIndex_].frame == nullptr) {
        return SyncFence::InvalidFence();
    }
    return surfaceFrames_[primarySurfaceIndex_].frame->GetAcquireFence();
}

bool RSUniRenderVirtualProcessor::UpdateMirrorInfo(DrawableV2::RSLogicalDisplayRenderNodeDrawable& displayDrawable)
{
    if (!RSProcessor::UpdateMirrorInfo(displayDrawable)) {
        return false;
    }
    if (!isMirror_) {
        // expand screen don't need to process mirror info
        return true;
    }
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable.GetRenderParams().get());
    if (!params) {
        return false;
    }
    mirrorSourceRotation_ = params->GetMirrorSourceRotation();
    auto mirroredDisplayDrawable = std::static_pointer_cast<DrawableV2::RSLogicalDisplayRenderNodeDrawable>(
        params->GetMirrorSourceDrawable().lock());
    if (!mirroredDisplayDrawable) {
        return false;
    }
    auto mirroredParams = static_cast<RSLogicalDisplayRenderParams*>(mirroredDisplayDrawable->GetRenderParams().get());
    if (!mirroredParams) {
        return false;
    }
    auto mirroredScreenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        mirroredParams->GetAncestorScreenDrawable().lock());
    if (!mirroredScreenDrawable) {
        return false;
    }
    auto mirroredScreenParams = static_cast<RSScreenRenderParams*>(mirroredScreenDrawable->GetRenderParams().get());
    if (!mirroredScreenParams) {
        return false;
    }

    mirroredScreenWidth_ = mirroredParams->GetFixedWidth();
    mirroredScreenHeight_ = mirroredParams->GetFixedHeight();
    const Rect& contentRect = mirroredParams->GetDisplayContentRect();
    if (contentRect.w > 0 && contentRect.h > 0) {
        mirroredScreenWidth_ = contentRect.w;
        mirroredScreenHeight_ = contentRect.h;
    }
    const auto& screenProperty = mirroredScreenParams->GetScreenProperty();
    if (screenProperty.GetIsSamplingOn()) {
        mirroredScreenWidth_ *= screenProperty.GetSamplingScale();
        mirroredScreenHeight_ *= screenProperty.GetSamplingScale();
    }
    mirroredTranslateX_ = mirroredParams->GetOffsetX();
    mirroredTranslateY_ = mirroredParams->GetOffsetY();

    return true;
}

void RSUniRenderVirtualProcessor::CanvasInit(DrawableV2::RSLogicalDisplayRenderNodeDrawable& displayDrawable)
{
    // Save the initial canvas state
    canvas_->Save();
    if (displayDrawable.IsFirstTimeToProcessor()) {
        RS_LOGI("RSUniRenderVirtualProcessor::FirstInit, id: %{public}" PRIu64 ", "
            "screen(%{public}f, %{public}f, %{public}f, %{public}f), "
            "rotation: %{public}d, correction: %{public}d, needRotation: %{public}d, scaleMode: %{public}d, "
            "autoBufferRotation: %{public}d, mirrorSourceRotation: %{public}" PRIu32, virtualScreenId_,
            virtualScreenWidth_, virtualScreenHeight_, mirroredScreenWidth_, mirroredScreenHeight_, screenRotation_,
            screenCorrection_, canvasRotation_, scaleMode_, autoBufferRotation_, mirrorSourceRotation_);
    }
    if (mirrorSourceRotation_ != ScreenRotation::INVALID_SCREEN_ROTATION) {
        displayDrawable.SetOriginScreenRotation(mirrorSourceRotation_);
    } else if (displayDrawable.IsFirstTimeToProcessor() || canvasRotation_ || autoBufferRotation_) {
        displayDrawable.SetOriginScreenRotation(screenRotation_);
    }
    auto rotationDiff =
        static_cast<int>(displayDrawable.GetOriginScreenRotation()) - static_cast<int>(screenCorrection_);
    auto rotationAngle = static_cast<ScreenRotation>((rotationDiff + SCREEN_ROTATION_NUM) % SCREEN_ROTATION_NUM);
    OriginScreenRotation(rotationAngle, renderFrameConfig_.width, renderFrameConfig_.height);

    RS_LOGD("RSUniRenderVirtualProcessor::CanvasInit, id: %{public}" PRIu64 ", "
        "screen(%{public}f, %{public}f, %{public}f, %{public}f), "
        "rotation: %{public}d, correction: %{public}d, needRotation: %{public}d, rotationAngle: %{public}d, "
        "scaleMode: %{public}d, autoBufferRotation: %{public}d, mirrorSourceRotation: %{public}" PRIu32,
        virtualScreenId_, virtualScreenWidth_, virtualScreenHeight_, mirroredScreenWidth_, mirroredScreenHeight_,
        screenRotation_, screenCorrection_, canvasRotation_, rotationAngle, scaleMode_, autoBufferRotation_,
        mirrorSourceRotation_);
}

int32_t RSUniRenderVirtualProcessor::GetBufferAge() const
{
    if (primarySurfaceIndex_ >= surfaceFrames_.size()) {
        return 0;
    }
    return surfaceFrames_[primarySurfaceIndex_].frame ?
        surfaceFrames_[primarySurfaceIndex_].frame->GetBufferAge() : 0;
}

GSError RSUniRenderVirtualProcessor::SetColorSpaceForMetadata(GraphicColorGamut colorSpace)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    auto iter = COLORSPACE_TYPE.find(colorSpace);
    if (iter == COLORSPACE_TYPE.end()) {
        return GSERROR_INVALID_ARGUMENTS;
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
    return SetColorSpaceVecForMetadata(colorSpaceVec);
}

GSError RSUniRenderVirtualProcessor::SetColorSpaceVecForMetadata(const std::vector<uint8_t>& colorSpaceVec)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    GSError finalResult = GSERROR_OK;
    for (size_t i = 0; i < surfaceFrames_.size(); ++i) {
        auto& frame = surfaceFrames_[i].frame;
        if (!frame) {
            RS_LOGD("RSUniRenderVirtualProcessor::SetColorSpaceVecForMetadata surfaceFrames_[%zu] frame is null.", i);
            finalResult = GSERROR_INVALID_ARGUMENTS;
            continue;
        }
        auto rsSurface = frame->GetSurface();
        if (!rsSurface) {
            RS_LOGD("RSUniRenderVirtualProcessor::SetColorSpaceVecForMetadata surfaceFrames_[%zu] "
                "rsSurface is null.", i);
            finalResult = GSERROR_INVALID_ARGUMENT;
            continue;
        }
        auto buffer = rsSurface->GetCurrentBuffer();
        if (!buffer) {
            RS_LOGD("RSUniRenderVirtualProcessor::SetColorSpaceVecForMetadata surfaceFrames_[%zu] buffer is null.", i);
            finalResult = GSERROR_NO_BUFFER;
            continue;
        }
        if (buffer->SetMetadata(ATTRKEY_COLORSPACE_INFO, colorSpaceVec) != GSERROR_OK) {
            RS_LOGD("RSUniRenderVirtualProcessor::SetColorSpaceVecForMetadata surfaceFrames_[%zu] "
                "SetMetadata failed.", i);
            finalResult = GSERROR_API_FAILED;
        }
    }
    return finalResult;
}

bool RSUniRenderVirtualProcessor::SetCropRectForMetadata(
    const HDI::Display::Graphic::Common::V1_0::BufferHandleMetaRegion& metaRegion)
{
    std::vector<uint8_t> data;
    if (MetadataHelper::ConvertMetadataToVec(metaRegion, data) != GSERROR_OK) {
        RS_LOGD("%{public}s : ConvertMetadataToVec failed.", __func__);
        return false;
    }
    bool finalResult = true;
    for (size_t i = 0; i < surfaceFrames_.size(); ++i) {
        auto& frame = surfaceFrames_[i].frame;
        if (!frame) {
            RS_LOGD("%{public}s : surfaceFrames_[%zu] frame is null.", __func__, i);
            finalResult = false;
            continue;
        }
        auto rsSurface = frame->GetSurface();
        if (!rsSurface) {
            RS_LOGD("%{public}s : surfaceFrames_[%zu] rsSurface is null.", __func__, i);
            finalResult = false;
            continue;
        }
        auto buffer = rsSurface->GetCurrentBuffer();
        if (!buffer) {
            RS_LOGD("%{public}s : surfaceFrames_[%zu] buffer is null.", __func__, i);
            finalResult = false;
            continue;
        }
        if (buffer->SetMetadata(HDI::Display::Graphic::Common::V1_0::ATTRKEY_CROP_REGION, data) != GSERROR_OK) {
            RS_LOGD("%{public}s : surfaceFrames_[%zu] SetMetadata failed.", __func__, i);
            finalResult = false;
        }
    }
    return finalResult;
}

void RSUniRenderVirtualProcessor::SetDirtyInfo(const std::vector<RectI>& damageRegion)
{
    for (size_t i = 0; i < surfaceFrames_.size(); ++i) {
        auto& frame = surfaceFrames_[i].frame;
        if (frame) {
            frame->SetDamageRegion(damageRegion);
        } else {
            RS_LOGW("%{public}s: surfaceFrames_[%zu] frame is null.", __func__, i);
        }
    }
    if (SetRoiRegionToCodec(damageRegion) != GSERROR_OK) {
        RS_LOGD("RSUniRenderVirtualProcessor::SetDirtyInfo SetRoiRegionToCodec failed.");
    }
}

GSError RSUniRenderVirtualProcessor::SetRoiRegionToCodec(const std::vector<RectI>& damageRegion)
{
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

    GSError finalResult = GSERROR_OK;
    for (size_t i = 0; i < surfaceFrames_.size(); ++i) {
        auto& frame = surfaceFrames_[i].frame;
        if (!frame) {
            RS_LOGD("%{public}s: surfaceFrames_[%zu] frame is null.", __func__, i);
            finalResult = GSERROR_INVALID_ARGUMENTS;
            continue;
        }
        auto rsSurface = frame->GetSurface();
        if (!rsSurface) {
            RS_LOGD("%{public}s: surfaceFrames_[%zu] rsSurface is null.", __func__, i);
            finalResult = GSERROR_INVALID_ARGUMENTS;
            continue;
        }
        auto buffer = rsSurface->GetCurrentBuffer();
        if (!buffer) {
            RS_LOGD("%{public}s: surfaceFrames_[%zu] buffer is null.", __func__, i);
            finalResult = GSERROR_NO_BUFFER;
            continue;
        }
        if (buffer->SetMetadata(GrallocBufferAttr::GRALLOC_BUFFER_ATTR_BUFFER_ROI_INFO, roiRegionsVec) != GSERROR_OK) {
            RS_LOGD("%{public}s: surfaceFrames_[%zu] SetMetadata failed.", __func__, i);
            finalResult = GSERROR_API_FAILED;
        }
    }
    return finalResult;
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

    if (enableVisibleRect_) {
        if (!drawMirrorCopy_) {
            canvas.Translate(-visibleRect_.GetLeft(), -visibleRect_.GetTop());
        }
        RS_LOGD("RSUniRenderVirtualProcessor::ScaleMirrorIfNeed: Scale With VisibleRect, "
            "mirrorScaleX_: %{public}f, mirrorScaleY_: %{public}f", mirrorScaleX_, mirrorScaleY_);
    } else {
        canvas.Translate(-mirroredTranslateX_, -mirroredTranslateY_);
    }
}

void RSUniRenderVirtualProcessor::MergeMirrorFenceToHardwareEnabledDrawables(const sptr<SyncFence>& acquireFence)
{
    if (!acquireFence || !acquireFence->IsValid()) {
        RS_LOGE("RSUniRenderVirtualProcessor::%{public}s acquireFence not valid!", __func__);
        return;
    }
    auto& renderThreadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!renderThreadParams) {
        RS_LOGE("RSUniRenderVirtualProcessor::%{public}s renderThreadParams null!", __func__);
        return;
    }
    for (const auto& [screenNodeId, _, drawable] : renderThreadParams->GetHardwareEnabledTypeDrawables()) {
        if (screenNodeId != mirroredScreenNodeId_) {
            RS_LOGD("RSUniRenderVirtualProcessor::%{public}s drawable not on mirroredscreen!", __func__);
            continue;
        }
        if (!drawable) {
            RS_LOGW("RSUniRenderVirtualProcessor::%{public}s drawable null!", __func__);
            continue;
        }
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
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
    if (renderEngine_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::PostProcess renderEngine is nullptr");
        return;
    }

    if (surfaceFrames_.size() == 1) {
        if (surfaceFrames_[primarySurfaceIndex_].frame == nullptr) {
            RS_LOGE("RSUniRenderVirtualProcessor::PostProcess renderFrame is nullptr");
            return;
        }
        auto surfaceOhos = surfaceFrames_[primarySurfaceIndex_].frame->GetSurface();
        RSBaseRenderEngine::SetUiTimeStamp(surfaceFrames_[primarySurfaceIndex_].frame, surfaceOhos);
        surfaceFrames_[primarySurfaceIndex_].frame->Flush();
        sptr<SyncFence> fence = surfaceFrames_[primarySurfaceIndex_].frame->GetAcquireFence();
        if (isMirror_) {
            MergeMirrorFenceToHardwareEnabledDrawables(fence);
        }
        RS_LOGD("RSUniRenderVirtualProcessor::PostProcess, FlushFrame succeed.");
        RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::PostProcess, FlushFrame succeed.");
        return;
    }

    FlushAllSurfaces();
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
        RS_LOGD("RSUniRenderVirtualProcessor::Fill: scaleX: %{public}f, scaleY: %{public}f",
            mirrorScaleX_, mirrorScaleY_);
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

    if (EnableSlrScale()) {
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
    RS_LOGD("RSUniRenderVirtualProcessor::UniScale: Translate startX: %{public}f, startY: %{public}f"
        " scaleX: %{public}f, scaleY: %{public}f", startX, startY, mirrorScaleX_, mirrorScaleY_);
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
    bool isSamplingOn)
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
            if (isSamplingOn && !RSSystemProperties::GetSLRScaleEnabled()) {
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

void RSUniRenderVirtualProcessor::SetVirtualScreenSize(DrawableV2::RSScreenRenderNodeDrawable& screenDrawable)
{
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable.GetRenderParams().get());
    if (!screenParams) {
        return;
    }
    const auto& screenProperty = screenParams->GetScreenProperty();
    autoBufferRotation_ = screenProperty.GetAutoBufferRotation();
    virtualScreenWidth_ = screenProperty.GetWidth();
    virtualScreenHeight_ = screenProperty.GetHeight();
    if (autoBufferRotation_) {
        auto rotationDiff = static_cast<int>(screenRotation_) - static_cast<int>(screenCorrection_);
        auto curBufferRotation =
            static_cast<ScreenRotation>((rotationDiff + SCREEN_ROTATION_NUM) % SCREEN_ROTATION_NUM);
        ScreenRotation firstBufferRotation = screenDrawable.GetFirstBufferRotation();
        if (firstBufferRotation == ScreenRotation::INVALID_SCREEN_ROTATION) {
            screenDrawable.SetFirstBufferRotation(curBufferRotation);
            RS_LOGI("RSUniRenderVirtualProcessor::%{public}s, set firstBufferRotation: %{public}d,"
                "width: %{public}d, height: %{public}d", __func__, static_cast<int>(curBufferRotation),
                renderFrameConfig_.width, renderFrameConfig_.height);
        } else if (CheckIfBufferSizeNeedChange(firstBufferRotation, curBufferRotation)) {
            std::swap(renderFrameConfig_.width, renderFrameConfig_.height);
            std::swap(virtualScreenWidth_, virtualScreenHeight_);
            RS_LOGD("RSUniRenderVirtualProcessor::%{public}s, swap buffer width and height, width: %{public}d, "
                "height: %{public}d", __func__, renderFrameConfig_.width, renderFrameConfig_.height);
            RS_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::%s: swap buffer width and height, "
                "width: %d, height: %d", __func__, renderFrameConfig_.width, renderFrameConfig_.height);
        }
    }

    originalVirtualScreenWidth_ = virtualScreenWidth_;
    originalVirtualScreenHeight_ = virtualScreenHeight_;
}

void RSUniRenderVirtualProcessor::RequestFramesForAllSurfaces(
    DrawableV2::RSScreenRenderNodeDrawable& screenDrawable)
{
#ifndef ROSEN_CROSS_PLATFORM
    surfaceFrames_.clear();
    primarySurfaceIndex_ = 0;

    auto params = static_cast<RSScreenRenderParams*>(screenDrawable.GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSUniRenderVirtualProcessor::%{public}s: ScreenParams is nullptr", __func__);
        return;
    }
    const auto& screenProperty = params->GetScreenProperty();
    const auto& surfaces = screenProperty.GetMultiSurfaceConfigs();
    if (surfaces.empty()) {
        RS_LOGE("RSUniRenderVirtualProcessor::%{public}s: No surface configs", __func__);
        return;
    }
    if (params->IsVirtualSurfaceChanged()) {
        screenDrawable.ClearVirtualSurfaces();
    }

    FrameContextConfig frameContextConfig(false);
    frameContextConfig.isVirtual = true;
    frameContextConfig.timeOut = 0;

    bool primaryFound = false;
    for (size_t i = 0; i < surfaces.size(); ++i) {
        const auto& surfaceRegion = surfaces[i];
        SurfaceFrameConfig config;
        config.surface = surfaceRegion.surface;
        config.region = surfaceRegion.region;

        if (!config.surface) {
            RS_LOGD("RSUniRenderVirtualProcessor::%{public}s: surface at index %{public}zu is null",
                __func__, i);
            continue;
        }

        std::unique_ptr<RSRenderFrame> frame;
#ifdef RS_ENABLE_GL
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
            frame = renderEngine_->RequestFrame(config.surface, renderFrameConfig_, forceCPU_, false,
                frameContextConfig);
        }
#endif
        if (frame == nullptr) {
            uint64_t pSurfaceUniqueId = config.surface->GetUniqueId();
            auto rsSurface = screenDrawable.GetVirtualSurface(pSurfaceUniqueId);
            if (rsSurface == nullptr) {
                RS_LOGD("RSUniRenderVirtualProcessor::%{public}s: Make rssurface for surface index %{public}zu",
                    __func__, i);
                rsSurface = renderEngine_->MakeRSSurface(config.surface, forceCPU_);
                screenDrawable.SetVirtualSurface(rsSurface, pSurfaceUniqueId);
            }
            frame = renderEngine_->RequestFrame(
                std::static_pointer_cast<RSSurfaceOhos>(rsSurface), renderFrameConfig_, forceCPU_, false,
                frameContextConfig);
        }

        if (frame == nullptr) {
            RS_LOGE("RSUniRenderVirtualProcessor::%{public}s: RequestFrame failed for surface index %{public}zu",
                __func__, i);
            continue;
        }

        config.frame = std::move(frame);
        config.canvas = config.frame->GetCanvas();

        if (!config.canvas) {
            RS_LOGD("RSUniRenderVirtualProcessor::%{public}s: Canvas is null for surface index %{public}zu",
                __func__, i);
            continue;
        }

        if (!primaryFound) {
            primarySurfaceIndex_ = surfaceFrames_.size();
            primaryFound = true;
        }

        surfaceFrames_.push_back(std::move(config));
    }

    RS_LOGD("RSUniRenderVirtualProcessor::%{public}s: surface count=%{public}zu, primary=%{public}zu",
        __func__, surfaceFrames_.size(), primarySurfaceIndex_);

    // Check if any surface has a non-empty region (determines offscreen rendering)
    needsOffscreenRender_ = false;
    for (const auto& frame : surfaceFrames_) {
        if (!frame.region.IsEmpty()) {
            needsOffscreenRender_ = true;
            break;
        }
    }
#endif
}

void RSUniRenderVirtualProcessor::CopyToSecondarySurfaces()
{
#ifndef ROSEN_CROSS_PLATFORM
    if (surfaceFrames_.size() <= 1) {
        return;
    }

    if (!surfaceFrames_[primarySurfaceIndex_].frame) {
        RS_LOGE("RSUniRenderVirtualProcessor::%{public}s: Primary surface frame is null", __func__);
        return;
    }

    // Get snapshot from primary surface
    auto primarySurface = surfaceFrames_[primarySurfaceIndex_].canvas->GetSurface();
    if (!primarySurface) {
        RS_LOGE("RSUniRenderVirtualProcessor::%{public}s: Primary surface is null", __func__);
        return;
    }

    auto snapshot = primarySurface->GetImageSnapshot();
    if (!snapshot) {
        RS_LOGD("RSUniRenderVirtualProcessor::%{public}s: Failed to get snapshot from primary surface", __func__);
        return;
    }

    // Copy to secondary surfaces
    for (size_t i = 0; i < surfaceFrames_.size(); ++i) {
        if (i == primarySurfaceIndex_) {
            continue;
        }
        RS_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::%s: Copying to surface index %zu", __func__, i);

        auto& canvas = surfaceFrames_[i].canvas;
        if (!canvas) {
            RS_LOGD("RSUniRenderVirtualProcessor::%{public}s: Canvas is null for surface index %{public}zu",
                __func__, i);
            continue;
        }
        canvas->Save();
        canvas->DrawImage(*snapshot, 0, 0);
        canvas->Restore();

        RS_LOGD("RSUniRenderVirtualProcessor::%{public}s: Copied to surface index %{public}zu", __func__, i);
    }
#endif
}

void RSUniRenderVirtualProcessor::FlushAllSurfaces()
{
#ifndef ROSEN_CROSS_PLATFORM
    if (surfaceFrames_.size() <= 1) {
        return;
    }

    if (!needsOffscreenRender_) {
        CopyToSecondarySurfaces();
    }

    FlushGpu();
    surfaceFrames_[primarySurfaceIndex_].frame->SubmitGpu();

    std::vector<sptr<SyncFence>> fences;
    FlushBuffer(fences);

    if (isMirror_) {
        MergeMirrorFenceToHardwareEnabledDrawables(MergeAcquireFences(fences));
    }
#endif
}

void RSUniRenderVirtualProcessor::FlushGpu()
{
    for (size_t i = 0; i < surfaceFrames_.size(); ++i) {
        auto& frame = surfaceFrames_[i].frame;
        if (frame) {
            frame->FlushGpu();
        } else {
            RS_LOGW("%{public}s: surfaceFrames_[%zu] frame is null.", __func__, i);
        }
    }
}

void RSUniRenderVirtualProcessor::FlushBuffer(std::vector<sptr<SyncFence>>& fences)
{
    for (size_t i = 0; i < surfaceFrames_.size(); ++i) {
        auto& frame = surfaceFrames_[i].frame;
        if (!frame) {
            continue;
        }
        auto& config = surfaceFrames_[i];
        auto surfaceOhos = frame->GetSurface();
        RSBaseRenderEngine::SetUiTimeStamp(frame, surfaceOhos);
        if (!config.frame->FlushBuffer()) {
            RS_LOGE("RSUniRenderVirtualProcessor::%{public}s: FlushBuffer failed for surface index %{public}zu",
                __func__, i);
            continue;
        }
        auto fence = config.frame->GetAcquireFence();
        if (fence && fence->IsValid()) {
            fences.emplace_back(fence);
        }
        RS_LOGD("RSUniRenderVirtualProcessor::%{public}s: FlushBuffer succeed for surface index %{public}zu, "
            "fence is %{public}s", __func__, i, fence && fence->IsValid() ? "valid" : "invalid");
    }
}

sptr<SyncFence> RSUniRenderVirtualProcessor::MergeAcquireFences(const std::vector<sptr<SyncFence>>& fences) const
{
    sptr<SyncFence> mergedFence = SyncFence::InvalidFence();
    for (const auto& fence : fences) {
        if (!fence || !fence->IsValid()) {
            continue;
        }
        if (!mergedFence || !mergedFence->IsValid()) {
            mergedFence = fence;
            continue;
        }
        mergedFence = SyncFence::MergeFence("VirtualScreenFence", mergedFence, fence);
    }
    return mergedFence;
}

void RSUniRenderVirtualProcessor::BlitRegionsToSurfaces(const std::shared_ptr<Drawing::Image>& offscreenImage)
{
#ifndef ROSEN_CROSS_PLATFORM
    if (surfaceFrames_.size() <= 1 || !offscreenImage) {
        RS_LOGD("RSUniRenderVirtualProcessor::%{public}s: Not in multi-surface mode or no offscreen image", __func__);
        return;
    }

    Drawing::SamplingOptions sampling(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);

    for (size_t i = 0; i < surfaceFrames_.size(); ++i) {
        auto& config = surfaceFrames_[i];
        auto& canvas = config.canvas;
        const auto& region = config.region;

        // Source rect from offscreen image (in virtual screen coordinates)
        Drawing::Rect srcRect(region.left_, region.top_,
            region.left_ + region.width_, region.top_ + region.height_);
        // Destination rect on this surface (full surface)
        Drawing::Rect dstRect(0, 0, region.width_, region.height_);

        canvas->DrawImageRect(*offscreenImage, srcRect, dstRect, sampling);

        RS_LOGD("RSUniRenderVirtualProcessor::%{public}s: Blitted region [%d,%d,%d,%d] to surface index %{public}zu",
            __func__, region.left_, region.top_, region.width_, region.height_, i);
    }

    RS_LOGD("RSUniRenderVirtualProcessor::%{public}s: Blitted regions to all surfaces", __func__);
#endif
}
} // namespace Rosen
} // namespace OHOS
