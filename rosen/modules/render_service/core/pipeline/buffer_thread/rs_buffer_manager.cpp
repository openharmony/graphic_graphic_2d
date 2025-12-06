/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "rs_trace.h"
#include "platform/common/rs_log.h"

#include "rs_buffer_manager.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"

namespace OHOS {
namespace Rosen {

void RSBufferCollectorHelper::OnCanvasDrawEnd()
{
    for (auto& info : pendingReleaseBufferInfos_) {
        if (info.bufferOwnerCount_) {
            info.bufferOwnerCount_->DecRef();
        }
        RSUniRenderThread::Instance().AddPendingReleaseBuffer(info.consumer_, info.buffer_, fence_);
    }
}

void RSBufferCollectorHelper::SetAcquireFence(sptr<SyncFence> fence)
{
    if (fence == nullptr) {
        return;
    }
    fence_ = SyncFence::MergeFence("RSBufferCollectorHelper", fence_, fence);
}

void RSBufferCollectorHelper::OnCanvasDrawBuffer(sptr<IConsumerSurface> consumer, sptr<SurfaceBuffer> buffer, std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount)
{
    if (consumer == nullptr || buffer == nullptr || bufferOwnerCount == nullptr) {
        return;
    }
    bufferOwnerCount->AddRef();
    pendingReleaseBufferInfos_.push_back({consumer, buffer, bufferOwnerCount});
}

void RSBufferManager::AddPendingReleaseBuffer(sptr<IConsumerSurface> consumer,
    sptr<SurfaceBuffer> buffer, sptr<SyncFence> fence)
{
    if (buffer == nullptr) {
        return;
    }
    auto seqNum = buffer->GetSeqNum();
    std::lock_guard<std::mutex> lock(screenNodeBufferReleasedMutex_);
    auto iter = pendingReleaseBuffers_.find(seqNum);
    if (iter == pendingReleaseBuffers_.end()) {
        pendingReleaseBuffers_[seqNum] = { consumer, buffer, fence };
    } else {
        if (fence != nullptr) {
            iter->second.mergedFence_ = SyncFence::MergeFence("bufferFence", iter->second.mergedFence_, fence);
        }
        iter->second.buffer_ = buffer;
        iter->second.consumer_ = consumer;
    }
}

void RSBufferManager::AddPendingReleaseBuffer(uint64_t seqNum, sptr<SyncFence> fence)
{
    std::lock_guard<std::mutex> lock(screenNodeBufferReleasedMutex_);
    auto iter = pendingReleaseBuffers_.find(seqNum);
    if (iter == pendingReleaseBuffers_.end()) {
        if (fence == nullptr) {
            return;
        }
        RS_LOGI("RSBufferManager AddPendingReleaseBuffer not find seqNum %{public}u fence %{public}d",
                        uint32_t(seqNum), fence->Get());
        RS_TRACE_NAME_FMT("RSBufferManager AddPendingReleaseBuffer not find seqNum %u fence %d",
                        uint32_t(seqNum), fence->Get());
        pendingReleaseBuffers_[seqNum] = { nullptr, nullptr, fence };
    } else {
        if (fence == nullptr) {
            return;
        }
        RS_LOGI("RSBufferManager AddPendingReleaseBuffer seqNum %{public}u mergedFence_ %{public}d fence %{public}d",
                        uint32_t(seqNum),
                        iter->second.mergedFence_ ? iter->second.mergedFence_->Get() : -1, fence->Get());
        RS_TRACE_NAME_FMT("RSBufferManager AddPendingReleaseBuffer seqNum %u mergedFence_ %d fence %d",
                        uint32_t(seqNum),
                        iter->second.mergedFence_ ? iter->second.mergedFence_->Get() : -1, fence->Get());
        {
            iter->second.mergedFence_ = SyncFence::MergeFence("bufferFence", iter->second.mergedFence_, fence);
            RS_LOGI("RSBufferManager AddPendingReleaseBuffer After Merge seqNum %{public}u mergedFence_ %{public}d fence %{public}d",
                            uint32_t(seqNum),
                            iter->second.mergedFence_ ? iter->second.mergedFence_->Get() : -1, fence->Get());
            RS_TRACE_NAME_FMT("RSBufferManager AddPendingReleaseBuffer After Merge seqNum %u mergedFence_ %d fence %d",
                            uint32_t(seqNum),
                            iter->second.mergedFence_ ? iter->second.mergedFence_->Get() : -1, fence->Get());
        }
    }
}

void RSBufferManager::BufferReleaseCallBack(uint64_t seqNum)
{
    RS_TRACE_NAME_FMT("RSBufferManager BufferReleaseCallBack %u", uint32_t(seqNum));
    RS_LOGI("RSBufferManager BufferReleaseCallBack %{public}u", uint32_t(seqNum));
    // RSBufferThread::Instance().PostTask([seqNum, this](){
        std::lock_guard<std::mutex> lock(screenNodeBufferReleasedMutex_);
        auto iter = pendingReleaseBuffers_.find(seqNum);
        if (iter != pendingReleaseBuffers_.end()) {
            auto info = iter->second;
            auto consumer = info.consumer_;
            if (consumer == nullptr || info.buffer_ == nullptr) {
                return;
            }
            RS_TRACE_NAME_FMT("RSBufferManager BufferReleaseCallBack ReleaseBuffer SeqNum %u Fence %d",
                uint32_t(info.buffer_->GetSeqNum()),
                info.mergedFence_ ? info.mergedFence_->Get() : -1);
            RS_LOGI("RSBufferManager BufferReleaseCallBack ReleaseBuffer SeqNum %{public}u Fence %{public}d",
                uint32_t(info.buffer_->GetSeqNum()),
                info.mergedFence_ ? info.mergedFence_->Get() : -1);
            consumer->ReleaseBuffer(info.buffer_, info.mergedFence_);
            pendingReleaseBuffers_.erase(iter);
        } else {
            RS_TRACE_NAME_FMT("RSBufferManager BufferReleaseCallBack Release not find %u", uint32_t(seqNum));
            RS_LOGI("RSBufferManager BufferReleaseCallBack Release not find %{public}u", uint32_t(seqNum));
        }
    // });
}

} // OHOS
} // Rosen