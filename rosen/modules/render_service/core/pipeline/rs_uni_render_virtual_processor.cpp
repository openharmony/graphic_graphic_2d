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

#include "common/rs_optional_trace.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "metadata_helper.h"
#include "platform/common/rs_log.h"
#ifndef NEW_RENDER_CONTEXT
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"
#endif
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "string_utils.h"

namespace OHOS {
namespace Rosen {
bool RSUniRenderVirtualProcessor::Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId,
                                       std::shared_ptr<RSBaseRenderEngine> renderEngine, bool isRenderThread)
{
    // TO-DO adapt isRenderThread
    if (!RSProcessor::Init(node, offsetX, offsetY, mirroredId, renderEngine, isRenderThread)) {
        return false;
    }

    // Do expand screen if the mirror id is invalid.
    if (mirroredId == INVALID_SCREEN_ID) {
        isExpand_ = true;
    } else {
        isExpand_ = false;
    }

    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        return false;
    }
    auto virtualScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
    canvasRotation_ = screenManager->GetCanvasRotation(node.GetScreenId());
    scaleMode_ = screenManager->GetScaleMode(node.GetScreenId());
    mirrorWidth_ = static_cast<float>(virtualScreenInfo.width);
    mirrorHeight_ = static_cast<float>(virtualScreenInfo.height);
    auto mainScreenNode = node.GetMirrorSource().lock();
    if (mainScreenNode) {
        screenRotation_ = mainScreenNode->GetScreenRotation();
        screenCorrection_ = screenManager->GetScreenCorrection(mainScreenNode->GetScreenId());
        auto mainScreenInfo = screenManager->QueryScreenInfo(mainScreenNode->GetScreenId());
        mainWidth_ = static_cast<float>(mainScreenInfo.width);
        mainHeight_ = static_cast<float>(mainScreenInfo.height);
    }

    renderFrameConfig_.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;

    producerSurface_ = screenManager->GetProducerSurface(node.GetScreenId());
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::Init for Screen(id %{public}" PRIu64 "): ProducerSurface is null!",
            node.GetScreenId());
        return false;
    }
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        renderFrame_ = renderEngine_->RequestFrame(producerSurface_, renderFrameConfig_, forceCPU_, false);
    }
#endif
    if (renderFrame_ == nullptr) {
        uint64_t pSurfaceUniqueId = producerSurface_->GetUniqueId();
        auto rsSurface = node.GetVirtualSurface(pSurfaceUniqueId);
        if (rsSurface == nullptr || screenManager->GetAndResetVirtualSurfaceUpdateFlag(node.GetScreenId())) {
            RS_LOGD("RSUniRenderVirtualProcessor::Init Make rssurface from producer Screen(id %{public}" PRIu64 ")",
                node.GetScreenId());
            RS_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::Init Make rssurface from producer Screen(id %" PRIu64 ")",
                node.GetScreenId());
            rsSurface = renderEngine_->MakeRSSurface(producerSurface_, forceCPU_);
            node.SetVirtualSurface(rsSurface, pSurfaceUniqueId);
        }
#ifdef NEW_RENDER_CONTEXT
        renderFrame_ = renderEngine_->RequestFrame(
            std::static_pointer_cast<RSRenderSurfaceOhos>(rsSurface), renderFrameConfig_, forceCPU_, false);
#else
        renderFrame_ = renderEngine_->RequestFrame(
            std::static_pointer_cast<RSSurfaceOhos>(rsSurface), renderFrameConfig_, forceCPU_, false);
#endif
    }
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::Init for Screen(id %{public}" PRIu64 "): RenderFrame is null!",
            node.GetScreenId());
        return false;
    }

    RS_LOGD("RSUniRenderVirtualProcessor::Init, RequestFrame succeed.");
    RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::Init, RequestFrame succeed.");

    canvas_ = renderFrame_->GetCanvas();
    if (canvas_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::Init for Screen(id %{public}" PRIu64 "): Canvas is null!",
            node.GetScreenId());
        return false;
    }

    CanvasInit(node);

    return true;
}

