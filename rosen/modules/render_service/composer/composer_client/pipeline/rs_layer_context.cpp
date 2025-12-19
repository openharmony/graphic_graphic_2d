/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rs_layer_context.h"
#include <mutex>
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSLayerContext::RSLayerContext()
    : rsLayerTransactionHandler_(std::make_shared<RSLayerTransactionHandler>())
{}

std::shared_ptr<RSLayerTransactionHandler> RSLayerContext::GetRSLayerTransaction() const
{
    std::unique_lock<std::recursive_mutex> lock(rsLayerTransMutex_);
    return rsLayerTransactionHandler_;
}

void RSLayerContext::SetRenderComposerClientConnection(const sptr<IRSRenderToComposerConnection>& conn)
{
    std::unique_lock<std::recursive_mutex> lock(rsLayerTransMutex_);
    if (rsLayerTransactionHandler_ == nullptr) {
        RS_LOGE("RSLayerContext::SetRenderComposerClientConnection rsLayerTransactionHandler is nullptr");
        return;
    }
    RS_LOGI("RSLayerContext::SetRenderComposerClientConnection");
    rsComposerConnection_ = conn;
    rsLayerTransactionHandler_->SetRSComposerConnectionProxy(conn);
}

void RSLayerContext::AddRSLayer(const std::shared_ptr<RSLayer>& rsLayer)
{
    std::unique_lock<std::mutex> lock(rsLayerMutex_);
    if (rsLayer == nullptr) {
        return;
    }
    if (rsLayers_.count(rsLayer->GetRSLayerId()) > 0) {
        return;
    }
    RS_LOGI("RSLayerContext::AddRSLayer rsLayerId: %{public}" PRIu64, rsLayer->GetRSLayerId());
    rsLayers_.emplace(rsLayer->GetRSLayerId(), rsLayer);
}

void RSLayerContext::RemoveRSLayer(RSLayerId layerId)
{
    std::unique_lock<std::mutex> lock(rsLayerMutex_);
    auto iter = rsLayers_.find(layerId);
    if (iter == rsLayers_.end()) {
        return;
    }
    RS_LOGI("RSLayerContext::RemoveRSLayer rsLayerId: %{public}" PRIu64, layerId);
    rsLayers_.erase(iter);
}

void RSLayerContext::ClearAllRSLayers()
{
    std::unique_lock<std::mutex> lock(rsLayerMutex_);
    rsLayers_.clear();
}

std::shared_ptr<RSLayer> RSLayerContext::GetRSLayer(RSLayerId rsLayerId) const
{
    std::unique_lock<std::mutex> lock(rsLayerMutex_);
    auto it = rsLayers_.find(rsLayerId);
    if (it != rsLayers_.end()) {
        return it->second.lock();
    }
    return nullptr;
}

void RSLayerContext::DumpLayersInfo(std::string &dumpString)
{
    std::unique_lock<std::mutex> lock(rsLayerMutex_);
    for (auto iter = rsLayers_.begin(); iter != rsLayers_.end(); iter++) {
        auto rsLayer = iter->second.lock();
        if (rsLayer == nullptr) {
            continue;
        }
        const std::string& name = rsLayer->GetSurfaceName();
        dumpString += "\n surface [" + name + "] NodeId[" + std::to_string(rsLayer->GetNodeId()) + "]";
        dumpString +=  " LayerId[" + std::to_string(rsLayer->GetRSLayerId()) + "]:\n";
        rsLayer->Dump(dumpString);
    }
}

void RSLayerContext::DumpCurrentFrameLayers()
{
    std::unique_lock<std::mutex> lock(rsLayerMutex_);
    for (auto iter = rsLayers_.begin(); iter != rsLayers_.end(); iter++) {
        auto rsLayer = iter->second.lock();
        if (rsLayer != nullptr) {
            rsLayer->DumpCurrentFrameLayer();
        }
    }
}

bool RSLayerContext::CommitRSLayer(CommitLayerInfo& commitLayerInfo)
{
    std::unique_lock<std::recursive_mutex> lock(rsLayerTransMutex_);
    if (rsLayerTransactionHandler_ == nullptr) {
        RS_LOGE("RSLayerContext::CommitLayers rsLayerTransactionHandler is nullptr");
        return false;
    }
    RS_LOGD("RSLayerContext::CommitRSLayer rsLayers_ size: %{public}zu", rsLayers_.size());
    return rsLayerTransactionHandler_->CommitRSLayerTransaction(commitLayerInfo);
}


void RSLayerContext::ANCOTransactionOnComplete(const std::shared_ptr<RSLayer>& rsLayer, const sptr<SyncFence>& previousReleaseFence)
{
    if (rsLayer == nullptr) {
        return;
    }
    if (rsLayer->IsAncoNative()) {
        auto consumer = rsLayer->GetSurface();
        auto curBuffer = rsLayer->GetBuffer();
        if (consumer == nullptr || curBuffer == nullptr) {
            return;
        }
        consumer->ReleaseBuffer(curBuffer, previousReleaseFence);
    }
}

