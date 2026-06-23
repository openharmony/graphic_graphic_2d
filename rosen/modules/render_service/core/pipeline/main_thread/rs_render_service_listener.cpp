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

#include "common/rs_optional_trace.h"
#include "common/rs_tunnel_layer_utils.h"
#include "platform/common/rs_log.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "feature/tunnel_layer/rs_tunnel_layer_helper.h"
#include "sync_fence.h"
#include "feature/delegate_composite/rs_delegate_composite_callback_manager.h"
namespace OHOS {
namespace Rosen {

RSRenderServiceListener::~RSRenderServiceListener() {}

RSRenderServiceListener::RSRenderServiceListener(std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode,
    std::weak_ptr<RSSurfaceHandler> surfaceHandler,
    std::shared_ptr<RSComposerClientManager> composerClientManager)
    : surfaceBufferInterface_(std::move(surfaceRenderNode)),
    surfaceHandler_(std::move(surfaceHandler)), composerClientManager_(std::move(composerClientManager))
{
    auto node = surfaceBufferInterface_.lock();
    if (node != nullptr) {
        name_ = node->GetName();
        nodeId_ = node->GetId();
    }
}

void RSRenderServiceListener::OnBufferAvailable()
{
    auto surfaceHandler = surfaceHandler_.lock();
    if (surfaceHandler == nullptr) {
        RS_LOGE("RSRenderServiceListener::OnBufferAvailable surfaceHandler is nullptr");
        return;
    }
    auto node = surfaceBufferInterface_.lock();
    if (node == nullptr) {
        RS_LOGE("RSRenderServiceListener::OnBufferAvailable node is nullptr");
        ConsumeBufferToKeepQueueRunning(surfaceHandler);
        return;
    }
    RS_LOGD("RsDebug RSRenderServiceListener::OnBufferAvailable node id:%{public}" PRIu64, nodeId_);
    surfaceHandler->IncreaseAvailableBuffer();
    auto consumer = surfaceHandler->GetConsumer();
    bool forceRefreshed = node->OnBufferAvailable();
    if (forceRefreshed) {
        RSMainThread::Instance()->ForceRefreshForUni();
        return;
    }

    bool isNewTunnelEnabled = Rosen::IsNewTunnelEnabled();
    auto doFastCompose = CheckFastCompose(consumer);
    if (isNewTunnelEnabled) {
        auto *surfaceRenderNodeRaw = node->AsRSSurfaceRenderNode();
        if (surfaceRenderNodeRaw != nullptr) {
            std::shared_ptr<RSSurfaceRenderNode> surfaceRenderNode(node, surfaceRenderNodeRaw);
            auto handleResult =
                RSTunnelLayerHelper::HandleListenerBuffer(surfaceRenderNode, surfaceHandler, composerClientManager_);
            if (handleResult.needRequestVsync) {
                RSMainThread::Instance()->RequestNextVSync("tunnel-pending-param");
            }
            if (handleResult.committed) {
                return;
            }
        }
    }
    SetBufferInfoAndRequest(surfaceRenderNode_.lock(), surfaceHandler, surfaceHandler->GetConsumer(), doFastCompose);
}

bool RSRenderServiceListener::CheckFastCompose(const sptr<IConsumerSurface>& consumer)
{
    if (consumer == nullptr) {
        return false;
    }
    auto doFastCompose = false;
    bool supportFastCompose = false;
    GSError ret = consumer->GetBufferSupportFastCompose(supportFastCompose);
    if (ret == GSERROR_OK && supportFastCompose) {
        int64_t lastFlushedDesiredPresentTimeStamp = 0;
        ret = consumer->GetLastFlushedDesiredPresentTimeStamp(lastFlushedDesiredPresentTimeStamp);
        if (ret == GSERROR_OK) {
            RS_TRACE_NAME_FMT("RSRenderServiceListener::OnBufferAvailable SupportFastCompose : %d, "
                "bufferTimeStamp : %" PRId64, supportFastCompose, lastFlushedDesiredPresentTimeStamp);
            doFastCompose = RSMainThread::Instance()->CheckFastCompose(lastFlushedDesiredPresentTimeStamp);
        }
    }
    return doFastCompose;
}

void RSRenderServiceListener::SetBufferInfoAndRequest(const std::shared_ptr<RSSurfaceRenderNode> &node,
    const std::shared_ptr<RSSurfaceHandler> &surfaceHandler, const sptr<IConsumerSurface> &consumer,
    bool doFastCompose)
{
    if (doFastCompose) {
        return;
    }
    uint64_t id = 0;
    int64_t lastConsumeTime = 0;
    uint32_t queueSize = 0;
    if (consumer != nullptr) {
        consumer->GetLastConsumeTime(lastConsumeTime);
        id = consumer->GetUniqueId();
        queueSize = consumer->GetQueueSize();
    }
    int32_t bufferCount = surfaceHandler->GetAvailableBufferCount();
    BufferInfo bufferInfo {id, name_, queueSize, bufferCount, lastConsumeTime,
        RSMainThread::Instance()->CheckAdaptiveCompose()};
    RSMainThread::Instance()->SetBufferInfo(bufferInfo);
    if (bufferInfo.isUrgent) {
        RSMainThread::Instance()->RequestNextVSync("UrgentSelfdrawing");
    } else {
        if (!node->GetDelegateMode()) {
            RSMainThread::Instance()->RequestNextVSync("Selfdrawing", 0);
        }
    }
}

void RSRenderServiceListener::OnTunnelHandleChange()
{
    auto surfaceHandler = surfaceHandler_.lock();
    if (surfaceHandler == nullptr) {
        RS_LOGE("RSRenderServiceListener::OnTunnelHandleChange surfaceHandler is nullptr");
        return;
    }
    auto node = surfaceBufferInterface_.lock();
    if (node == nullptr) {
        RS_LOGE("RSRenderServiceListener::OnTunnelHandleChange node is nullptr");
        return;
    }
    node->OnTunnelHandleChange();
    RSMainThread::Instance()->RequestNextVSync();
}

void RSRenderServiceListener::OnCleanCache(uint32_t *bufSeqNum)
{
    auto surfaceHandler = surfaceHandler_.lock();
    if (surfaceHandler == nullptr) {
        RS_LOGE("RSRenderServiceListener::OnCleanCache surfaceHandler is nullptr");
        return;
    }

    std::lock_guard<std::mutex> lock(pendingStateMutex_);
    pendingCallbackBits_ |= PENDING_ON_CLEAN_CACHE_BIT;

    uint64_t curBufferId = 0;
    auto curBuffer = surfaceHandler->GetBuffer();
    if (curBuffer && bufSeqNum) {
        *bufSeqNum = curBuffer->GetSeqNum();
        curBufferId = curBuffer->GetBufferId();
        cleanCacheBufSeqNum_ = *bufSeqNum;
    }
    surfaceHandler->ResetBufferAvailableCount();

    RSMainThread::Instance()->PostTask(
        [surfaceNode = surfaceBufferInterface_, surfaceHandler = surfaceHandler_, curBufferId, nodeId = nodeId_]() {
            auto handler = surfaceHandler.lock();
            if (handler == nullptr) {
                RS_LOGE("RSRenderServiceListener::OnCleanCache handler is nullptr");
                return;
            }
            RS_LOGD("RsDebug RSRenderServiceListener::OnCleanCache in mainthread node id:%{public}" PRIu64, nodeId);
            handler->SetLastBufferId(curBufferId);
            handler->ResetPreBuffer();
            auto node = surfaceNode.lock();
            if (node == nullptr) {
                RS_LOGE("RSRenderServiceListener::OnCleanCache in mainthread node is nullptr");
                return;
            }
            std::set<uint64_t> tmpSet;
            node->OnCleanCache(tmpSet);
            handler->EnqueueAndFlushGPUCacheCleanup(tmpSet);
        });
    RS_TRACE_NAME_FMT("RSRenderServiceListener::OnCleanCache cleanCacheBufSeqNum : %d, nodeId : %" PRId64,
                    cleanCacheBufSeqNum_, nodeId_);
    CleanLayerBufferCache();
}

void RSRenderServiceListener::CleanLayerBufferCache()
{
    auto surfaceHandler = surfaceHandler_.lock();
    if (!surfaceHandler) {
        RS_LOGD("%{public}s: get surfaceHandler fail", __func__);
        return;
    }
    auto consumer = surfaceHandler->GetConsumer();
    if (!consumer) {
        RS_LOGD("%{public}s get consumer fail", __func__);
        return;
    }
    composerClientManager_->CleanLayerBufferBySurfaceId(consumer->GetUniqueId(), nodeId_);
}

void RSRenderServiceListener::OnGoBackground()
{
    RSMainThread::Instance()->PostTask(
        [surfaceRenderNode = surfaceBufferInterface_, surfaceHandler = surfaceHandler_, nodeId = nodeId_]() {
            auto node = surfaceRenderNode.lock();
            auto handler = surfaceHandler.lock();
            if (handler == nullptr) {
                RS_LOGE("RSRenderServiceListener::OnGoBackground handler is nullptr");
                return;
            }
            if (node == nullptr) {
                RS_LOGE("RSRenderServiceListener::OnGoBackground node is nullptr");
                handler->ResetBufferAvailableCount();
                handler->CleanCache();
                return;
            }
            std::set<uint64_t> tmpSet;
            node->NeedClearBufferCache(tmpSet);
            handler->EnqueueAndFlushGPUCacheCleanup(tmpSet);
            handler->ResetBufferAvailableCount();
            handler->CleanCache();
            node->OnSurfaceGoBackground();
        });
}

void RSRenderServiceListener::OnTransformChange()
{
    auto surfaceHandler = surfaceHandler_.lock();
    if (surfaceHandler == nullptr) {
        RS_LOGE("RSRenderServiceListener::OnTransformChange surfaceHandler is nullptr");
        return;
    }

    RSMainThread::Instance()->PostTask([surfaceRenderNode = surfaceBufferInterface_, surfaceHandler = surfaceHandler_,
                                           nodeId = nodeId_, nodeName = name_]() {
        auto node = surfaceRenderNode.lock();
        auto handler = surfaceHandler.lock();
        if (node == nullptr) {
            RS_LOGD("RSRenderServiceListener::OnTransformChange node is nullptr");
            return;
        }
        RS_LOGD("RsDebug RSRenderServiceListener::OnTransformChange node id:%{public}" PRIu64, nodeId);
        node->OnTransformChange();
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name %s, id %" PRIu64 " disable directComposition by transformChange",
            nodeName.c_str(), nodeId);
        handler->SetBufferTransformTypeChanged(true);
    });
}

void RSRenderServiceListener::OnDropBuffer()
{
    auto handler = surfaceHandler_.lock();
    if (handler == nullptr) {
        RS_LOGD("RSRenderServiceListener::OnDropBuffer handler is nullptr");
        return;
    }

    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderServiceListener::OnDropBuffer node id:%{public}" PRIu64, nodeId_);
    RS_LOGD("RsDebug RSRenderServiceListener::OnDropBuffer node id:%{public}" PRIu64, nodeId_);
    handler->SetBufferDropped(true);
}
 
void RSRenderServiceListener::ConsumeBufferToKeepQueueRunning(std::shared_ptr<RSSurfaceHandler>& surfaceHandler)
{
    surfaceHandler->IncreaseAvailableBuffer();
    if (surfaceHandler->GetAvailableBufferCount() > 0) {
        std::weak_ptr<RSSurfaceHandler> weakHandler = surfaceHandler;
        RSMainThread::Instance()->PostTask([weakHandler, nodeId = nodeId_]() {
            RS_TRACE_NAME_FMT("OnBufferAvailable acquire buffer begin nodeId: %" PRId64, nodeId);
            auto handler = weakHandler.lock();
            if (handler == nullptr) {
                RS_LOGD("RSRenderServiceListener::ConsumeBufferToKeepQueueRunning PostTask handler is nullptr");
                return;
            }
            const auto &consumer = handler->GetConsumer();
            if (consumer == nullptr) {
                RS_LOGD("RSRenderServiceListener::ConsumeBufferToKeepQueueRunning PostTask consumer is nullptr");
                return;
            }

            IConsumerSurface::AcquireBufferReturnValue returnValue;
            auto ret = consumer->AcquireBuffer(returnValue, 0, false);
            if (ret != SURFACE_ERROR_OK) {
                RS_LOGE("RSRenderServiceListener::ConsumeBufferToKeepQueueRunning PostTask AcquireBuffer failed, "
                        "nodeId:%{public}" PRIu64 ", ret:%{public}d", nodeId, ret);
                return;
            }
            handler->SetAvailableBufferCount(static_cast<int32_t>(consumer->GetAvailableBufferCount()));
            if (returnValue.buffer == nullptr) {
                return;
            }

            auto surfaceBuffer = std::make_shared<RSSurfaceHandler::SurfaceBufferEntry>();
            surfaceBuffer->buffer = returnValue.buffer;
            surfaceBuffer->acquireFence = returnValue.fence;
            surfaceBuffer->timestamp = returnValue.timestamp;
            surfaceBuffer->RegisterReleaseBufferListener([](uint64_t bufferId) {
                RSUniRenderThread::Instance().ReleaseBufferById(bufferId);
            });
            RSUniRenderThread::Instance().AddPendingReleaseBuffer(consumer, surfaceBuffer->buffer,
                SyncFence::InvalidFence(), surfaceBuffer->bufferOwnerCount_);
            handler->ConsumeAndUpdateBuffer(*surfaceBuffer);

            ret = consumer->ReleaseBuffer(returnValue.buffer, returnValue.fence);
            if (ret != SURFACE_ERROR_OK) {
                RS_LOGE("RSRenderServiceListener::ConsumeBufferToKeepQueueRunning PostTask ReleaseBuffer failed, "
                        "nodeId:%{public}" PRIu64 ", ret:%{public}d", nodeId, ret);
                return;
            }
        });
    }
    return;
}

void RSRenderServiceListener::ProcessPendingCallbacks()
{
    bool isInterfaceDirty = false;
    uint8_t pendingCallbackBits = 0;
    uint32_t cleanCacheBufSeqNum = 0;
    {
        std::lock_guard<std::mutex> lock(pendingStateMutex_);
        if (!isInterfaceDirty_ || pendingCallbackBits_ == 0) {
            return;
        }
        isInterfaceDirty = isInterfaceDirty_;
        pendingCallbackBits = pendingCallbackBits_;
        cleanCacheBufSeqNum = cleanCacheBufSeqNum_;
        pendingCallbackBits_ = 0;
        isInterfaceDirty_ = false;
    }

    auto surfaceHandler = surfaceHandler_.lock();
    if (surfaceHandler == nullptr) {
        return;
    }

    if ((pendingCallbackBits & PENDING_ON_CLEAN_CACHE_BIT) != 0) {
        OnCleanCache(&cleanCacheBufSeqNum);
    }
}

void RSRenderServiceListener::SetRSSurfaceBufferInterface(
    std::weak_ptr<RSSurfaceBufferInterface> surfaceBufferInterface)
{
    RS_TRACE_NAME_FMT("RSRenderServiceListener::SetRSSurfaceBufferInterface nodeId : %" PRIu64, nodeId_);
    surfaceBufferInterface_ = std::move(surfaceBufferInterface);
    {
        std::lock_guard<std::mutex> lock(pendingStateMutex_);
        isInterfaceDirty_ = true;
    }
    ProcessPendingCallbacks();
}

void RSRenderServiceListener::OnCleanCacheForBufferInfoMap(std::vector<CleanCacheBufferInfo> &infos)
{
#ifndef ROSEN_CROSS_PLATFORM
    auto node = surfaceRenderNode_.lock();
    if (node == nullptr || !node->GetDelegateMode()) {
        return;
    }
    auto surfaceHandler = node->GetRSSurfaceHandler();
    if (!surfaceHandler) {
        return;
    }
    auto consumer = surfaceHandler->GetConsumer();
    if (!consumer) {
        return;
    }
    RsDelegateCompositeCallbackManager::GetInstance().OnCleanCacheForBufferInfoMap(
        infos, node->GetId(), consumer->GetUniqueId());
#endif
}

bool RSRenderServiceListener::IsNeedBufferInfo()
{
    auto node = surfaceRenderNode_.lock();
    if (node == nullptr) {
        return false;
    }
    return node->GetDelegateMode();
}
} // namespace Rosen
} // namespace OHOS
