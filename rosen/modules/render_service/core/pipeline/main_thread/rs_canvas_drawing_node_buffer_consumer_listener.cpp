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
#include "pipeline/render_thread/rs_base_surface_util.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSCanvasDrawingNodeBufferConsumerListener::RSCanvasDrawingNodeBufferConsumerListener(
    std::weak_ptr<RSSurfaceHandler> surfaceHandler, NodeId nodeId)
    : surfaceHandler_(surfaceHandler), nodeId_(nodeId)
{}

RSCanvasDrawingNodeBufferConsumerListener::~RSCanvasDrawingNodeBufferConsumerListener() {}

void RSCanvasDrawingNodeBufferConsumerListener::OnBufferAvailable()
{
    auto surfaceHandler = surfaceHandler_.lock();
    if (surfaceHandler == nullptr) {
        RS_LOGE("RSCanvasDrawingNodeBufferConsumerListener::OnBufferAvailable: null surfaceHandler, nodeId=%{public}"
            PRIu64, nodeId_);
        return;
    }
    surfaceHandler->IncreaseAvailableBuffer();
    if (surfaceHandler->GetAvailableBufferCount() > 1) {
        RSMainThread::Instance()->PostTask([surfaceHandler, nodeId = nodeId_]() {
            RSBaseSurfaceUtil::DropFirstFlushedBuffer(*surfaceHandler, nodeId);
        });
    }
    RSMainThread::Instance()->RequestNextVSync();
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
    auto surfaceHandler = surfaceHandler_.lock();
    if (surfaceHandler == nullptr) {
        RS_LOGE("RSCanvasDrawingNodeBufferConsumerListener::CleanBuffers: null surfaceHandler, nodeId=%{public}" PRIu64,
            nodeId_);
        return;
    }
    const auto& consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return;
    }
    RSMainThread::Instance()->PostTask([surfaceHandler, consumer, nodeId = nodeId_]() {
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
} // namespace Rosen
} // namespace OHOS
