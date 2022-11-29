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

#include "rs_uni_render_mirror_processor.h"

#include <ctime>

#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"
#include "rs_trace.h"
#include "string_utils.h"

namespace OHOS {
namespace Rosen {
bool RSUniRenderMirrorProcessor::Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId)
{
    if (!RSProcessor::Init(node, offsetX, offsetY, mirroredId)) {
        return false;
    }

    renderFrameConfig_.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;

    auto screenManager = CreateOrGetScreenManager();
    producerSurface = screenManager->GetProducerSurface(node.GetScreenId());
    if (producerSurface == nullptr) {
        RS_LOGE("RSUniRenderMirrorProcessor::Init for Screen(id %" PRIu64 "): ProducerSurface is null!",
            node.GetScreenId());
        return false;
    }

    // this is a work-around for the lack of color gamut conversion and yuv support in GPU.
    // currently we must forceCPU to do the composition for virtual screen.
    bool forceCPU = true;
    renderFrame_ = renderEngine_->RequestFrame(producerSurface, renderFrameConfig_, forceCPU);
    if (renderFrame_ == nullptr) {
        return false;
    }
    canvas_ = renderFrame_->GetCanvas();
    if (canvas_ == nullptr) {
        return false;
    }
    canvas_->concat(screenTransformMatrix_);

    return true;
}

void RSUniRenderMirrorProcessor::PostProcess()
{
    if (producerSurface == nullptr) {
        RS_LOGE("RSUniRenderMirrorProcessor::PostProcess surface is null!");
        return;
    }
    auto surfaceId = producerSurface->GetUniqueId();
    renderEngine_->SetUiTimeStamp(renderFrame_, surfaceId);
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderMirrorProcessor::PostProcess renderFrame_ is null.");
        return;
    }
    RSProcessor::RequestPerf(3, true); // set perf level 3 in mirrorScreen state
    renderFrame_->Flush();
}

void RSUniRenderMirrorProcessor::ProcessSurface(RSSurfaceRenderNode& node)
{
    RS_LOGI("RSUniRenderMirrorProcessor::ProcessSurface() is not supported.");
}

void RSUniRenderMirrorProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    RS_TRACE_NAME("RSUniRenderMirrorProcessor::ProcessDisplaySurface");
    if (canvas_ == nullptr) {
        RS_LOGE("RSVirtualScreenProcessor::ProcessDisplaySurface: Canvas is null!");
        return;
    }
    canvas_->drawImage(node.Snapshot(), 0, 0);
}
} // namespace Rosen
} // namespace OHOS