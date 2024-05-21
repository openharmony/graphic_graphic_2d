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
#include "platform/common/rs_log.h"
#ifndef NEW_RENDER_CONTEXT
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"
#endif
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "rs_trace.h"
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
    auto mirrorScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
    canvasRotation_ = screenManager->GetCanvasRotation(node.GetScreenId());
    scaleMode_ = screenManager->GetScaleMode(node.GetScreenId());
    mirrorWidth_ = static_cast<float>(mirrorScreenInfo.width);
    mirrorHeight_ = static_cast<float>(mirrorScreenInfo.height);

    renderFrameConfig_.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;

    producerSurface_ = screenManager->GetProducerSurface(node.GetScreenId());
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::Init for Screen(id %{public}" PRIu64 "): ProducerSurface is null!",
            node.GetScreenId());
        return false;
    }
    auto rsSurface = node.GetVirtualSurface();
    if (rsSurface == nullptr) {
        RS_LOGD("RSUniRenderVirtualProcessor::Init Make rssurface from producer Screen(id %{public}" PRIu64 ")",
            node.GetScreenId());
        rsSurface = renderEngine_->MakeRSSurface(producerSurface_, forceCPU_);
        node.SetVirtualSurface(rsSurface);
    }
#ifdef NEW_RENDER_CONTEXT
    renderFrame_ = renderEngine_->RequestFrame(
        std::static_pointer_cast<RSRenderSurfaceOhos>(rsSurface), renderFrameConfig_, forceCPU_, false);
#else
    renderFrame_ = renderEngine_->RequestFrame(
        std::static_pointer_cast<RSSurfaceOhos>(rsSurface), renderFrameConfig_, forceCPU_, false);
#endif
    if (renderFrame_ == nullptr) {
        return false;
    }
    canvas_ = renderFrame_->GetCanvas();
    if (canvas_ == nullptr) {
        return false;
    }
    auto mirrorNode = node.GetMirrorSource().lock();
    if (mirrorNode) {
        exFoldScreen_ = (RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0);
        std::string lemScreen = system::GetParameter("const.window.foldscreen.type", "0,0,0,0");
        if (lemScreen[0] == '2') { // Small folding screen
            exFoldScreen_ = false;
        }
    }
    if (mirrorNode && !canvasRotation_) {
        mainScreenRotation_ = mirrorNode->GetScreenRotation();
        if (node.IsFirstTimeToProcessor()) {
            node.SetOriginScreenRotation(mainScreenRotation_);
            RS_LOGI("RSUniRenderVirtualProcessor::Init, OriginScreenRotation: %{public}d",
                node.GetOriginScreenRotation());
        }
        if (!exFoldScreen_) {
            OriginScreenRotation(node.GetOriginScreenRotation(), renderFrameConfig_.width, renderFrameConfig_.height);
        }
        RS_LOGD("RSUniRenderVirtualProcessor::Init, (id %{public}" PRIu64 "), mainScreenRotation: %{public}d, " \
            "canvasRotation: %{public}d, originScreenRotation: %{public}d, scaleMode: %{public}d",
            node.GetScreenId(), mainScreenRotation_, canvasRotation_, node.GetOriginScreenRotation(), scaleMode_);
    }
    return true;
}

int RSUniRenderVirtualProcessor::GetBufferAge()
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
    return buffer->SetMetadata(BufferHandleAttrKey::ATTRKEY_HDR_DYNAMIC_METADATA, roiRegionsVec);
}

void RSUniRenderVirtualProcessor::OriginScreenRotation(ScreenRotation screenRotation, float width, float height)
{
    if (screenRotation == ScreenRotation::ROTATION_90) {
        canvas_->Translate(width / 2.0f, height / 2.0f);
        canvas_->Rotate(90, 0, 0); // 90 degrees
        canvas_->Translate(-(height / 2.0f), -(width / 2.0f));
    } else if (screenRotation == ScreenRotation::ROTATION_180) {
        canvas_->Rotate(180, width / 2.0f, height / 2.0f); // 180 degrees
    } else if (screenRotation == ScreenRotation::ROTATION_270) {
        canvas_->Translate(width / 2.0f, height / 2.0f);
        canvas_->Rotate(270, 0, 0); // 270 degrees
        canvas_->Translate(-(height / 2.0f), -(width / 2.0f));
    } else {
        return;
    }
}

