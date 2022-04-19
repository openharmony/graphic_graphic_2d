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
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    Rect damage = {0};

    auto availableBufferCnt = node.GetAvailableBufferCount();
    if (availableBufferCnt == 0) {
        RS_LOGD("RSProcessor::ProcessSurface(node: %lld): no new buffer, try use old buffer.", node.GetId());
        buffer = node.GetBuffer();
        acquireFence = node.GetFence();
        damage = node.GetDamageRegion();
    } else {
        const auto& surfaceConsumer = node.GetConsumer();
        if (surfaceConsumer == nullptr) {
            RS_LOGE("RSProcessor::ProcessSurface(node: %lld): surfaceConsumer is null!", node.GetId());
            return false;
        }

        int32_t fenceFd = -1;
        int64_t timeStamp = 0;
        auto ret = surfaceConsumer->AcquireBuffer(buffer, fenceFd, timeStamp, damage);
        acquireFence = new SyncFence(fenceFd);
        if (ret != OHOS::SURFACE_ERROR_OK) {
            RS_LOGW("RSProcessor::ProcessSurface(node: %lld): AcquireBuffer failed(ret: %d), try use old buffer.",
                node.GetId(), ret);
            buffer = node.GetBuffer();
            acquireFence = node.GetFence();
            damage = node.GetDamageRegion();
        } else {
            availableBufferCnt = node.ReduceAvailableBuffer();
        }
    }

    if (buffer == nullptr) {
        RS_LOGE("RSProcessor::ProcessSurface(node: %lld): no avaliable buffer!", node.GetId());
        return false;
    }

    task();
    node.SetBuffer(buffer);
    node.SetFence(acquireFence);
    node.SetDamageRegion(damage);

    // still hava buffer(s) to consume.
    if (availableBufferCnt > 0) {
        RSMainThread::Instance()->RequestNextVSync();
    }

    return true;
}
} // namespace Rosen
} // namespace OHOS
