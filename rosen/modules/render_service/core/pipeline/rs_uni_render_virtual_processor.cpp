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
                                       std::shared_ptr<RSBaseRenderEngine> renderEngine)
{
    if (!RSProcessor::Init(node, offsetX, offsetY, mirroredId, renderEngine)) {
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
    mirrorWidth_ = static_cast<float>(mirrorScreenInfo.width);
    mirrorHeight_ = static_cast<float>(mirrorScreenInfo.height);

    renderFrameConfig_.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;

    producerSurface_ = screenManager->GetProducerSurface(node.GetScreenId());
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::Init for Screen(id %{public}" PRIu64 "): ProducerSurface is null!",
            node.GetScreenId());
        return false;
    }
    renderFrame_ = renderEngine_->RequestFrame(producerSurface_, renderFrameConfig_, forceCPU_, false);
    if (renderFrame_ == nullptr) {
        return false;
    }
    canvas_ = renderFrame_->GetCanvas();
    if (canvas_ == nullptr) {
        return false;
    }
    auto mirrorNode = node.GetMirrorSource().lock();
    isPhone_ = RSMainThread::Instance()->GetDeviceType() == DeviceType::PHONE;
    if (mirrorNode) {
        mainScreenRotation_ = mirrorNode->GetScreenRotation();
        RS_LOGD("RSUniRenderVirtualProcessor::Init, Screen(id %{public}" PRIu64 "), Rotation: %d", node.GetScreenId(),
            static_cast<uint32_t>(mainScreenRotation_));
    }
    if (mirrorNode && node.IsFirstTimeToProcessor()) {
        if (isPhone_) {
            node.SetOriginScreenRotation(mainScreenRotation_);
        } else {
            auto boundsGeoPtr = (mirrorNode->GetRenderProperties().GetBoundsGeometry());
            if (boundsGeoPtr != nullptr) {
                boundsGeoPtr->UpdateByMatrixFromSelf();
                node.SetInitMatrix(boundsGeoPtr->GetMatrix());
            }
        }
    }
#ifndef USE_ROSEN_DRAWING
    if (mirrorNode && isPhone_) {
        if (!(RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0) &&
            (node.GetOriginScreenRotation() == ScreenRotation::ROTATION_90 ||
            node.GetOriginScreenRotation() == ScreenRotation::ROTATION_270)) {
            CanvasRotation(node.GetOriginScreenRotation(), renderFrameConfig_.width, renderFrameConfig_.height);
            canvas_->translate(-(renderFrameConfig_.height / 2.0f), -(renderFrameConfig_.width / 2.0f));
        }
    } else {
        SkMatrix invertMatrix;
        if (node.GetInitMatrix().invert(&invertMatrix)) {
            screenTransformMatrix_.postConcat(invertMatrix);
        }
        canvas_->concat(screenTransformMatrix_);
    }
#else
    if (mirrorNode && isPhone_) {
        if (!(RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0) &&
            (node.GetOriginScreenRotation() == ScreenRotation::ROTATION_90 ||
            node.GetOriginScreenRotation() == ScreenRotation::ROTATION_270)) {
            CanvasRotation(node.GetOriginScreenRotation(), renderFrameConfig_.width, renderFrameConfig_.height);
            canvas_->Translate(-(renderFrameConfig_.height / 2.0f), -(renderFrameConfig_.width / 2.0f));
        }
    } else {
        Drawing::Matrix invertMatrix;
        if (node.GetInitMatrix().Invert(invertMatrix)) {
            screenTransformMatrix_.PostConcat(invertMatrix);
        }
        canvas_->ConcatMatrix(screenTransformMatrix_);
    }
#endif
    return true;
}

void RSUniRenderVirtualProcessor::CanvasRotation(ScreenRotation screenRotation, float width, float height)
{
#ifndef USE_ROSEN_DRAWING
    if (screenRotation == ScreenRotation::ROTATION_90) {
        canvas_->translate(width / 2.0f, height / 2.0f);
        canvas_->rotate(90); // 90 degrees
    } else if (screenRotation == ScreenRotation::ROTATION_180) {
        canvas_->rotate(180, width / 2.0f, height / 2.0f); // 180 degrees
    } else if (screenRotation == ScreenRotation::ROTATION_270) {
        canvas_->translate(width / 2.0f, height / 2.0f);
        canvas_->rotate(270); // 270 degrees
    }
#else
    if (screenRotation == ScreenRotation::ROTATION_90) {
        canvas_->Translate(width / 2.0f, height / 2.0f);
        canvas_->Rotate(90, 0, 0); // 90 degrees
    } else if (screenRotation == ScreenRotation::ROTATION_180) {
        canvas_->Rotate(180, width / 2.0f, height / 2.0f); // 180 degrees
    } else if (screenRotation == ScreenRotation::ROTATION_270) {
        canvas_->Translate(width / 2.0f, height / 2.0f);
        canvas_->Rotate(270, 0, 0); // 270 degrees
    }
#endif
}

