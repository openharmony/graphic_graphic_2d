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
#include "feature/lpp/lpp_video_handler.h"

#include "rs_trace.h"

#include "common/rs_optional_trace.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "platform/common/rs_log.h"
namespace OHOS::Rosen {
LppVideoHandler& LppVideoHandler::Instance()
{
    static LppVideoHandler instance;
    return instance;
}

void LppVideoHandler::ConsumeAndUpdateLppBuffer(
    uint64_t vsyncId, const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::shared_ptr<RSSurfaceHandler> surfaceHandler;
    bool isInvalidNode = UNLIKELY(surfaceNode == nullptr) ||
                         surfaceNode->GetAbilityState() == RSSurfaceNodeAbilityState::BACKGROUND ||
                         (surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler()) == nullptr ||
                         surfaceHandler->GetConsumer() == nullptr;
    if (isInvalidNode) {
        return;
    }
    const auto& consumer = surfaceHandler->GetConsumer();
    surfaceHandler->SetSourceType(static_cast<uint32_t>(consumer->GetSurfaceSourceType()));
    if (consumer->GetSurfaceSourceType() != OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO) {
        return;
    }
    bool needRemoveTopNode = lppConsumerMap_.find(vsyncId) == lppConsumerMap_.end() &&
                             lppConsumerMap_.size() >= LPP_SURFACE_NODE_MAX_SIZE;
    if (needRemoveTopNode) {
        RS_TRACE_NAME_FMT("ConsumeAndUpdateLowPowerBuffer erase Node Id = %ld", lppConsumerMap_.begin()->first);
        lppConsumerMap_.erase(lppConsumerMap_.begin());
    }
    lppConsumerMap_[vsyncId].push_back(consumer);
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
    int64_t timestamp = 0;
    std::vector<Rect> damages;
    consumer->AcquireBuffer(buffer, acquireFence, timestamp, damages, true);
    if (buffer == nullptr) {
        return;
    }
    std::shared_ptr<RSSurfaceHandler::SurfaceBufferEntry> surfaceBuffer =
        std::make_shared<RSSurfaceHandler::SurfaceBufferEntry>();
    surfaceBuffer->buffer = buffer;
    surfaceBuffer->acquireFence = acquireFence;
    surfaceBuffer->timestamp = timestamp;
    RSBaseRenderUtil::MergeBufferDamages(surfaceBuffer->damageRect, damages);
    if (surfaceBuffer->damageRect.h <= 0 || surfaceBuffer->damageRect.w <= 0) {
        RS_LOGW("RsDebug ConsumerLowPowerBuffer(id: %{public}" PRIu64 ") buffer damage is invalid",
            surfaceHandler->GetNodeId());
    }
    surfaceBuffer->damageRect.y =
        surfaceBuffer->buffer->GetHeight() - surfaceBuffer->damageRect.y - surfaceBuffer->damageRect.h;
    surfaceHandler->ConsumeAndUpdateBuffer(*surfaceBuffer);
    RS_TRACE_NAME_FMT("ConsumeAndUpdateLowPowerBuffer node: [%lu]", surfaceHandler->GetNodeId());
    surfaceBuffer = nullptr;
}

void LppVideoHandler::JudgeRequestVsyncForLpp(uint64_t vsyncId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RS_TRACE_NAME_FMT("JudgeRequestVsyncForLpp vsyncId = %ld", vsyncId);
    if (lppConsumerMap_.find(vsyncId) == lppConsumerMap_.end()) {
        return;
    }
    // When first awakened
    if (!isRequestedVsync_ && lppRsState_ == LppState::LPP_LAYER) {
        lppRsState_ = LppState::UNKOWN;
    }

    if (lppConsumerMap_.size() >= LPP_SURFACE_NODE_MAX_SIZE - 1 && lppRsState_ == LppState::UNKOWN) {
        RS_TRACE_NAME("JudgeRequestVsyncForLpp skip lpp");
        lppConsumerMap_.clear();
        isRequestedVsync_ = false;
        return;
    }
    bool isRsDrawLpp =
        lppRsState_ == LppState::UNKOWN || lppRsState_ == LppState::UNI_RENDER || hasVirtualMirrorDisplay_.load();
    bool isStopVsyncForCount = true;
    bool isForceRequestVsync = false;
    for (const auto& lppConsumer : lppConsumerMap_[vsyncId]) {
        auto consumer = lppConsumer.promote();
        if (consumer == nullptr) {
            continue;
        }
        auto ret = consumer->SetLppDrawSource(lppShbState_ == LppState::LPP_LAYER, lppRsState_ != LppState::LPP_LAYER);
        bool isStopLppVsyncForCount = false;
        if (ret == GSERROR_NO_CONSUMER) {
            RS_TRACE_NAME("Force RequiestNextVsync for comsume lpp");
            isForceRequestVsync = true;
        } else if (ret != GSERROR_OK) {
            RS_TRACE_NAME("Stop By SetLppDrawSource");
            isStopLppVsyncForCount = true;
        }
        isStopVsyncForCount = isStopVsyncForCount && isStopLppVsyncForCount;
    }
    RS_TRACE_NAME_FMT("JudgeRequestVsyncForLpp vsyncId: %ld, [%d, %d, %d]", vsyncId, lppRsState_,
        hasVirtualMirrorDisplay_.load(), isStopVsyncForCount);

    isRequestedVsync_ = (isRsDrawLpp && !isStopVsyncForCount) || isForceRequestVsync;
    if (isRequestedVsync_) {
        RSMainThread::Instance()->RequestNextVSync();
    } else {
        lppConsumerMap_.clear();
    }
}

void LppVideoHandler::AddLppLayerId(const std::vector<RSLayerPtr>& layers)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        lppLayerId_.clear();
    }
    for (const auto& layer : layers) {
        bool isLppLayer = layer != nullptr &&
            layer->GetTunnelLayerId() != 0 && layer->GetTunnelLayerProperty() == LPP_LAYER_PROPERTY;
        if (!isLppLayer) {
            continue;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        lppLayerId_.insert(layer->GetNodeId());
    }
}

