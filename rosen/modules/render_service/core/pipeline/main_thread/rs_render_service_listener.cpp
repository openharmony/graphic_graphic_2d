/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "pipeline/main_thread/rs_render_service_listener.h"

#include "platform/common/rs_log.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "frame_report.h"
#include "sync_fence.h"
#include "common/rs_optional_trace.h"
#include "screen_manager/rs_screen_manager.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"
namespace OHOS {
namespace Rosen {

RSRenderServiceListener::~RSRenderServiceListener() {}

RSRenderServiceListener::RSRenderServiceListener(std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode)
    : surfaceRenderNode_(surfaceRenderNode)
{}

void RSRenderServiceListener::OnBufferAvailable()
{
    auto node = surfaceRenderNode_.lock();
    if (node == nullptr) {
        RS_LOGD("RSRenderServiceListener::OnBufferAvailable node is nullptr");
        return;
    }
    RS_LOGD("RsDebug RSRenderServiceListener::OnBufferAvailable node id:%{public}" PRIu64, node->GetId());
    auto surfaceHandler = node->GetMutableRSSurfaceHandler();
    surfaceHandler->IncreaseAvailableBuffer();
    auto consumer = surfaceHandler->GetConsumer();
    if (consumer) {
        uint64_t uniqueId = consumer->GetUniqueId();
        bool isActiveGame = FrameReport::GetInstance().IsActiveGameWithUniqueId(uniqueId);
        if (isActiveGame) {
            std::string name = node->GetName();
            FrameReport::GetInstance().SetPendingBufferNum(name, surfaceHandler->GetAvailableBufferCount());
        }
    }

    if (!node->IsNotifyUIBufferAvailable()) {
        // Only ipc for one time.
        RS_LOGD("RsDebug RSRenderServiceListener::OnBufferAvailable id = %{public}" PRIu64 " Notify"
            " UI buffer available", node->GetId());
        node->NotifyUIBufferAvailable();
    }
    if (node->GetIsTextureExportNode()) {
        RS_LOGD("RsDebug RSRenderServiceListener::OnBufferAvailable id = %{public}" PRIu64 " Notify"
            " RT buffer available", node->GetId());
        node->NotifyRTBufferAvailable(node->GetIsTextureExportNode());
    }
    if (ForceRefresh(node)) {
        return;
    }

    if (consumer) {
        bool supportFastCompose = false;
        GSError ret =  consumer->GetBufferSupportFastCompose(supportFastCompose);
        if (ret == GSERROR_OK && supportFastCompose) {
            int64_t lastFlushedDesiredPresentTimeStamp = 0;
            ret = consumer->GetLastFlushedDesiredPresentTimeStamp(lastFlushedDesiredPresentTimeStamp);
            if (ret == GSERROR_OK) {
                RS_TRACE_NAME_FMT("RSRenderServiceListener::OnBufferAvailable SupportFastCompose : %d, " \
                "bufferTimeStamp : %" PRId64, supportFastCompose, lastFlushedDesiredPresentTimeStamp);
                RSMainThread::Instance()->CheckFastCompose(lastFlushedDesiredPresentTimeStamp);
                return;
            }
        }
    }
    SetBufferInfoAndRequest(node, surfaceHandler, surfaceHandler->GetConsumer());
}

void RSRenderServiceListener::SetBufferInfoAndRequest(const std::shared_ptr<RSSurfaceRenderNode> &node,
    const std::shared_ptr<RSSurfaceHandler> &surfaceHandler, const sptr<IConsumerSurface> &consumer)
{
    uint64_t id = 0;
    int64_t lastConsumeTime = 0;
    uint32_t queueSize = 0;
    if (consumer != nullptr) {
        consumer->GetLastConsumeTime(lastConsumeTime);
        id = consumer->GetUniqueId();
        queueSize = consumer->GetQueueSize();
    }
    int32_t bufferCount = surfaceHandler->GetAvailableBufferCount();
    std::string name = node->GetName();
    if (RSMainThread::Instance()->CheckAdaptiveCompose()) {
        RSMainThread::Instance()->SetBufferInfo(id, name, queueSize, bufferCount, lastConsumeTime, true);
        RSMainThread::Instance()->RequestNextVSync("UrgentSelfdrawing");
    } else {
        RSMainThread::Instance()->SetBufferInfo(id, name, queueSize, bufferCount, lastConsumeTime, false);
        RSMainThread::Instance()->RequestNextVSync("selfdrawing");
    }
}

void RSRenderServiceListener::OnTunnelHandleChange()
{
    auto node = surfaceRenderNode_.lock();
    if (node == nullptr) {
        RS_LOGE("RSRenderServiceListener::OnTunnelHandleChange node is nullptr");
        return;
    }
    node->SetTunnelHandleChange(true);
    if (!node->IsNotifyUIBufferAvailable()) {
        // Only ipc for one time.
        RS_LOGD("RsDebug RSRenderServiceListener::OnTunnelHandleChange id = %{public}" PRIu64 " Notify"
            " UI buffer available", node->GetId());
        node->NotifyUIBufferAvailable();
    }
    RSMainThread::Instance()->RequestNextVSync();
}

void RSRenderServiceListener::OnCleanCache(uint32_t *bufSeqNum)
{
    auto node = surfaceRenderNode_.lock();
    if (node == nullptr) {
        RS_LOGD("RSRenderServiceListener::OnCleanCache node is nullptr");
        return;
    }
    RS_LOGD("RsDebug RSRenderServiceListener::OnCleanCache node id:%{public}" PRIu64, node->GetId());

    auto surfaceHandler = node->GetRSSurfaceHandler();
    if (surfaceHandler) {
        auto curBuffer = surfaceHandler->GetBuffer();
        if (curBuffer && bufSeqNum) {
            *bufSeqNum = curBuffer->GetSeqNum();
        }
        surfaceHandler->ResetBufferAvailableCount();
    }
    std::weak_ptr<RSSurfaceRenderNode> surfaceNode = surfaceRenderNode_;
    RSMainThread::Instance()->PostTask([surfaceNode]() {
        auto node = surfaceNode.lock();
        if (node == nullptr) {
            RS_LOGD("RSRenderServiceListener::OnCleanCache node is nullptr");
            return;
        }
        auto surfaceHandler = node->GetRSSurfaceHandler();
        if (surfaceHandler == nullptr) {
            RS_LOGD("RSRenderServiceListener::OnCleanCache surfaceHandler is nullptr");
            return;
        }
        RS_LOGD("RsDebug RSRenderServiceListener::OnCleanCache in mainthread node id:%{public}" PRIu64, node->GetId());
        surfaceHandler->ResetPreBuffer();
        std::set<uint32_t> tmpSet;
        node->NeedClearPreBuffer(tmpSet);
        RSMainThread::Instance()->AddToUnmappedCacheSet(tmpSet);
    });

    CleanLayerBufferCache();
}

void RSRenderServiceListener::CleanLayerBufferCache()
{
    auto node = surfaceRenderNode_.lock();
    if (!node) {
        RS_LOGD("%{public}s: get node fail", __func__);
        return;
    }
    auto surfaceHandler = node->GetRSSurfaceHandler();
    if (!surfaceHandler) {
        RS_LOGD("%{public}s: get surfaceHandler fail", __func__);
        return;
    }
    auto consumer = surfaceHandler->GetConsumer();
    if (!consumer) {
        RS_LOGD("%{public}s get consumer fail", __func__);
        return;
    }
    uint64_t surfaceId = consumer->GetUniqueId();
    RSHardwareThread::Instance().PostTask([surfaceId]() {
        sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
        if (!screenManager) {
            RS_LOGE("RSRenderServiceListener get screenManager fail!");
            return;
        }
        std::vector<ScreenId> screenIds = screenManager->GetAllScreenIds();
        for (size_t i = 0; i < screenIds.size(); ++i) {
            auto output = screenManager->GetOutput(screenIds[i]);
            if (output == nullptr) {
                continue;
            }
            output->CleanLayerBufferBySurfaceId(surfaceId);
        }
    });
}

void RSRenderServiceListener::OnGoBackground()
{
    std::weak_ptr<RSSurfaceRenderNode> surfaceNode = surfaceRenderNode_;
    RSMainThread::Instance()->PostTask([surfaceNode]() {
        auto node = surfaceNode.lock();
        if (node == nullptr) {
            RS_LOGD("RSRenderServiceListener::OnBufferAvailable node is nullptr");
            return;
        }
        auto surfaceHandler = node->GetMutableRSSurfaceHandler();
        RS_LOGD("RsDebug RSRenderServiceListener::OnGoBackground node id:%{public}" PRIu64, node->GetId());
        std::set<uint32_t> tmpSet;
        node->NeedClearBufferCache(tmpSet);
        RSMainThread::Instance()->AddToUnmappedCacheSet(tmpSet);
        surfaceHandler->ResetBufferAvailableCount();
        surfaceHandler->CleanCache();
        node->UpdateBufferInfo(nullptr, {}, nullptr, nullptr);
        node->SetNotifyRTBufferAvailable(false);
        node->SetContentDirty();
        node->ResetHardwareEnabledStates();
    });
}

void RSRenderServiceListener::OnTransformChange()
{
    std::weak_ptr<RSSurfaceRenderNode> surfaceNode = surfaceRenderNode_;
    RSMainThread::Instance()->PostTask([surfaceNode]() {
        auto node = surfaceNode.lock();
        if (node == nullptr) {
            RS_LOGD("RSRenderServiceListener::OnTransformChange node is nullptr");
            return;
        }
        RS_LOGD("RsDebug RSRenderServiceListener::OnTransformChange node id:%{public}" PRIu64, node->GetId());
        node->SetContentDirty();
        node->SetDoDirectComposition(false);
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name %s, id %" PRIu64 " disable directComposition by transformChange",
            node->GetName().c_str(), node->GetId());
        if (node->GetRSSurfaceHandler() != nullptr) {
            node->GetRSSurfaceHandler()->SetBufferTransformTypeChanged(true);
        }
    });
}

bool RSRenderServiceListener::ForceRefresh(std::shared_ptr<RSSurfaceRenderNode> &node)
{
    if (node->IsLayerTop()) {
        // Ensure that ......
        RSMainThread::Instance()->ForceRefreshForUni();
        return true;
    }
    if ((node->GetAncoFlags() & static_cast<uint32_t>(AncoFlags::FORCE_REFRESH)) != 0) {
        node->SetAncoFlags(node->GetAncoFlags() & (~static_cast<uint32_t>(AncoFlags::FORCE_REFRESH)));
        RS_TRACE_NAME_FMT("AncoForceRefresh id %lld", node->GetId());
        RS_LOGD("AncoForceRefresh id %{public}" PRIu64 "", node->GetId());
        RSMainThread::Instance()->ForceRefreshForUni();
        return true;
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS