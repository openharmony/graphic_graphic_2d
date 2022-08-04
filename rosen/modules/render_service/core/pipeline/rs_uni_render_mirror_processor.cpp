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

    renderFrameConfig_.usage = HBM_USE_CPU_READ | HBM_USE_MEM_DMA;

    auto screenManager = CreateOrGetScreenManager();
    sptr<Surface> producerSurface = screenManager->GetProducerSurface(node.GetScreenId());
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
    SetBufferTimeStamp();
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderMirrorProcessor::PostProcess renderFrame_ is null.");
        return;
    }
    renderFrame_->Flush();
}

void RSUniRenderMirrorProcessor::ProcessSurface(RSSurfaceRenderNode& node)
{
    RS_LOGI("RSUniRenderMirrorProcessor::ProcessSurface() is not supported.");
}

void RSUniRenderMirrorProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    if (canvas_ == nullptr) {
        RS_LOGE("RSVirtualScreenProcessor::ProcessDisplaySurface: Canvas is null!");
        return;
    }
    canvas_->drawImage(node.Snapshot(), 0, 0);
}

void RSUniRenderMirrorProcessor::SetBufferTimeStamp()
{
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderMirrorProcessor::SetBufferTimeStamp: renderFrame_ is nullptr");
        return;
    }

    auto frameOhosRaster =  static_cast<RSSurfaceFrameOhosRaster *>(renderFrame_->GetFrame().get());
    if (frameOhosRaster == nullptr || frameOhosRaster->GetBuffer() == nullptr) {
        RS_LOGE("RSUniRenderMirrorProcessor::SetBufferTimeStamp: buffer is nullptr");
        return;
    }

    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    GSError ret = frameOhosRaster->GetBuffer()->GetExtraData()->ExtraSet("timeStamp", static_cast<int64_t>(duration));
    if (ret != GSERROR_OK) {
        RS_LOGE("RSProcessor::SetBufferTimeStamp buffer ExtraSet failed");
    }
}
} // namespace Rosen
} // namespace OHOS