void LppVideoHandler::SetHasVirtualMirrorDisplay(bool hasVirtualMirrorDisplay)
{
    hasVirtualMirrorDisplay_.store(hasVirtualMirrorDisplay);
}

void LppVideoHandler::RemoveLayerId(const std::vector<RSLayerPtr>& layers)
{
    for (const auto& layer : layers) {
        bool isLppLayer = layer != nullptr &&
            layer->GetTunnelLayerId() != 0 && layer->GetTunnelLayerProperty() == LPP_LAYER_PROPERTY;
        if (!isLppLayer) {
            continue;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        lppLayerId_.erase(layer->GetNodeId());
    }
}

void LppVideoHandler::JudgeLppLayer(uint64_t vsyncId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RS_TRACE_NAME_FMT("JudgeLppLayer vsyncId: %ld", vsyncId);
    if (lppConsumerMap_.find(vsyncId) == lppConsumerMap_.end()) {
        return;
    }
    bool hasLppLayer = !lppLayerId_.empty();
    lppShbState_ = hasLppLayer ? LppState::LPP_LAYER : LppState::UNI_RENDER;
    size_t lppNodeSize = lppConsumerMap_[vsyncId].size();
    for (const auto& lppConsumer : lppConsumerMap_[vsyncId]) {
        auto consumer = lppConsumer.promote();
        if (consumer == nullptr) {
            continue;
        }
        auto ret = consumer->SetLppDrawSource(hasLppLayer, lppRsState_ != LppState::LPP_LAYER);
        if (ret != GSERROR_NO_CONSUMER && ret != GSERROR_OK) {
            RS_TRACE_NAME("JudgeLppLayer Stop By SetLppDrawSource");
            lppNodeSize--;
        }
    }
    bool isAllLppLayer = lppNodeSize != 0 && lppNodeSize == lppLayerId_.size();
    LppState lastLppRsState = lppRsState_;
    lppRsState_ = isAllLppLayer ? LppState::LPP_LAYER : LppState::UNI_RENDER;
    if (lastLppRsState == LppState::LPP_LAYER && lppRsState_ == LppState::UNI_RENDER) {
        RSMainThread::Instance()->RequestNextVSync();
        isRequestedVsync_ = true;
    }
    RS_TRACE_NAME_FMT("JudgeLppLayer lppState: [%d, %d]", hasLppLayer, isAllLppLayer);
    for (auto it = lppConsumerMap_.begin(); it != lppConsumerMap_.end();) {
        if (it->first <= vsyncId) {
            it = lppConsumerMap_.erase(it);
        } else {
            break;
        }
    }
}
} // namespace OHOS::Rosen