void RSLayerContext::ReleaseLayerBuffers(uint64_t screenId,
    std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>>& timestampVec,
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>>& releaseBufferFenceVec)
{
    auto layerPresentTimestamp = [](const std::shared_ptr<RSLayer>& layer, const sptr<IConsumerSurface>& cSurface) -> void {
        if (cSurface == nullptr) {
            RS_LOGE("layerPresentTimestamp cSurface is nullptr");
            return;
        }
        if (!layer->IsSupportedPresentTimestamp()) {
            return;
        }
        const auto& buffer = layer->GetBuffer();
        if (buffer == nullptr) {
             RS_LOGE("layerPresentTimestamp buffer is nullptr");
            return;
        }
        if (cSurface->SetPresentTimestamp(buffer->GetSeqNum(), layer->GetPresentTimestamp()) != GSERROR_OK) {
            RS_LOGE("LayerPresentTimestamp SetPresentTimestamp failed");
        }
    };
    RS_LOGI("RSLayerContext::ReleaseLayerBuffers screenId: %{public}" PRIu64, screenId);
    for (const auto& [id, isGraphicPresentTimestamp, graphicPresentTimestamp] : timestampVec) {
        if (rsLayers_.count(id) == 0) {
            RS_LOGE("RSLayerContext::ReleaseLayerBuffers has no id: %{public}" PRIu64 " layer", id);
            continue;
        }
        auto layer = rsLayers_[id].lock();
        if (layer == nullptr) {
            RS_LOGE("RSLayerContext::ReleaseLayerBuffers layer is nullptr");
            continue;
        }
        layer->SetIsSupportedPresentTimestamp(isGraphicPresentTimestamp);
        layer->SetPresentTimestamp(graphicPresentTimestamp);
        layerPresentTimestamp(layer, layer->GetSurface());
    }

    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> uniBufferCount = nullptr;
    sptr<SyncFence> uniFence = nullptr;
    std::set<uint32_t> decedSet = {};
    for (const auto& [id, buffer, fence] : releaseBufferFenceVec) {
        if (rsLayers_.count(id) == 0) {
            RS_LOGE("RSLayerContext::ReleaseLayerBuffers has no id: %{public}" PRIu64 " layer", id);
            continue;
        }
        auto layer = rsLayers_[id].lock();
        if (layer == nullptr) {
            RS_LOGE("RSLayerContext::ReleaseLayerBuffers layer is nullptr");
            continue;
        }
        if (buffer == nullptr) {
            continue;
        }
        auto seqNum = buffer->GetSeqNum();
        auto bufferOwnerCount = layer->GetSeqNumFromBufferOwnerCounts(seqNum);

        RS_OPTIONAL_TRACE_NAME_FMT("RSBufferManager ReleaseLayerBuffers seqNum %u fence %d",
            uint32_t(seqNum), fence ? fence->Get() : -1);

        if (bufferOwnerCount) {
            if (layer->GetUniRenderFlag()) {
                uniBufferCount = bufferOwnerCount;
                uniFence = fence;
            }

            if (onBufferReleaseFunc_) {
                onBufferReleaseFunc_(seqNum, fence);
            } else {
                RS_LOGE("RSBufferManager onBufferReleaseFunc_ is nullptr");
            }

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
            if (rsLayers_.count(id) == 0) {
                RS_LOGE("RSLayerContext::ReleaseLayerBuffers has no id: %{public}" PRIu64 " layer", id);
                continue;
            }
            auto layer = rsLayers_[id].lock();
            if (layer == nullptr) {
                RS_LOGE("RSLayerContext::ReleaseLayerBuffers layer is nullptr");
                continue;
            }

            auto bufferOwnerCount = layer->GetSeqNumFromBufferOwnerCounts(seqNum);
            if (bufferOwnerCount == nullptr) {
                continue;
            }
            if (onBufferReleaseFunc_) {
                onBufferReleaseFunc_(bufferOwnerCount->seqNum_, uniFence);
            } else {
                RS_LOGE("RSBufferManager onBufferReleaseFunc_ is nullptr");
            }
            if (!bufferOwnerCount->CheckLastUniBufferOwner(uniBufferCount->seqNum_)) {
                layer->SetBufferOwnerCount(bufferOwnerCount);
            }
            bufferOwnerCount->OnBufferReleased();
        }
        uniBufferCount->uniOnDrawBufferMap_.clear();
    }
}

void RSLayerContext::ClearFrameBuffers()
{
    std::unique_lock<std::recursive_mutex> lock(rsLayerTransMutex_);
    if (rsComposerConnection_ == nullptr) {
        RS_LOGE("RSLayerContext::ClearFrameBufferIfNeed rsComposerConnection_ is nullptr");
        return;
    }
    rsComposerConnection_->ClearFrameBuffers();
}

void RSLayerContext::CleanLayerBufferBySurfaceId(uint64_t surfaceId)
{
    std::unique_lock<std::recursive_mutex> lock(rsLayerTransMutex_);
    if (rsComposerConnection_ == nullptr) {
        RS_LOGE("RSLayerContext::CleanLayerBufferBySurfaceId rsComposerConnection_ is nullptr");
        return;
    }
    rsComposerConnection_->CleanLayerBufferBySurfaceId(surfaceId);
}
} // namespace Rosen
} // namespace OHOS