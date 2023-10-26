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

    boundsWidth_ = node.GetRenderProperties().GetBoundsWidth();
    boundsHeight_ = node.GetRenderProperties().GetBoundsHeight();

    renderFrameConfig_.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;

    auto screenManager = CreateOrGetScreenManager();
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
    if (mirrorNode == nullptr) {
        return false;
    }
    auto rotation = mirrorNode->GetScreenRotation();
    isPhone_ = RSMainThread::Instance()->GetDeviceType() == DeviceType::PHONE;
    if (mirrorNode && node.IsFirstTimeToProcessor()) {
        if (isPhone_) {
            node.setFirstTimeScreenRotation(rotation);
        } else {
            auto boundsGeoPtr = (mirrorNode->GetRenderProperties().GetBoundsGeometry());
            if (boundsGeoPtr != nullptr) {
                boundsGeoPtr->UpdateByMatrixFromSelf();
                node.SetInitMatrix(boundsGeoPtr->GetMatrix());
            }
        }
        RS_LOGD("RSUniRenderVirtualProcessor::Init, Screen(id %{public}" PRIu64 "), Rotation: %d", node.GetScreenId(),
            static_cast<uint32_t>(rotation));
    }
    if (isPhone_) {
        if (node.getFirstTimeScreenRotation() == ScreenRotation::ROTATION_90) {
            canvas_->rotate(90, renderFrameConfig_.height / 2, renderFrameConfig_.height / 2); // 90 degrees
            canvas_->translate(0, renderFrameConfig_.height - renderFrameConfig_.width);
        } else if (node.getFirstTimeScreenRotation() == ScreenRotation::ROTATION_180) {
            canvas_->rotate(180, renderFrameConfig_.width / 2, renderFrameConfig_.height / 2); // 180 degrees
        } else if (node.getFirstTimeScreenRotation() == ScreenRotation::ROTATION_270) {
            canvas_->rotate(270, renderFrameConfig_.height / 2, renderFrameConfig_.height / 2); // 270 degrees
        }
    } else {
#ifndef USE_ROSEN_DRAWING
    SkMatrix invertMatrix;
    if (node.GetInitMatrix().invert(&invertMatrix)) {
        screenTransformMatrix_.postConcat(invertMatrix);
    }
    canvas_->concat(screenTransformMatrix_);
#else
    Drawing::Matrix invertMatrix;
    if (node.GetInitMatrix().Invert(invertMatrix)) {
        screenTransformMatrix_ = screenTransformMatrix_ * invertMatrix;
    }
    canvas_->ConcatMatrix(screenTransformMatrix_);
#endif
    }
    return true;
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

void RSUniRenderVirtualProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    if (!isExpand_) {
        RS_TRACE_NAME("RSUniRenderVirtualProcessor::ProcessDisplaySurface");
        if (canvas_ == nullptr || node.GetBuffer() == nullptr) {
            RS_LOGE("RSUniRenderVirtualProcessor::ProcessDisplaySurface: Canvas or buffer is null!");
            return;
        }

        canvas_->save();
        canvas_->clear(SK_ColorBLACK);
#ifndef USE_ROSEN_DRAWING
        SkMatrix invertMatrix;
        if (screenTransformMatrix_.invert(&invertMatrix)) {
            canvas_->concat(invertMatrix);
        }
#else
        Drawing::Matrix invertMatrix;
        if (screenTransformMatrix_.Invert(invertMatrix)) {
            canvas_->ConcatMatrix(invertMatrix);
        }
#endif
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, forceCPU_);
        auto screenManager = CreateOrGetScreenManager();
        auto mainScreenInfo = screenManager->QueryScreenInfo(screenManager->GetDefaultScreenId());
        float mainWidth = static_cast<float>(mainScreenInfo.width);
        float mainHeight = static_cast<float>(mainScreenInfo.height);
        // If the width and height not match the main screen, calculate the dstRect.
        if (mainWidth != boundsWidth_ || mainHeight != boundsHeight_) {
            SkRect mirrorDstRect;
            if ((boundsHeight_ / boundsWidth_) < (mainHeight / mainWidth)) {
                float mirrorScale = boundsHeight_ / mainHeight;
                mirrorDstRect = SkRect::MakeXYWH((boundsWidth_ - (mirrorScale * mainWidth)) / 2, 0, // 2 for calc X
                    mirrorScale * mainWidth, boundsHeight_);
            } else if ((boundsHeight_ / boundsWidth_) > (mainHeight / mainWidth)) {
                float mirrorScale = boundsWidth_ / mainWidth;
                mirrorDstRect = SkRect::MakeXYWH(0, (boundsHeight_ - (mirrorScale * mainHeight)) / 2, // 2 for calc Y
                    boundsWidth_, mirrorScale * mainHeight);
            }
            params.dstRect = mirrorDstRect;
        }

        renderEngine_->DrawDisplayNodeWithParams(*canvas_, node, params);
        canvas_->restore();
    }
}

void RSUniRenderVirtualProcessor::ProcessDrivenSurface(RSDrivenSurfaceRenderNode& node)
{
    RS_LOGI("RSUniRenderVirtualProcessor::ProcessDrivenSurface() is not supported.");
}
} // namespace Rosen
} // namespace OHOS