bool RSUniRenderVirtualProcessor::InitUniProcessor(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable)
{
    if (!RSProcessor::InitUniProcessor(displayDrawable)) {
        return false;
    }
    screenManager_ = CreateOrGetScreenManager();
    if (screenManager_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::InitUniProcessor ScreenManager is null!");
        return false;
    }
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable.GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSUniRenderVirtualProcessor::InitUniProcessor params is null!");
        return false;
    }
    auto mirroredDisplayDrawable = std::static_pointer_cast<DrawableV2::RSDisplayRenderNodeDrawable>(
        params->GetMirrorSourceDrawable());
    if (!mirroredDisplayDrawable) {
        RS_LOGE("RSUniRenderVirtualProcessor::InitUniProcessor mirroredDisplayDrawable is null");
        return false;
    }
    virtualScreenId_ = params->GetScreenId();
    canvasRotation_ = screenManager_->GetCanvasRotation(virtualScreenId_);
    scaleMode_ = screenManager_->GetScaleMode(virtualScreenId_);
    updateFlag_ = screenManager_->GetAndResetVirtualSurfaceUpdateFlag(virtualScreenId_);
    auto virtualScreenInfo = screenManager_->QueryScreenInfo(virtualScreenId_);
    virtualScreenWidth_ = static_cast<float>(virtualScreenInfo.width);
    virtualScreenHeight_ = static_cast<float>(virtualScreenInfo.height);
    auto mirroredParams = static_cast<RSDisplayRenderParams*>(mirroredDisplayDrawable->GetRenderParams().get());
    if (mirroredParams) {
        screenRotation_ = mirroredParams->GetScreenRotation();
        mirroredScreenId_ = mirroredParams->GetScreenId();
        screenCorrection_ = screenManager_->GetScreenCorrection(mirroredScreenId_);
        auto mirroredScreenInfo = screenManager_->QueryScreenInfo(mirroredScreenId_);
        mirroredScreenWidth_ = static_cast<float>(mirroredScreenInfo.width);
        mirroredScreenHeight_ = static_cast<float>(mirroredScreenInfo.height);
    }
    renderFrameConfig_.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;
    producerSurface_ = screenManager_->GetProducerSurface(virtualScreenId_);
    if (!RequestVirtualFrame(displayDrawable)) {
        RS_LOGE("RSUniRenderVirtualProcessor::InitUniProcessor RenderFrame is null!");
        return false;
    }
    canvas_ = renderFrame_->GetCanvas();
    if (canvas_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::InitUniProcessor Canvas is null!");
        return false;
    }
    CanvasInit(displayDrawable);
    return true;
}

bool RSUniRenderVirtualProcessor::RequestVirtualFrame(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable)
{
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (renderEngine == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::RequestVirtualFrame RenderEngine is null!");
        return false;
    }
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::RequestVirtualFrame for virtualScreen(id %{public}" PRIu64 "):"
            "ProducerSurface is null!", virtualScreenId_);
        return false;
    }
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        renderFrame_ = renderEngine->RequestFrame(producerSurface_, renderFrameConfig_, forceCPU_, false);
    }
#endif
    if (renderFrame_ == nullptr) {
        uint64_t pSurfaceUniqueId = producerSurface_->GetUniqueId();
        auto rsSurface = displayDrawable.GetVirtualSurface(pSurfaceUniqueId);
        if (rsSurface == nullptr || updateFlag_) {
            RS_LOGD("RSUniRenderVirtualProcessor::RequestVirtualFrame,"
                "Make rssurface from producer virtualScreen(id %{public}" PRIu64 ")", virtualScreenId_);
            RS_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::RequestVirtualFrame,"
                "Make rssurface from producer virtualScreen(id %" PRIu64 ")", virtualScreenId_);
            rsSurface = renderEngine->MakeRSSurface(producerSurface_, forceCPU_);
            displayDrawable.SetVirtualSurface(rsSurface, pSurfaceUniqueId);
        }
