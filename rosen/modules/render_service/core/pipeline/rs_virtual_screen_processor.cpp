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


#include "rs_virtual_screen_processor.h"

#include <ctime>

#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"
#include "rs_trace.h"
#include "string_utils.h"

namespace OHOS {
namespace Rosen {
RSVirtualScreenProcessor::RSVirtualScreenProcessor()
{
}

RSVirtualScreenProcessor::~RSVirtualScreenProcessor() noexcept
{
}

bool RSVirtualScreenProcessor::Init(ScreenId id, int32_t offsetX, int32_t offsetY)
{
    if (!RSProcessor::Init(id, offsetX, offsetY)) {
        return false;
    }

    renderFrameConfig_.usage = HBM_USE_CPU_READ | HBM_USE_MEM_DMA;

    auto screenManager = CreateOrGetScreenManager();
    producerSurface_ = screenManager->GetProducerSurface(id);
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSVirtualScreenProcessor::Init for Screen(id %llu): ProducerSurface is null!", id);
        return false;
    }

    // this is a work-around for the lack of colorgamut convertion and yuv support in GPU.
    // currently we must forceCPU to do the composition for virtual screen.
    bool forceCPU = true;
    renderFrame_ = renderEngine_->RequestFrame(producerSurface_, renderFrameConfig_, forceCPU);
    if (renderFrame_ == nullptr) {
        return false;
    }
    canvas_ = renderFrame_->GetCanvas();
    if (canvas_ == nullptr) {
        return false;
    }

    return true;
}

void RSVirtualScreenProcessor::PostProcess()
{
    SetBufferTimeStamp();
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSVirtualScreenProcessor::PostProcess renderFrame_ is null.");
        return;
    }
    renderFrame_->Flush();
}

void RSVirtualScreenProcessor::ProcessSurface(RSSurfaceRenderNode& node)
{
    if (canvas_ == nullptr) {
        RS_LOGE("RSVirtualScreenProcessor::ProcessSurface: Canvas is null!");
        return;
    }

    std::string traceInfo;
    AppendFormat(traceInfo, "RSVirtualScreenProcessor::ProcessSurface Node:%s ",
        node.GetName().c_str());
    RS_TRACE_NAME(traceInfo.c_str());

    if (GetMirror()) {
        RS_LOGI("RSVirtualScreenProcessor::ProcessSurface mirrorScreen is not support rotation");
        screenInfo_.rotation = ScreenRotation::ROTATION_0;
        screenInfo_.rotationMatrix = SkMatrix::I();
    }

    IRect clipRect = {
        node.GetDstRect().left_ - offsetX_,
        node.GetDstRect().top_ - offsetY_,
        node.GetDstRect().width_,
        node.GetDstRect().height_};
    renderEngine_->DrawSurfaceNode(*canvas_, node, screenInfo_, clipRect, true);
}

void RSVirtualScreenProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    RS_LOGI("RSVirtualScreenProcessor::ProcessDisplaySurface() is not suppoerted.");
}

void RSVirtualScreenProcessor::SetBufferTimeStamp()
{
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSVirtualScreenProcessor::SetBufferTimeStamp: renderFrame_ is nullptr");
        return;
    }

    auto frameOhosRaster =  static_cast<RSSurfaceFrameOhosRaster *>(renderFrame_->GetFrame().get());
    if (frameOhosRaster == nullptr || frameOhosRaster->GetBuffer() == nullptr) {
        RS_LOGE("RSVirtualScreenProcessor::SetBufferTimeStamp: buffer is nullptr");
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