void RSUniRenderVirtualProcessor::RotateMirrorCanvasIfNeed(RSDisplayRenderNode& node, bool canvasRotation)
{
    if (!canvasRotation && !RSSystemProperties::IsFoldScreenFlag()) {
        return;
    }
    auto rotation = canvasRotation ? node.GetScreenRotation() : node.GetOriginScreenRotation();
    if (RSSystemProperties::IsFoldScreenFlag() && node.GetScreenId() == 0) {
        // set rotation 0->90 90->180 180->270 270->0
        rotation = static_cast<ScreenRotation>((static_cast<int>(rotation) + 1) % SCREEN_ROTATION_NUM);
    }
#ifndef USE_ROSEN_DRAWING
    if (rotation != ScreenRotation::ROTATION_0) {
        auto screenManager = CreateOrGetScreenManager();
        auto mainScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
        if (rotation == ScreenRotation::ROTATION_90) {
            canvas_->rotate(90); // 90 degrees
            canvas_->translate(0, -(static_cast<float>(mainScreenInfo.height)));
        } else if (rotation == ScreenRotation::ROTATION_180) {
            canvas_->rotate(180, static_cast<float>(mainScreenInfo.width) / 2, // 180 degrees, 2 is centre
                static_cast<float>(mainScreenInfo.height) / 2); // 2 is centre
        } else if (rotation == ScreenRotation::ROTATION_270) {
            canvas_->rotate(270); // 270 degrees
            canvas_->translate(-(static_cast<float>(mainScreenInfo.width)), 0);
        }
    }
#else
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
#endif
}

void RSUniRenderVirtualProcessor::ScaleMirrorIfNeed(RSDisplayRenderNode& node)
{
    if (mainWidth_ != mirrorWidth_ || mainHeight_ != mirrorHeight_) {
#ifndef USE_ROSEN_DRAWING
        canvas_->clear(SK_ColorBLACK);
#else
        canvas_->Clear(SK_ColorBLACK);
#endif
        float mirrorScale = 1.0f; // 1 for init scale
        float startX = 0.0f;
        float startY = 0.0f;
        if ((mirrorHeight_ / mirrorWidth_) < (mainHeight_ / mainWidth_)) {
            mirrorScale = mirrorHeight_ / mainHeight_;
            startX = (mirrorWidth_ - (mirrorScale * mainWidth_)) / 2; // 2 for calc X
        } else if ((mirrorHeight_ / mirrorWidth_) > (mainHeight_ / mainWidth_)) {
            mirrorScale = mirrorWidth_ / mainWidth_;
            startY = (mirrorHeight_ - (mirrorScale * mainHeight_)) / 2; // 2 for calc Y
        }
#ifndef USE_ROSEN_DRAWING
        canvas_->translate(startX, startY);
        canvas_->scale(mirrorScale, mirrorScale);
#else
        canvas_->Translate(startX, startY);
        canvas_->Scale(mirrorScale, mirrorScale);
#endif
    }
}

void RSUniRenderVirtualProcessor::CanvasAdjustment(RSDisplayRenderNode& node, bool canvasRotation)
{
    const auto& property = node.GetRenderProperties();
    auto geoPtr = property.GetBoundsGeometry();
    if (geoPtr) {
        // if need rotation, canvas shouid be set to original absolute position
#ifndef USE_ROSEN_DRAWING
        if (canvasRotation) {
            canvas_->setMatrix(geoPtr->GetAbsMatrix());
        } else {
            canvas_->concatMatrix(geoPtr->GetMatrix());
        }
#else
        if (canvasRotation) {
            canvas_->SetMatrix(geoPtr->GetAbsMatrix());
        } else {
            canvas_->ConcatMatrix(geoPtr->GetMatrix());
        }
#endif
    }
}

void RSUniRenderVirtualProcessor::PostProcess(RSDisplayRenderNode* node)
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

void RSUniRenderVirtualProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    if (!isExpand_) {
        if (canvas_ == nullptr || node.GetBuffer() == nullptr) {
            RS_LOGE("RSUniRenderVirtualProcessor::ProcessDisplaySurface: Canvas or buffer is null!");
            return;
        }
        RS_TRACE_NAME_FMT("RSUniRenderVirtualProcessor::ProcessDisplaySurface:(%f, %f, %f, %f), " \
            "rotation:%d, oriRotation:%d",
            mainWidth_, mainHeight_, mirrorWidth_, mirrorHeight_,
            static_cast<int>(node.GetScreenRotation()), static_cast<int>(node.GetOriginScreenRotation()));

        CanvasAdjustment(node, canvasRotation_);

#ifndef USE_ROSEN_DRAWING
        canvas_->save();
        canvas_->clear(SK_ColorBLACK);
        SkMatrix invertMatrix;
        if (screenTransformMatrix_.invert(&invertMatrix)) {
            canvas_->concat(invertMatrix);
        }
#else
        canvas_->Save();
        canvas_->Clear(Drawing::Color::COLOR_BLACK);
        Drawing::Matrix invertMatrix;
        if (screenTransformMatrix_.Invert(invertMatrix)) {
            canvas_->ConcatMatrix(invertMatrix);
        }
#endif
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, forceCPU_);
        auto screenManager = CreateOrGetScreenManager();
        auto mainScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
        mainWidth_ = static_cast<float>(mainScreenInfo.width);
        mainHeight_ = static_cast<float>(mainScreenInfo.height);
        if ((RSSystemProperties::IsFoldScreenFlag() && node.GetScreenId() == 0)) {
            std::swap(mainWidth_, mainHeight_);
        }
        
        auto rotation = canvasRotation_ ? node.GetScreenRotation() : node.GetOriginScreenRotation();
        if (rotation == ScreenRotation::ROTATION_90 ||
            rotation == ScreenRotation::ROTATION_270) {
            std::swap(mainWidth_, mainHeight_);
        }
        
        ScaleMirrorIfNeed(node);
        RotateMirrorCanvasIfNeed(node, canvasRotation_);

        renderEngine_->DrawDisplayNodeWithParams(*canvas_, node, params);
#ifndef USE_ROSEN_DRAWING
        canvas_->restore();
#else
        canvas_->Restore();
#endif
    }
}

void RSUniRenderVirtualProcessor::ProcessRcdSurface(RSRcdSurfaceRenderNode& node)
{
    RS_LOGI("RSUniRenderVirtualProcessor::ProcessRcdSurface() is not supported.");
}
} // namespace Rosen
} // namespace OHOS