#ifdef NEW_RENDER_CONTEXT
        renderFrame_ = renderEngine->RequestFrame(
            std::static_pointer_cast<RSRenderSurfaceOhos>(rsSurface), renderFrameConfig_, forceCPU_, false);
#else
        renderFrame_ = renderEngine->RequestFrame(
            std::static_pointer_cast<RSSurfaceOhos>(rsSurface), renderFrameConfig_, forceCPU_, false);
#endif
    }
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::RequestVirtualFrame RenderFrame is null!");
        return false;
    }
    return true;
}

void RSUniRenderVirtualProcessor::CanvasInit(RSDisplayRenderNode& node)
{
    if (node.IsFirstTimeToProcessor() || canvasRotation_) {
        if (node.IsFirstTimeToProcessor()) {
            RS_LOGI("RSUniRenderVirtualProcessor::FirstInit, id: %{public}" PRIu64 ", " \
                "screen(%{public}f, %{public}f, %{public}f, %{public}f), " \
                "rotation: %{public}d, correction: %{public}d, needRotation: %{public}d, scaleMode: %{public}d",
                node.GetScreenId(), mainWidth_, mainHeight_, mirrorWidth_, mirrorHeight_,
                screenRotation_, screenCorrection_, canvasRotation_, scaleMode_);
        }
        node.SetOriginScreenRotation(screenRotation_);
    }
    auto rotationDiff = static_cast<int>(node.GetOriginScreenRotation()) - static_cast<int>(screenCorrection_);
    auto rotationAngle = static_cast<ScreenRotation>((rotationDiff + SCREEN_ROTATION_NUM) % SCREEN_ROTATION_NUM);
    OriginScreenRotation(rotationAngle, renderFrameConfig_.width, renderFrameConfig_.height);

    RS_LOGD("RSUniRenderVirtualProcessor::CanvasInit, id: %{public}" PRIu64 ", " \
        "screen(%{public}f, %{public}f, %{public}f, %{public}f), " \
        "rotation: %{public}d, correction: %{public}d, needRotation: %{public}d, rotationAngle: %{public}d",
        node.GetScreenId(), mainWidth_, mainHeight_, mirrorWidth_, mirrorHeight_,
        screenRotation_, screenCorrection_, canvasRotation_, rotationAngle);
}

void RSUniRenderVirtualProcessor::CanvasInit(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable)
{
    if (displayDrawable.IsFirstTimeToProcessor() || canvasRotation_) {
        if (displayDrawable.IsFirstTimeToProcessor()) {
            RS_LOGI("RSUniRenderVirtualProcessor::CanvasInit, id: %{public}" PRIu64 ", " \
                "screen(%{public}f, %{public}f, %{public}f, %{public}f), " \
                "rotation: %{public}d, correction: %{public}d, needRotation: %{public}d, scaleMode: %{public}d",
                virtualScreenId_, mirroredScreenWidth_, mirroredScreenHeight_, virtualScreenWidth_,
                virtualScreenHeight_, screenRotation_, screenCorrection_, canvasRotation_, scaleMode_);
        }
        displayDrawable.SetOriginScreenRotation(screenRotation_);
    }
    auto rotationDiff = static_cast<int>(displayDrawable.GetOriginScreenRotation()) -
        static_cast<int>(screenCorrection_);
    auto rotationAngle = static_cast<ScreenRotation>((rotationDiff + SCREEN_ROTATION_NUM) % SCREEN_ROTATION_NUM);
    OriginScreenRotation(rotationAngle, renderFrameConfig_.width, renderFrameConfig_.height);

    RS_LOGD("RSUniRenderVirtualProcessor::CanvasInit, id: %{public}" PRIu64 ", " \
        "screen(%{public}f, %{public}f, %{public}f, %{public}f), " \
        "rotation: %{public}d, correction: %{public}d, needRotation: %{public}d, scaleMode: %{public}d",
        virtualScreenId_, mirroredScreenWidth_, mirroredScreenHeight_, virtualScreenWidth_, virtualScreenHeight_,
        screenRotation_, screenCorrection_, canvasRotation_, scaleMode_);
}

