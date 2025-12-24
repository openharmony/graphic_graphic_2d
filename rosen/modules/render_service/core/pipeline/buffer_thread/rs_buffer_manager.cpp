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
        RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager AddPendingReleaseBuffer not find seqNum %u fence %d",
                        uint32_t(seqNum), fence->Get());
        pendingReleaseBuffers_[seqNum] = { nullptr, nullptr, fence };
    } else {
        if (fence == nullptr) {
            return;
        }
        RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager AddPendingReleaseBuffer seqNum %u mergedFence_ %d fence %d",
                        uint32_t(seqNum),
                        iter->second.mergedFence_ ? iter->second.mergedFence_->Get() : -1, fence->Get());
        {
            iter->second.mergedFence_ = SyncFence::MergeFence("bufferFence", iter->second.mergedFence_, fence);
            RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager AddPendingReleaseBuffer After Merge seqNum %u mergedFence_ %d "
                "fence %d", uint32_t(seqNum),
                iter->second.mergedFence_ ? iter->second.mergedFence_->Get() : -1, fence->Get());
        }
    }
}

void RSBufferManager::ReleaseLayerBuffers(std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>>& rsLayers,
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>>& releaseBufferFenceVec)
{
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> uniBufferCount = nullptr;
    sptr<SyncFence> uniFence = nullptr;
    std::set<uint32_t> decedSet = {};
    for (const auto& [id, buffer, fence] : releaseBufferFenceVec) {
        if (rsLayers.count(id) == 0) {
            RS_LOGE("RSBufferManager::ReleaseLayerBuffers has no id: %{public}" PRIu64 " layer", id);
            continue;
        }
        auto layer = rsLayers[id].lock();
        if (layer == nullptr) {
            RS_LOGE("RSBufferManager::ReleaseLayerBuffers layer is nullptr");
            continue;
        }
        if (buffer == nullptr) {
            continue;
        }
        auto seqNum = buffer->GetSeqNum();
        auto bufferOwnerCount = layer->GetSeqNumFromBufferOwnerCounts(seqNum);

        RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager::ReleaseLayerBuffers seqNum %u fence %d",
            uint32_t(seqNum), fence ? fence->Get() : -1);

        if (bufferOwnerCount) {
            if (layer->GetUniRenderFlag()) {
                uniBufferCount = bufferOwnerCount;
                uniFence = fence;
            }

            AddPendingReleaseBuffer(seqNum, fence);
            bufferOwnerCount->OnBufferReleased();
            decedSet.insert(seqNum);
         }
    }

    if (uniBufferCount) {
        std::lock_guard<std::mutex> lock(uniBufferCount->mapMutex_);
        for (auto [id, seqNum] : uniBufferCount->uniOnDrawBufferMap_) {
            if (decedSet.find(seqNum) != decedSet.end()) {
                continue;
            }
            if (rsLayers.count(id) == 0) {
                RS_LOGE("RSBufferManager::ReleaseLayerBuffers has no id: %{public}" PRIu64 " layer", id);
                continue;
            }
            auto layer = rsLayers[id].lock();
            if (layer == nullptr) {
                RS_LOGE("RSBufferManager::ReleaseLayerBuffers layer is nullptr");
                continue;
            }

            auto bufferOwnerCount = layer->GetSeqNumFromBufferOwnerCounts(seqNum);
            if (bufferOwnerCount == nullptr) {
                continue;
            }

            AddPendingReleaseBuffer(bufferOwnerCount->seqNum_, uniFence);
            if (!bufferOwnerCount->CheckLastUniBufferOwner(uniBufferCount->seqNum_)) {
                layer->SetBufferOwnerCount(bufferOwnerCount);
            }
            bufferOwnerCount->OnBufferReleased();
        }
        uniBufferCount->uniOnDrawBufferMap_.clear();
    }
}

void RSBufferManager::BufferReleaseCallBack(uint64_t seqNum)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager BufferReleaseCallBack %u", uint32_t(seqNum));
    std::lock_guard<std::mutex> lock(screenNodeBufferReleasedMutex_);
    auto iter = pendingReleaseBuffers_.find(seqNum);
    if (iter != pendingReleaseBuffers_.end()) {
        auto info = iter->second;
        auto consumer = info.consumer_;
        if (consumer == nullptr || info.buffer_ == nullptr) {
            return;
        }
        RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager BufferReleaseCallBack ReleaseBuffer SeqNum %u Fence %d",
            uint32_t(info.buffer_->GetSeqNum()),
            info.mergedFence_ ? info.mergedFence_->Get() : -1);
        consumer->ReleaseBuffer(info.buffer_, info.mergedFence_);
        pendingReleaseBuffers_.erase(iter);
    } else {
        RS_LOGE("RSBufferManager BufferReleaseCallBack Release not find %{public}u", uint32_t(seqNum));
    }
}

} // OHOS
} // Rosen