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

// set the acquireFence of unirender buffer, indicating that all buffers have been fully rendered on the GPU
void RSBufferCollectorHelper::SetAcquireFence(sptr<SyncFence> fence)
{
    if (fence == nullptr) {
        return;
    }
    if (fence_ && fence_->Get() != -1) {
        fence_ = SyncFence::MergeFence("RSBufferCollectorHelper", fence_, fence);
    } else {
        fence_ = fence;
    }
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
    auto bufferId = buffer->GetBufferId();
    RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager::AddPendingReleaseBuffer(with consumer) bufferId %" PRIu64
        " seq %u fence %d", bufferId, uint32_t(buffer->GetSeqNum()), fence->Get());
    std::lock_guard<std::mutex> lock(screenNodeBufferReleasedMutex_);
    auto iter = pendingReleaseBuffers_.find(bufferId);
    if (iter == pendingReleaseBuffers_.end()) {
        pendingReleaseBuffers_[bufferId] = { consumer, buffer, fence };
    } else {
        if (fence != nullptr) {
            // Merge the fences on the GPU and the fences on the DSS;
            // the buffer can only be released when both sides have finished using it.
            if (iter->second.mergedFence_ && iter->second.mergedFence_->Get() != -1) {
                iter->second.mergedFence_ = SyncFence::MergeFence("bufferFence", iter->second.mergedFence_, fence);
                RS_OPTIONAL_TRACE_NAME_FMT("AddPendingReleaseBuffer After Merge bufferId %" PRIu64 " mergedFence_ %d "
                    "fence %d", bufferId,
                    iter->second.mergedFence_ ? iter->second.mergedFence_->Get() : -1, fence->Get());
            } else {
                iter->second.mergedFence_ = fence;
            }
        }
        iter->second.buffer_ = buffer;
        if (iter->second.consumer_ == nullptr) {
            iter->second.consumer_ = consumer;
        }
    }
}

void RSBufferManager::AddPendingReleaseBuffer(uint64_t bufferId, sptr<SyncFence> fence)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager::AddPendingReleaseBuffer(without consumer) bufferId %" PRIu64
        " fence %d", bufferId, fence->Get());
    if (fence == nullptr) {
        RS_LOGE("RSBufferManager::AddPendingReleaseBuffer(without consumer) fence is null");
        return;
    }
    std::lock_guard<std::mutex> lock(screenNodeBufferReleasedMutex_);
    auto iter = pendingReleaseBuffers_.find(bufferId);
    if (iter == pendingReleaseBuffers_.end()) {
        pendingReleaseBuffers_[bufferId] = { nullptr, nullptr, fence };
    } else {
        if (iter->second.mergedFence_ && iter->second.mergedFence_->Get() != -1) {
            iter->second.mergedFence_ = SyncFence::MergeFence("bufferFence", iter->second.mergedFence_, fence);
            RS_OPTIONAL_TRACE_NAME_FMT("AddPendingReleaseBuffer After Merge bufferId %" PRIu64 " mergedFence_ %d "
                "fence %d", bufferId,
                iter->second.mergedFence_ ? iter->second.mergedFence_->Get() : -1, fence->Get());
        } else {
            iter->second.mergedFence_ = fence;
        }
    }
}

void RSBufferManager::OnReleaseLayerBuffers(std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>>& rsLayers,
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>>& releaseBufferFenceVec, uint64_t screenId)
{
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> uniBufferCount = nullptr;
    sptr<SyncFence> uniFence = nullptr;
    std::set<uint32_t> decedSet = {};
    for (const auto& [id, buffer, fence] : releaseBufferFenceVec) {
        if (rsLayers.count(id) == 0) {
            RS_LOGE("RSBufferManager::OnReleaseLayerBuffers has no id: %{public}" PRIu64 " layer", id);
            continue;
        }
        auto layer = rsLayers[id].lock();
        if (layer == nullptr) {
            RS_LOGE("RSBufferManager::OnReleaseLayerBuffers layer is nullptr");
            continue;
        }
        if (buffer == nullptr) {
            continue;
        }
        auto bufferId = buffer->GetBufferId();
        auto bufferOwnerCount = layer->PopBufferOwnerCountById(bufferId);

        RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager::OnReleaseLayerBuffers bufferId % " PRIu64 " seq:%u fence %d",
            bufferId, uint32_t(buffer->GetSeqNum()), fence ? fence->Get() : -1);

        if (bufferOwnerCount) {
            if (layer->GetUniRenderFlag()) {
                uniBufferCount = bufferOwnerCount;
                uniFence = fence;
            }

            AddPendingReleaseBuffer(bufferId, fence);
            bufferOwnerCount->OnBufferReleased();
            decedSet.insert(bufferId);
         }
    }

    ReleaseUniOnDrawBuffers(uniBufferCount, uniFence, decedSet, rsLayers, screenId);
}

void RSBufferManager::ReleaseUniOnDrawBuffers(std::shared_ptr<RSSurfaceHandler::BufferOwnerCount>& uniBufferCount,
    sptr<SyncFence>& uniFence, std::set<uint32_t>& decedSet,
    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>>& rsLayers, uint64_t screenId)
{
    if (uniBufferCount == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(uniBufferCount->mapMutex_);
    for (auto [id, bufferId] : uniBufferCount->uniOnDrawBufferMap_) {
        if (decedSet.find(bufferId) != decedSet.end()) {
            continue;
        }
        if (rsLayers.count(id) == 0) {
            RS_LOGE("RSBufferManager::ReleaseUniOnDrawBuffers has no id: %{public}" PRIu64 " layer", id);
            continue;
        }
        auto layer = rsLayers[id].lock();
        if (layer == nullptr) {
            RS_LOGE("RSBufferManager::ReleaseUniOnDrawBuffers layer is nullptr");
            continue;
        }

        auto bufferOwnerCount = layer->PopBufferOwnerCountById(bufferId);
        if (bufferOwnerCount == nullptr) {
            continue;
        }

        RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager::ReleaseUniOnDrawBuffers bufferId %" PRIu64, bufferId);
        AddPendingReleaseBuffer(bufferOwnerCount->bufferId_, uniFence);
        if (!bufferOwnerCount->CheckLastUniBufferOwner(uniBufferCount->bufferId_, screenId)) {
            layer->SetBufferOwnerCount(bufferOwnerCount, false);
        }
        bufferOwnerCount->OnBufferReleased();
    }
    uniBufferCount->uniOnDrawBufferMap_.clear();
}

void RSBufferManager::ReleaseBufferById(uint64_t bufferId)
{
    std::lock_guard<std::mutex> lock(screenNodeBufferReleasedMutex_);
    auto iter = pendingReleaseBuffers_.find(bufferId);
    if (iter == pendingReleaseBuffers_.end()) {
        RS_LOGE("RSBufferManager::ReleaseBufferById not find bufferId:%{public}" PRIu64, bufferId);
        return;
    }
    auto info = iter->second;
    auto consumer = info.consumer_;
    if (consumer == nullptr || info.buffer_ == nullptr) {
        RS_LOGE("RSBufferManager::ReleaseBufferById consumer or info.buffer_ is null");
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager::ReleaseBufferById bufferId %" PRIu64 " Fence %d",
        info.buffer_->GetBufferId(), info.mergedFence_ ? info.mergedFence_->Get() : -1);
    consumer->ReleaseBuffer(info.buffer_, info.mergedFence_);
    pendingReleaseBuffers_.erase(iter);
}

} // OHOS
} // Rosen