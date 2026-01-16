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

#include "rs_composer_context.h"
#include <mutex>
#include "platform/common/rs_log.h"
#include "rs_trace.h"

#undef LOG_TAG
#define LOG_TAG "RSComposerContext"
namespace OHOS {
namespace Rosen {
RSComposerContext::RSComposerContext(const sptr<IRSRenderToComposerConnection>& conn)
{
    rsComposerConnection_ = conn;
    rsLayerTransactionHandler_ = std::make_shared<RSLayerTransactionHandler>();
    rsLayerTransactionHandler_->SetRSComposerConnectionProxy(conn);
}

std::shared_ptr<RSLayerTransactionHandler> RSComposerContext::GetRSLayerTransaction() const
{
    std::unique_lock<std::recursive_mutex> lock(rsLayerTransMutex_);
    return rsLayerTransactionHandler_;
}

void RSComposerContext::AddRSLayer(const std::shared_ptr<RSLayer>& rsLayer)
{
    std::unique_lock<std::mutex> lock(rsLayerMutex_);
    if (rsLayer == nullptr) {
        return;
    }
    if (rsLayers_.count(rsLayer->GetRSLayerId()) > 0) {
        return;
    }
    RS_LOGI("%{public}s rsLayerId: %{public}" PRIu64, __func__, rsLayer->GetRSLayerId());
    rsLayers_.emplace(rsLayer->GetRSLayerId(), rsLayer);
}

void RSComposerContext::RemoveRSLayer(RSLayerId layerId)
{
    std::unique_lock<std::mutex> lock(rsLayerMutex_);
    auto iter = rsLayers_.find(layerId);
    if (iter == rsLayers_.end()) {
        return;
    }
    RS_LOGI("%{public}s rsLayerId: %{public}" PRIu64, __func__, layerId);
    rsLayers_.erase(iter);
}

std::shared_ptr<RSLayer> RSComposerContext::GetRSLayer(RSLayerId rsLayerId) const
{
    std::unique_lock<std::mutex> lock(rsLayerMutex_);
    auto it = rsLayers_.find(rsLayerId);
    if (it != rsLayers_.end()) {
        return it->second.lock();
    }
    return nullptr;
}

void RSComposerContext::DumpLayersInfo(std::string &dumpString)
{
    std::unique_lock<std::mutex> lock(rsLayerMutex_);
    for (auto iter = rsLayers_.begin(); iter != rsLayers_.end(); iter++) {
        auto rsLayer = iter->second.lock();
        if (rsLayer == nullptr ||
            std::find(lastCommitLayersId_.begin(), lastCommitLayersId_.end(),
                rsLayer->GetNodeId()) == lastCommitLayersId_.end()) {
            continue;
        }
        const std::string& name = rsLayer->GetSurfaceName();
        dumpString += "\n surface [" + name + "] NodeId[" + std::to_string(rsLayer->GetNodeId()) + "]";
        dumpString +=  " LayerId[" + std::to_string(rsLayer->GetRSLayerId()) + "]:\n";
        rsLayer->Dump(dumpString);
    }
}

void RSComposerContext::DumpCurrentFrameLayers()
{
    std::unique_lock<std::mutex> lock(rsLayerMutex_);
    for (auto iter = rsLayers_.begin(); iter != rsLayers_.end(); iter++) {
        auto rsLayer = iter->second.lock();
        if (rsLayer != nullptr) {
            rsLayer->DumpCurrentFrameLayer();
        }
    }
}

bool RSComposerContext::CommitLayers(ComposerInfo& composerInfo)
{
    std::unique_lock<std::recursive_mutex> lock(rsLayerTransMutex_);
    if (rsLayerTransactionHandler_ == nullptr) {
        RS_LOGE("%{public}s rsLayerTransactionHandler is nullptr", __func__);
        return false;
    }
    RS_LOGD("%{public}s rsLayers_ size: %{public}zu", __func__, rsLayers_.size());
    return rsLayerTransactionHandler_->CommitRSLayerTransaction(composerInfo);
}


void RSComposerContext::ANCOTransactionOnComplete(const std::shared_ptr<RSLayer>& rsLayer,
    const sptr<SyncFence>& previousReleaseFence)
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

void RSComposerContext::ReleaseLayerBuffers(uint64_t screenId,
    std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>>& timestampVec,
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>>& releaseBufferFenceVec)
{
    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    {
        std::unique_lock<std::mutex> lock(rsLayerMutex_);
        rsLayers = rsLayers_;
        lastCommitLayersId_.clear();
        for (const auto& [id, _1, _2] : releaseBufferFenceVec) {
            lastCommitLayersId_.push_back(id);
        }
    }
    auto layerPresentTimestamp = [](const std::shared_ptr<RSLayer>& layer, const sptr<IConsumerSurface>& cSurface) -> void {
        if (cSurface == nullptr) {
            RS_LOGE("layerPresentTimestamp cSurface is nullptr");
            return;
        }
        if (!layer->GetIsSupportedPresentTimestamp()) {
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
    RS_LOGD("%{public}s screenId: %{public}" PRIu64, __func__, screenId);
    for (const auto& [id, isGraphicPresentTimestamp, graphicPresentTimestamp] : timestampVec) {
        if (rsLayers.count(id) == 0) {
            RS_LOGE("%{public}s has no id: %{public}" PRIu64 " layer", __func__, id);
            continue;
        }
        auto layer = rsLayers[id].lock();
        if (layer == nullptr) {
            RS_LOGE("%{public}s layer is nullptr", __func__);
            continue;
        }
        layer->SetIsSupportedPresentTimestamp(isGraphicPresentTimestamp);
        layer->SetPresentTimestamp(graphicPresentTimestamp);
        layerPresentTimestamp(layer, layer->GetSurface());
    }

    if (onReleaseLayerBuffersCB_) {
        onReleaseLayerBuffersCB_(rsLayers, releaseBufferFenceVec);
    } else {
        RS_LOGE("RSBufferManager onReleaseLayerBuffersCB_ is nullptr");
    }
}

void RSComposerContext::ClearFrameBuffers()
{
    std::unique_lock<std::recursive_mutex> lock(rsLayerTransMutex_);
    if (rsComposerConnection_ == nullptr) {
        RS_LOGE("%{public}s rsComposerConnection_ is nullptr", __func__);
        return;
    }
    rsComposerConnection_->ClearFrameBuffers();
}

void RSComposerContext::CleanLayerBufferBySurfaceId(uint64_t surfaceId)
{
    std::unique_lock<std::recursive_mutex> lock(rsLayerTransMutex_);
    if (rsComposerConnection_ == nullptr) {
        RS_LOGE("%{public}s rsComposerConnection_ is nullptr", __func__);
        return;
    }
    rsComposerConnection_->CleanLayerBufferBySurfaceId(surfaceId);
}

void RSComposerContext::PreAllocProtectedFrameBuffers(const sptr<SurfaceBuffer>& buffer)
{
    std::unique_lock<std::recursive_mutex> lock(rsLayerTransMutex_);
    if (rsComposerConnection_ == nullptr) {
        RS_LOGE("%{public}s rsComposerConnection_ is nullptr", __func__);
        return;
    }
    rsComposerConnection_->PreAllocProtectedFrameBuffers(buffer);
}
} // namespace Rosen
} // namespace OHOS