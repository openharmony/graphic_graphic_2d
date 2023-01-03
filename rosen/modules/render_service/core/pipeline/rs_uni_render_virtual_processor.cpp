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
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"
#include "pipeline/rs_uni_render_util.h"
#include "rs_trace.h"
#include "string_utils.h"

namespace OHOS {
namespace Rosen {
bool RSUniRenderVirtualProcessor::Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId)
{
    if (!RSProcessor::Init(node, offsetX, offsetY, mirroredId)) {
        return false;
    }

    // Do expand screen if the mirror id is invalid.
    if (mirroredId == INVALID_SCREEN_ID) {
        isExpand_ = true;
    } else {
        isExpand_ = false;
    }

    renderFrameConfig_.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;

    auto screenManager = CreateOrGetScreenManager();
    producerSurface_ = screenManager->GetProducerSurface(node.GetScreenId());
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::Init for Screen(id %" PRIu64 "): ProducerSurface is null!",
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

    return true;
}

void RSUniRenderVirtualProcessor::PostProcess()
{
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::PostProcess surface is null!");
        return;
    }
    auto surfaceId = producerSurface_->GetUniqueId();
    renderEngine_->SetUiTimeStamp(renderFrame_, surfaceId);
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::PostProcess renderFrame_ is null.");
        return;
    }
    RSProcessor::RequestPerf(3, true); // set perf level 3 in mirrorScreen state
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
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, forceCPU_);
        renderEngine_->DrawDisplayNodeWithParams(*canvas_, node, params);
    }
}
} // namespace Rosen
} // namespace OHOS