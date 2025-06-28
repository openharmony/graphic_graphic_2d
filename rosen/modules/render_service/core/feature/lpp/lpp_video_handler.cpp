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
#include "platform/common/rs_log.h"
#include "pipeline/main_thread/rs_main_thread.h"
namespace OHOS::Rosen {
LppVideoHandler& LppVideoHandler::Instance()
{
    static LppVideoHandler lppVideoHandler;
    return lppVideoHandler;
}

void LppVideoHandler::AddLppSurfaceNode(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    std::shared_ptr<RSSurfaceHandler> surfaceHandler;
    if (UNLIKELY(surfaceNode == nullptr) || (surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler()) == nullptr ||
        surfaceHandler->GetSourceType() != OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO) {
        return;
    }
    lowPowerSurfaceNode_.push_back(surfaceNode);
}

void LppVideoHandler::ConsumeAndUpdateLppBuffer(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    std::shared_ptr<RSSurfaceHandler> surfaceHandler;
    if (UNLIKELY(surfaceNode == nullptr) || (surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler()) == nullptr ||
        surfaceHandler->GetSourceType() != OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO) {
        return;
    }
 
    const auto& consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return;
    }
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

void LppVideoHandler::JudgeRsDrawLppState(bool isPostUniRender)
{
    RS_TRACE_NAME_FMT("JudgeRsDrawLppState isPostUniRender = %d", isPostUniRender);
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
        RS_TRACE_NAME_FMT("isHardware  =%d", isHardware);

        if (!isPostUniRender || (isPostUniRender && !isHardware)) {
            // GPU or SKIP
            isShbDrawLpp = false;
            requestNextVsyncForLpp = true;
        }
        if (isHardware) {
            // HWC
            isShbDrawLpp = true;
            requestNextVsyncForLpp = false;
        }

        if (hasVirtualMirrorDisplay_.load()) {
            // Virtual screen needs to keep Vsync awake
            RS_TRACE_NAME_FMT("Virtual screen needs to keep Vsync awake");
            requestNextVsyncForLpp = true;
        }
        RS_LOGI("JudgeRsDrawLppState isShbDrawLpp = %d, requestNextVsyncForLpp = %d", isShbDrawLpp, requestNextVsyncForLpp);
        if (consumer->SetLppDrawSource(isShbDrawLpp, requestNextVsyncForLpp) != GSError::GSERROR_OK) {
            RS_TRACE_NAME_FMT("Stop By SetLppDrawSource");
            requestNextVsyncForLpp = false;
        }
        if (!requestNextVsyncForLpp) {
            surfaceNode->GetMutableRSSurfaceHandler()->CleanCache();
        }
        if (requestNextVsyncForLpp) {
            ConsumeAndUpdateLppBuffer(surfaceNode);
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

void LppVideoHandler::ClearLppSufraceNode()
{
    lowPowerSurfaceNode_.clear();
}

LppVideoHandler::~LppVideoHandler()
{
    lowPowerSurfaceNode_.clear();
}

} // namespace OHOS::Rosen