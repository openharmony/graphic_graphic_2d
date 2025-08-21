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
 
void LppVideoHandler::ConsumeAndUpdateLppBuffer(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::shared_ptr<RSSurfaceHandler> surfaceHandler;
    if (UNLIKELY(surfaceNode == nullptr) || (surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler()) == nullptr ||
        surfaceHandler->GetSourceType() != OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO) {
        return;
    }
 
    const auto& consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return;
    }
    lowPowerSurfaceNode_.push_back(surfaceNode);
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
 
void LppVideoHandler::JudgeRsDrawLppState(bool needDrawFrame, bool doDirectComposition)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bool requestVsync = false;
    for (const auto& surfaceNode : lowPowerSurfaceNode_) {
        sptr<IConsumerSurface> consumer;
        if (UNLIKELY(surfaceNode == nullptr) ||
            surfaceNode->GetAbilityState() == RSSurfaceNodeAbilityState::BACKGROUND ||
            surfaceNode->GetMutableRSSurfaceHandler() == nullptr ||
            (consumer = surfaceNode->GetMutableRSSurfaceHandler()->GetConsumer()) == nullptr) {
            continue;
        }
 
        bool isHardware = !(surfaceNode->IsHardwareForcedDisabled());
        bool requestNextVsyncForLpp = false;
        bool isShbDrawLpp = true;
 
        if (!needDrawFrame) {
            // Render nothing to update
            requestNextVsyncForLpp = true;
            isShbDrawLpp = false;
        } else {
            if (!doDirectComposition && !isHardware) {
                // GPU
                requestNextVsyncForLpp = true;
                isShbDrawLpp = false;
            }
            if (doDirectComposition || (!doDirectComposition && isHardware)) {
                // Hareware
                requestNextVsyncForLpp = false;
                isShbDrawLpp = true;
            }
        }
 
        if (hasVirtualMirrorDisplay_.load()) {
            // Virtual screen needs to keep Vsync awake
            RS_TRACE_NAME_FMT("Virtual screen needs to keep Vsync awake");
            requestNextVsyncForLpp = true;
        }
        if (consumer->SetLppDrawSource(isShbDrawLpp, requestNextVsyncForLpp) != GSError::GSERROR_OK) {
            RS_TRACE_NAME_FMT("Stop By SetLppDrawSource");
            requestNextVsyncForLpp = false;
        }
        requestVsync = requestVsync || requestNextVsyncForLpp;
    }
    if (requestVsync) {
        RSMainThread::Instance()->RequestNextVSync();
    }
}
 
void LppVideoHandler::SetHasVirtualMirrorDisplay(bool hasVirtualMirrorDisplay)
{
    hasVirtualMirrorDisplay_.store(hasVirtualMirrorDisplay);
}
 
void LppVideoHandler::ClearLppSurfaceNode()
{
    std::lock_guard<std::mutex> lock(mutex_);
    lowPowerSurfaceNode_.clear();
}
} // namespace OHOS::Rosen