int32_t RSUniRenderVirtualProcessor::GetBufferAge() const
{
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::GetBufferAge renderFrame_ is null.");
        return 0;
    }
    return renderFrame_->GetBufferAge();
}

void RSUniRenderVirtualProcessor::SetDirtyInfo(std::vector<RectI>& damageRegion)
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

GSError RSUniRenderVirtualProcessor::SetRoiRegionToCodec(std::vector<RectI>& damageRegion)
{
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
    if (damageRegion.size() <= ROI_REGIONS_MAX_CNT) {
        for (auto& rect : damageRegion) {
            RoiRegionInfo region = RoiRegionInfo{rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight()};
            roiRegions.regions[roiRegions.regionCnt++] = region;
        }
    } else {
        RectI mergedRect;
        for (auto& rect : damageRegion) {
            mergedRect = mergedRect.JoinRect(rect);
        }
        RoiRegionInfo region = RoiRegionInfo{mergedRect.GetLeft(), mergedRect.GetTop(),
            mergedRect.GetWidth(), mergedRect.GetHeight()};
        roiRegions.regions[roiRegions.regionCnt++] = region;
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

void RSUniRenderVirtualProcessor::ScaleMirrorIfNeed(RSDisplayRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (screenCorrection_ == ScreenRotation::ROTATION_90 ||
        screenCorrection_ == ScreenRotation::ROTATION_270) {
        std::swap(mirrorWidth_, mirrorHeight_);
    }

    auto angle = node.GetOriginScreenRotation();
    if (angle == ScreenRotation::ROTATION_90 ||
        angle == ScreenRotation::ROTATION_270) {
        std::swap(mirrorWidth_, mirrorHeight_);
    }

    RS_LOGD("RSUniRenderVirtualProcessor::ScaleMirrorIfNeed:(%{public}f, %{public}f, %{public}f, %{public}f), " \
        "screenCorrection:%{public}d, oriRotation:%{public}d, scaleMode:%{public}d",
        mainWidth_, mainHeight_, mirrorWidth_, mirrorHeight_,
        static_cast<int>(screenCorrection_), static_cast<int>(angle), static_cast<int>(scaleMode_));

    if (mainWidth_ == mirrorWidth_ && mainHeight_ == mirrorHeight_) {
        return;
    }

    canvas.Clear(SK_ColorBLACK);
    if (scaleMode_ == ScreenScaleMode::FILL_MODE) {
        Fill(canvas, mainWidth_, mainHeight_, mirrorWidth_, mirrorHeight_);
    } else if (scaleMode_ == ScreenScaleMode::UNISCALE_MODE) {
        UniScale(canvas, mainWidth_, mainHeight_, mirrorWidth_, mirrorHeight_);
    }
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

    RS_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::ScaleMirrorIfNeed:(%f, %f, %f, %f), " \
        "screenCorrection:%d, oriRotation:%d",
        mirroredScreenWidth_, mirroredScreenHeight_, virtualScreenWidth_, virtualScreenHeight_,
        static_cast<int>(screenCorrection_), static_cast<int>(angle));

    if (mirroredScreenWidth_ == virtualScreenWidth_ && mirroredScreenHeight_ == virtualScreenHeight_) {
        return;
    }

    canvas.Clear(SK_ColorBLACK);
    if (scaleMode_ == ScreenScaleMode::FILL_MODE) {
        Fill(canvas, mirroredScreenWidth_, mirroredScreenHeight_, virtualScreenWidth_, virtualScreenHeight_);
    } else if (scaleMode_ == ScreenScaleMode::UNISCALE_MODE) {
        UniScale(canvas, mirroredScreenWidth_, mirroredScreenHeight_, virtualScreenWidth_, virtualScreenHeight_);
    }
}

void RSUniRenderVirtualProcessor::PostProcess()
{
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::PostProcess surface is null!");
        return;
    }
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::PostProcess renderframe is null.");
        return;
    }
    auto surfaceOhos = renderFrame_->GetSurface();
    renderEngine_->SetUiTimeStamp(renderFrame_, surfaceOhos);
    renderFrame_->Flush();
    RS_LOGD("RSUniRenderVirtualProcessor::PostProcess, FlushFrame succeed.");
    RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::PostProcess, FlushFrame succeed.");
}

