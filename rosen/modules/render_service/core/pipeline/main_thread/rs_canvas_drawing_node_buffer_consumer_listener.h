/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CORE_PIPELINE_MAIN_THREAD_RS_CANVAS_DRAWING_NODE_BUFFER_CONSUMER_LISTENER_H
#define RENDER_SERVICE_CORE_PIPELINE_MAIN_THREAD_RS_CANVAS_DRAWING_NODE_BUFFER_CONSUMER_LISTENER_H

#include <ibuffer_consumer_listener.h>

#include "pipeline/rs_canvas_drawing_render_node.h"

namespace OHOS {
namespace Rosen {
class RSCanvasDrawingNodeBufferConsumerListener : public IBufferConsumerListener {
public:
    explicit RSCanvasDrawingNodeBufferConsumerListener(std::weak_ptr<RSContext> rsContext, NodeId nodeId);
    ~RSCanvasDrawingNodeBufferConsumerListener() override;

    void OnBufferAvailable() override;
    void OnTunnelHandleChange() override;
    void OnCleanCache(uint32_t* bufSeqNum) override;
    void OnGoBackground() override;
    void OnTransformChange() override;

private:
    std::shared_ptr<RSSurfaceHandler> GetSurfaceHandler(std::shared_ptr<RSContext> rsContext);

    void CleanBuffers();

    static void CollectBuffersForClean(
        std::set<uint64_t>& bufferCacheSet, std::shared_ptr<RSSurfaceHandler> surfaceHandler);

    static void DropFirstFlushedBuffer(std::weak_ptr<RSSurfaceHandler> weakHandler, NodeId nodeId);

    std::weak_ptr<RSContext> rsContext_ = {};
    std::weak_ptr<RSCanvasDrawingRenderNode> node_ = {};
    NodeId nodeId_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_MAIN_THREAD_RS_CANVAS_DRAWING_NODE_BUFFER_CONSUMER_LISTENER_H