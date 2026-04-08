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
#include <sstream>

namespace OHOS {
namespace Rosen {

constexpr int32_t MAX_BUFFER_FENCE_COUT = 10;

static inline bool TryWaitFences(std::vector<sptr<SyncFence>>& fences)
{
    for (auto iter = fences.begin(); iter != fences.end();) {
        auto fence = *iter;
        // remove obsolete fence
        if (!fence || fence->Get() == -1) {
            iter = fences.erase(iter);
            continue;
        }

        // wait fence, if success remove it from vector
        if (fence->Wait(0) != 0) {
            return false;
        }
        RS_OPTIONAL_TRACE_NAME_FMT("TryWaitFences: wait fence: %d success", fence->Get());
        iter = fences.erase(iter);
    }

    return true;
}

static inline sptr<SyncFence> TryMergeFence(sptr<SyncFence> fence1, sptr<SyncFence> fence2)
{
    sptr<SyncFence> fenceRet = fence2;
    // caller use fence2 not nullptr
    if (fence1 && fence1->Get() != -1 && fence1->Wait(0) != 0) {
        fenceRet = SyncFence::MergeFence("bufferFence", fence1, fence2);
        RS_OPTIONAL_TRACE_NAME_FMT("TryMergeFence %d fence %d success", fence1 ? fence1->Get() : -1, fence2->Get());
    }
    return fenceRet;
}

static inline sptr<SyncFence> TryMergeFence(std::vector<sptr<SyncFence>> fences)
{
    sptr<SyncFence> mergedFence = SyncFence::InvalidFence();
    if (!TryWaitFences(fences)) {
        for (auto& fence : fences) {
            if (fence == nullptr) {
                continue;
            }
            mergedFence = TryMergeFence(mergedFence, fence);
        }
    }
    return mergedFence;
}

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
    fence_ = TryMergeFence(fence_, fence);
}

void RSBufferCollectorHelper::OnCanvasDrawBuffer(sptr<IConsumerSurface> consumer, sptr<SurfaceBuffer> buffer,
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount)
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
        pendingReleaseBuffers_[bufferId] = { consumer, buffer, { fence } };
    } else {
        if (fence != nullptr) {
            // Merge the fences on the GPU and the fences on the DSS;
            // the buffer can only be released when both sides have finished using it.
            iter->second.mergedFences_.push_back(fence);
            if (iter->second.mergedFences_.size() > MAX_BUFFER_FENCE_COUT) {
                TryWaitFences(iter->second.mergedFences_);
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
    if (fence == nullptr) {
        RS_LOGE("RSBufferManager::AddPendingReleaseBuffer(without consumer) fence is null");
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager::AddPendingReleaseBuffer(without consumer) bufferId %" PRIu64
        " fence %d", bufferId, fence->Get());
    std::lock_guard<std::mutex> lock(screenNodeBufferReleasedMutex_);
    auto iter = pendingReleaseBuffers_.find(bufferId);
    if (iter == pendingReleaseBuffers_.end()) {
        pendingReleaseBuffers_[bufferId] = { nullptr, nullptr, { fence } };
    } else {
        iter->second.mergedFences_.push_back(fence);
        if (iter->second.mergedFences_.size() > MAX_BUFFER_FENCE_COUT) {
            TryWaitFences(iter->second.mergedFences_);
        }
    }
}

void RSBufferManager::OnReleaseLayerBuffers(std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>>& rsLayers,
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>>& releaseBufferFenceVec, uint64_t screenId)
{
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> uniBufferCount = nullptr;
    sptr<SyncFence> uniFence = nullptr;
    std::set<uint64_t> decedSet = {};
    for (const auto& [id, buffer, fence] : releaseBufferFenceVec) {
        auto iter = rsLayers.find(id);
        if (iter == rsLayers.end()) {
            RS_LOGE("RSBufferManager::OnReleaseLayerBuffers has no id: %{public}" PRIu64 " layer", id);
            continue;
        }
        auto layer = iter->second.lock();
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
    sptr<SyncFence>& uniFence, std::set<uint64_t>& decedSet,
    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>>& rsLayers, uint64_t screenId)
{
    if (uniBufferCount == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(uniBufferCount->mapMutex_);
    for (const auto& [layerId, bufferIdSet] : uniBufferCount->uniOnDrawBuffersMap_) {
        auto iter = rsLayers.find(layerId);
        if (iter == rsLayers.end()) {
            RS_LOGE("RSBufferManager::ReleaseUniOnDrawBuffers has no layer: %{public}" PRIu64 " layer", layerId);
            continue;
        }
        auto layer = iter->second.lock();
        if (layer == nullptr) {
            RS_LOGE("RSBufferManager::ReleaseUniOnDrawBuffers layer: %{public}" PRIu64 " has been released", layerId);
            continue;
        }

        for (const auto bufferId : bufferIdSet) {
            if (decedSet.find(bufferId) != decedSet.end()) {
                continue;
            }

            auto bufferOwnerCount = layer->PopBufferOwnerCountById(bufferId);
            if (bufferOwnerCount == nullptr) {
                RS_LOGE("RSBufferManager::ReleaseUniOnDrawBuffers not buffer: %{public}" PRIu64
                    " in layer:%{public}" PRIu64, bufferId, layerId);
                continue;
            }

            RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager::ReleaseUniOnDrawBuffers bufferId %" PRIu64
                " in layer:%" PRIu64, bufferId, layerId);
            AddPendingReleaseBuffer(bufferOwnerCount->bufferId_, uniFence);
            if (!bufferOwnerCount->CheckLastUniBufferOwner(uniBufferCount->bufferId_, screenId)) {
                layer->SetBufferOwnerCount(bufferOwnerCount, false);
            }
            bufferOwnerCount->OnBufferReleased();
        }
    }
    uniBufferCount->uniOnDrawBuffersMap_.clear();
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
    auto buffer = info.buffer_.promote();
    if (consumer == nullptr || buffer == nullptr) {
        RS_LOGE("RSBufferManager::ReleaseBufferById consumer or buffer is null");
        return;
    }

    auto mergedFence = TryMergeFence(info.mergedFences_);
    RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager::ReleaseBufferById bufferId %" PRIu64 " Fence %d",
        buffer->GetBufferId(), mergedFence ? mergedFence->Get() : -1);
    consumer->ReleaseBuffer(buffer, mergedFence);
    pendingReleaseBuffers_.erase(iter);
}

#ifndef ROSEN_CROSS_PLATFORM
void RSBufferManager::DumpPendingReleaseBuffers(std::string& output)
{
    std::lock_guard<std::mutex> lock(screenNodeBufferReleasedMutex_);
    std::ostringstream oss;

    oss << "\n=== RSBufferManager::pendingReleaseBuffers_ Dump ===\n";
    oss << "Total entries: " << pendingReleaseBuffers_.size() << "\n";

    if (pendingReleaseBuffers_.empty()) {
        oss << "(empty)\n";
    } else {
        oss << "BufferID\t\tConsumerID\t\tBuffer\t\tWidth\tHeight\tSeqNum\tFenceCount\n";
        oss << "--------\t\t--------\t\t------\t\t-----\t------\t------\t----------\n";
        for (const auto& [bufferId, info] : pendingReleaseBuffers_) {
            oss << bufferId << "\t\t";

            // Consumer info
            if (info.consumer_) {
                oss << info.consumer_->GetUniqueId() << "\t\t";
            } else {
                oss << "null\t\t";
            }

            // Buffer info
            if (auto buffer = info.buffer_.promote()) {
                oss << "valid\t\t";
                oss << buffer->GetWidth() << "\t";
                oss << buffer->GetHeight() << "\t";
                oss << buffer->GetSeqNum() << "\t";
            } else {
                oss << "null\t\t";
                oss << "-\t";
                oss << "-\t";
                oss << "-\t";
            }

            oss << info.mergedFences_.size() << "\n";

            // Dump fence details
            for (size_t i = 0; i < info.mergedFences_.size(); ++i) {
                auto& fence = info.mergedFences_[i];
                oss << "  [" << i << "] fence_fd=" << (fence ? fence->Get() : -1);
                if (fence && fence->Get() != -1) {
                    oss << " (valid)";
                } else if (fence) {
                    oss << " (invalid)";
                } else {
                    oss << " (null)";
                }
                oss << "\n";
            }
        }
    }
    oss << "=== End Dump ===\n";

    output = oss.str();
}
#endif

} // OHOS
} // Rosen