void RSUniRenderVirtualProcessor::ProcessSurface(RSSurfaceRenderNode& node)
{
    (void)node;
    RS_LOGI("RSUniRenderVirtualProcessor::ProcessSurface() is not supported.");
}

void RSUniRenderVirtualProcessor::CalculateTransform(RSDisplayRenderNode& node)
{
    if (isExpand_) {
        return;
    }
    if (canvas_ == nullptr || node.GetBuffer() == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::ProcessDisplaySurface: Canvas or buffer is null!");
        return;
    }

    canvas_->Save();
    ScaleMirrorIfNeed(node, *canvas_);
    canvasMatrix_ = canvas_->GetTotalMatrix();
}

void RSUniRenderVirtualProcessor::CalculateTransform(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable)
{
    auto surfaceHandler = displayDrawable.GetRSSurfaceHandlerOnDraw();
    if (surfaceHandler == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::CalculateTransform: SurfaceHandler is null!");
    }
    if (canvas_ == nullptr || surfaceHandler->GetBuffer() == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::CalculateTransform: Canvas or buffer is null!");
        return;
    }

    canvas_->Save();
    ScreenRotation angle = displayDrawable.GetOriginScreenRotation();
    ScaleMirrorIfNeed(angle, *canvas_);
    canvasMatrix_ = canvas_->GetTotalMatrix();
}

void RSUniRenderVirtualProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    if (isExpand_) {
        return;
    }
    if (canvas_ == nullptr || node.GetBuffer() == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::ProcessDisplaySurface: Canvas or buffer is null!");
        return;
    }
    auto params = RSUniRenderUtil::CreateBufferDrawParam(node, forceCPU_);
    params.isMirror = true;
    renderEngine_->DrawDisplayNodeWithParams(*canvas_, node, params);
    canvas_->Restore();
}

void RSUniRenderVirtualProcessor::ProcessVirtualDisplaySurface(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable)
{
    auto surfaceHandler = displayDrawable.GetRSSurfaceHandlerOnDraw();
    if (surfaceHandler == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::ProcessVirtualDisplaySurface: SurfaceHandler is null!");
    }
    if (canvas_ == nullptr || surfaceHandler->GetBuffer() == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::ProcessVirtualDisplaySurface: Canvas or buffer is null!");
        return;
    }
    auto bufferDrawParam = RSUniRenderUtil::CreateBufferDrawParam(*surfaceHandler, forceCPU_);
    bufferDrawParam.isMirror = true;
    renderEngine_->DrawDisplayNodeWithParams(*canvas_, *surfaceHandler, bufferDrawParam);
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
    if (mainWidth > 0 && mainHeight > 0) {
        float startX = 0.0f;
        float startY = 0.0f;
        mirrorScaleX_ = mirrorWidth / mainWidth;
        mirrorScaleY_ = mirrorHeight / mainHeight;
        if (mirrorScaleY_ < mirrorScaleX_) {
            mirrorScaleX_ = mirrorScaleY_;
            startX = (mirrorWidth - (mirrorScaleX_ * mainWidth)) / 2; // 2 for calc X
        } else {
            mirrorScaleY_ = mirrorScaleX_;
            startY = (mirrorHeight - (mirrorScaleY_ * mainHeight)) / 2; // 2 for calc Y
        }
        canvas.Translate(startX, startY);
        canvas.Scale(mirrorScaleX_, mirrorScaleY_);
    }
}

void RSUniRenderVirtualProcessor::ProcessRcdSurface(RSRcdSurfaceRenderNode& node)
{
    RS_LOGI("RSUniRenderVirtualProcessor::ProcessRcdSurface() is not supported.");
}
} // namespace Rosen
} // namespace OHOS