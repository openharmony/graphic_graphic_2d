/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_processor.h"

#include <ctime>
#include <sync_fence.h>
#include "rs_trace.h"

#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"

#include <platform/ohos/rs_surface_ohos.h>

namespace OHOS {
namespace Rosen {
SkCanvas* RSProcessor::CreateCanvas(
    const std::shared_ptr<RSSurfaceOhos>& surface,
    const BufferRequestConfig& requestConfig)
{
    RS_TRACE_NAME("CreateCanvas");

    if (surface == nullptr) {
        RS_LOGE("RSProcessor::CreateCanvas: surface is null!");
        return nullptr;
    }

#ifdef RS_ENABLE_GL
    if (renderContext_ == nullptr) {
        RS_LOGE("RSProcessor::CreateCanvas: render context is null!");
        return nullptr;
    }
    surface->SetRenderContext(renderContext_.get());
#endif

    currFrame_ = surface->RequestFrame(requestConfig.width, requestConfig.height);
    if (currFrame_ == nullptr) {
        RS_LOGE("RSProcessor::CreateCanvas: requestFrame failed!");
        return nullptr;
    }

    return currFrame_->GetCanvas();
}

std::unique_ptr<SkCanvas> RSProcessor::CreateCanvas(sptr<Surface> producerSurface, BufferRequestConfig requestConfig)
{
    RS_TRACE_NAME("CreateCanvas");
    auto ret = producerSurface->RequestBuffer(buffer_, releaseFence_, requestConfig);
    if (ret != SURFACE_ERROR_OK || buffer_ == nullptr) {
        return nullptr;
    }
    sptr<SyncFence> tempFence = new SyncFence(releaseFence_);
    int res = tempFence->Wait(3000);
    if (res < 0) {
        RS_LOGE("RsDebug RSProcessor::CreateCanvas this buffer is not available");
        //[PLANNING]: deal with the buffer is not available
    }
    auto addr = static_cast<uint32_t*>(buffer_->GetVirAddr());
    if (addr == nullptr) {
        return nullptr;
    }
    SkImageInfo info = SkImageInfo::Make(buffer_->GetWidth(), buffer_->GetHeight(),
                                        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    return SkCanvas::MakeRasterDirect(info, addr, buffer_->GetStride());
}

void RSProcessor::FlushBuffer(sptr<Surface> surface, BufferFlushConfig flushConfig)
{
    if (!surface || !buffer_) {
        RS_LOGE("RSProcessor::FlushBuffer surface or buffer is nullptr");
        return;
    }
    surface->FlushBuffer(buffer_, -1, flushConfig);
}

void RSProcessor::SetBufferTimeStamp()
{
    if (!buffer_) {
        RS_LOGE("RSProcessor::SetBufferTimeStamp buffer is nullptr");
        return;
    }
    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    GSError ret = buffer_->GetExtraData()->ExtraSet("timeStamp", static_cast<int64_t>(duration));
    if (ret != GSERROR_OK) {
        RS_LOGE("RSProcessor::SetBufferTimeStamp buffer ExtraSet failed");
    }
}

bool RSProcessor::ConsumeAndUpdateBuffer(RSSurfaceRenderNode& node, SpecialTask& task, sptr<SurfaceBuffer>& buffer)
{
    if (node.GetAvailableBufferCount() == 0 && !node.GetBuffer()) {
        RS_LOGI("RsDebug RSProcessor::ProcessSurface have no Available Buffer and"\
            "Node have no buffer node id:%llu", node.GetId());
        return false;
    }
    auto& surfaceConsumer = node.GetConsumer();
    if (!surfaceConsumer) {
        RS_LOGI("RSProcessor::ProcessSurface output is nullptr");
        return false;
    }
    if (node.GetAvailableBufferCount() >= 1) {
        int32_t fence = -1;
        int64_t timestamp = 0;
        Rect damage;
        auto sret = surfaceConsumer->AcquireBuffer(buffer, fence, timestamp, damage);
        sptr<SyncFence> acquireFence = new SyncFence(fence);
        if (!buffer || sret != OHOS::SURFACE_ERROR_OK) {
            RS_LOGE("RSProcessor::ProcessSurface: AcquireBuffer failed! sret: %{public}d", sret);
            if (sret == OHOS::GSERROR_NO_BUFFER) {
                node.ReduceAvailableBuffer();
            }
            return false;
        }
        task();
        node.SetBuffer(buffer);
        node.SetFence(acquireFence);
        node.SetDamageRegion(damage);
        if (node.ReduceAvailableBuffer() >= 1) {
            if (auto mainThread = RSMainThread::Instance()) {
                mainThread->RequestNextVSync();
            }
        }
    } else {
        node.SetBuffer(node.GetBuffer());
        node.SetFence(node.GetFence());
        node.SetDamageRegion(node.GetDamageRegion());
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