void RSUniRenderVirtualProcessor::RotateMirrorCanvasIfNeed(RSDisplayRenderNode& node, bool canvasRotation)
{
    if (!canvasRotation && !exFoldScreen_) {
        return;
    }
    auto rotation = canvasRotation ? node.GetScreenRotation() : node.GetOriginScreenRotation();
    if (exFoldScreen_) {
        // set rotation 0->90 90->180 180->270 270->0
        rotation = static_cast<ScreenRotation>((static_cast<int>(rotation) + 1) % SCREEN_ROTATION_NUM);
    }
    if (rotation != ScreenRotation::ROTATION_0) {
        auto screenManager = CreateOrGetScreenManager();
        auto mainScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
        if (rotation == ScreenRotation::ROTATION_90) {
            canvas_->Rotate(90, 0, 0); // 90 degrees
            canvas_->Translate(0, -(static_cast<float>(mainScreenInfo.height)));
        } else if (rotation == ScreenRotation::ROTATION_180) {
            canvas_->Rotate(180, static_cast<float>(mainScreenInfo.width) / 2, // 180 degrees, 2 is centre
                static_cast<float>(mainScreenInfo.height) / 2); // 2 is centre
        } else if (rotation == ScreenRotation::ROTATION_270) {
            canvas_->Rotate(270, 0, 0); // 270 degrees
            canvas_->Translate(-(static_cast<float>(mainScreenInfo.width)), 0);
        }
    }
}

void RSUniRenderVirtualProcessor::ScaleMirrorIfNeed(RSDisplayRenderNode& node)
{
    if (mainWidth_ == mirrorWidth_ && mainHeight_ == mirrorHeight_) {
        return;
    }

    canvas_->Clear(SK_ColorBLACK);

    if (scaleMode_ == ScreenScaleMode::FILL_MODE) {
        Fill(*canvas_, mainWidth_, mainHeight_, mirrorWidth_, mirrorHeight_);
    } else if (scaleMode_ == ScreenScaleMode::UNISCALE_MODE) {
        UniScale(*canvas_, mainWidth_, mainHeight_, mirrorWidth_, mirrorHeight_);
    }
}

void RSUniRenderVirtualProcessor::JudgeResolution(RSDisplayRenderNode& node)
{
    auto screenManager = CreateOrGetScreenManager();
    auto mainScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
    mainWidth_ = static_cast<float>(mainScreenInfo.width);
    mainHeight_ = static_cast<float>(mainScreenInfo.height);
    auto rotation = canvasRotation_ ? node.GetScreenRotation() : node.GetOriginScreenRotation();
    auto flag = (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270);

    if (exFoldScreen_) {
        if (!flag) {
            std::swap(mainWidth_, mainHeight_);
        }
    } else {
        if (flag) {
            if (canvasRotation_) {
                std::swap(mainWidth_, mainHeight_);
            } else {
                std::swap(mirrorWidth_, mirrorHeight_);
            }
        }
    }
}

void RSUniRenderVirtualProcessor::CanvasAdjustment(RSDisplayRenderNode& node, bool canvasRotation)
{
    const auto& property = node.GetRenderProperties();
    auto geoPtr = property.GetBoundsGeometry();
    if (geoPtr) {
        // if need rotation, canvas shouid be set to original absolute position
        if (canvasRotation) {
            canvas_->SetMatrix(geoPtr->GetAbsMatrix());
        } else {
            canvas_->ConcatMatrix(geoPtr->GetMatrix());
        }
    }
}

void RSUniRenderVirtualProcessor::PostProcess()
{
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::PostProcess surface is null!");
        return;
    }
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::PostProcess renderFrame_ is null.");
        return;
    }
    auto surfaceOhos = renderFrame_->GetSurface();
    renderEngine_->SetUiTimeStamp(renderFrame_, surfaceOhos);
    renderFrame_->Flush();
}

void RSUniRenderVirtualProcessor::ProcessSurface(RSSurfaceRenderNode& node)
{
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
    CanvasAdjustment(node, canvasRotation_);

    canvas_->Clear(Drawing::Color::COLOR_BLACK);
    Drawing::Matrix invertMatrix;
    if (screenTransformMatrix_.Invert(invertMatrix)) {
        canvas_->ConcatMatrix(invertMatrix);
    }
    
    JudgeResolution(node);
    ScaleMirrorIfNeed(node);
    RotateMirrorCanvasIfNeed(node, canvasRotation_);
    RS_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::ProcessDisplaySurface:(%f, %f, %f, %f), " \
        "rotation:%d, oriRotation:%d",
        mainWidth_, mainHeight_, mirrorWidth_, mirrorHeight_,
        static_cast<int>(node.GetScreenRotation()), static_cast<int>(node.GetOriginScreenRotation()));
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
    renderEngine_->DrawDisplayNodeWithParams(*canvas_, node, params);
    canvas_->Restore();
}

void RSUniRenderVirtualProcessor::Fill(RSPaintFilterCanvas& canvas,
    float mainWidth, float mainHeight, float mirrorWidth, float mirrorHeight)
{
    if (mainWidth > 0 && mainHeight > 0) {
        float mirrorScaleX = mirrorWidth / mainWidth;
        float mirrorScaleY = mirrorHeight / mainHeight;
        canvas.Scale(mirrorScaleX, mirrorScaleY);
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