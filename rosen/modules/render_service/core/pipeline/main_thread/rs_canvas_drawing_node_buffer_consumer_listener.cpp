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

#include "pipeline/main_thread/rs_canvas_drawing_node_buffer_consumer_listener.h"

#include "common/rs_optional_trace.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSCanvasDrawingNodeBufferConsumerListener::RSCanvasDrawingNodeBufferConsumerListener(
    std::weak_ptr<RSContext> rsContext, NodeId nodeId)
    : rsContext_(rsContext), nodeId_(nodeId)
{}

RSCanvasDrawingNodeBufferConsumerListener::~RSCanvasDrawingNodeBufferConsumerListener() {}

void RSCanvasDrawingNodeBufferConsumerListener::OnBufferAvailable()
{
    auto rsContext = rsContext_.lock();
    if (rsContext == nullptr) {
        RS_LOGE("RSCanvasDrawingNodeBufferConsumerListener::OnBufferAvailable: null rsContext, nodeId=%{public}" PRIu64,
            nodeId_);
        return;
    }

    auto surfaceHandler = GetSurfaceHandler(rsContext);
    if (surfaceHandler == nullptr) {
        RS_LOGE("RSCanvasDrawingNodeBufferConsumerListener::OnBufferAvailable: null surfaceHandler, nodeId=%{public}"
            PRIu64, nodeId_);
        return;
    }

    surfaceHandler->IncreaseAvailableBuffer();
    if (surfaceHandler->GetAvailableBufferCount() > 1) {
        std::weak_ptr<RSSurfaceHandler> weakHandler = surfaceHandler;
        RSMainThread::Instance()->PostTask(
            [weakHandler, nodeId = nodeId_]() { DropFirstFlushedBuffer(weakHandler, nodeId); });
    }
    rsContext->RequestVsync();
}

void RSCanvasDrawingNodeBufferConsumerListener::OnTunnelHandleChange() {}

void RSCanvasDrawingNodeBufferConsumerListener::OnTransformChange() {}

void RSCanvasDrawingNodeBufferConsumerListener::OnCleanCache(uint32_t* bufSeqNum)
{
    CleanBuffers();
}

void RSCanvasDrawingNodeBufferConsumerListener::OnGoBackground()
{
    CleanBuffers();
}

void RSCanvasDrawingNodeBufferConsumerListener::CleanBuffers()
{
    auto rsContext = rsContext_.lock();
    if (rsContext == nullptr) {
        RS_LOGE("RSCanvasDrawingNodeBufferConsumerListener::CleanBuffers: null rsContext, nodeId=%{public}" PRIu64,
            nodeId_);
        return;
    }

    auto surfaceHandler = GetSurfaceHandler(rsContext);
    if (surfaceHandler == nullptr) {
        RS_LOGE("RSCanvasDrawingNodeBufferConsumerListener::CleanBuffers: null surfaceHandler, nodeId=%{public}" PRIu64,
            nodeId_);
        return;
    }

    std::weak_ptr<RSSurfaceHandler> weakHandler = surfaceHandler;
    RSMainThread::Instance()->PostTask([weakHandler]() {
        auto surfaceHandler = weakHandler.lock();
        if (surfaceHandler == nullptr) {
            return;
        }
        const auto& consumer = surfaceHandler->GetConsumer();
        if (consumer == nullptr) {
            return;
        }
        std::set<uint64_t> cacheSet;
        CollectBuffersForClean(cacheSet, surfaceHandler);
        surfaceHandler->EnqueueAndFlushGPUCacheCleanup(cacheSet);
        surfaceHandler->CleanCache();
        surfaceHandler->SetAvailableBufferCount(static_cast<int32_t>(consumer->GetAvailableBufferCount()));
    });
}

void RSCanvasDrawingNodeBufferConsumerListener::CollectBuffersForClean(
    std::set<uint64_t>& bufferCacheSet, std::shared_ptr<RSSurfaceHandler> surfaceHandler)
{
    for (const auto& buffer : { surfaceHandler->GetBuffer(), surfaceHandler->GetPreBuffer() }) {
        if (buffer != nullptr) {
            auto bufferId = buffer->GetBufferId();
            auto bufferHandle = buffer->GetBufferHandle();
            bufferCacheSet.insert(bufferId);
            RS_OPTIONAL_TRACE_NAME_FMT(
                "RSCanvasDrawingNodeBufferConsumerListener::CollectBuffersForClean insert buffer, seqNum=%" PRIu64
                ", fd=%d", bufferId, bufferHandle ? bufferHandle->fd : 0);
            RS_LOGD("RSCanvasDrawingNodeBufferConsumerListener::CollectBuffersForClean insert buffer, "
                "seqNum=%{public}" PRIu64 ", fd=%{public}d", bufferId, bufferHandle ? bufferHandle->fd : 0);
        }
    }
}

std::shared_ptr<RSSurfaceHandler> RSCanvasDrawingNodeBufferConsumerListener::GetSurfaceHandler(
    std::shared_ptr<RSContext> rsContext)
{
    std::shared_ptr<RSSurfaceHandler> surfaceHandler = nullptr;
    auto node = node_.lock();
    if (node == nullptr) {
        node = rsContext->GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(nodeId_);
    }
    if (node != nullptr) {
        node_ = node;
        surfaceHandler = node->GetMutableSurfaceHandler();
    } else {
        RS_LOGE("RSCanvasDrawingNodeBufferConsumerListener::GetSurfaceHandler: null node, nodeId=%{public}" PRIu64,
            nodeId_);
        surfaceHandler = rsContext->GetMutableNodeMap().GetSurfaceHandler(nodeId_, false);
    }
    return surfaceHandler;
}

void RSCanvasDrawingNodeBufferConsumerListener::DropFirstFlushedBuffer(
    std::weak_ptr<RSSurfaceHandler> weakHandler, NodeId nodeId)
{
    auto surfaceHandler = weakHandler.lock();
    if (surfaceHandler == nullptr) {
        return;
    }
    if (surfaceHandler->GetAvailableBufferCount() <= 1) {
        return;
    }
    const auto& consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return;
    }
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    auto ret = consumer->AcquireBuffer(returnValue, 0, false);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSCanvasDrawingNodeBufferConsumerListener::DropFirstFlushedBuffer: AcquireBuffer fail, "
            "ret=%{public}u, nodeId=%{public}" PRIu64, ret, nodeId);
        return;
    }
    surfaceHandler->SetAvailableBufferCount(static_cast<int32_t>(consumer->GetAvailableBufferCount()));
    if (returnValue.buffer == nullptr) {
        return;
    }
    ret = consumer->ReleaseBuffer(returnValue.buffer, returnValue.fence);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSCanvasDrawingNodeBufferConsumerListener::DropFirstFlushedBuffer: ReleaseBuffer fail, "
            "ret=%{public}u, nodeId=%{public}" PRIu64, ret, nodeId);
    }
}
} // namespace Rosen
} // namespace OHOS
