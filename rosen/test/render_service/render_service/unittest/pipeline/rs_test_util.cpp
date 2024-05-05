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
#include "rs_test_util.h"

namespace OHOS::Rosen {
NodeId RSTestUtil::id = 0;
std::shared_ptr<RSSurfaceRenderNode> RSTestUtil::CreateSurfaceNode()
{
    id++;
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.name += std::to_string(id);
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    if (RSUniRenderJudgement::IsUniRender()) {
        rsSurfaceRenderNode->InitRenderParams();
    }
    csurf = IConsumerSurface::Create(config.name);
    rsSurfaceRenderNode->SetConsumer(csurf);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(rsSurfaceRenderNode);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(surfaceRenderNode);
    csurf->RegisterConsumerListener(listener);
    return rsSurfaceRenderNode;
}

std::shared_ptr<RSSurfaceRenderNode> RSTestUtil::CreateSurfaceNodeWithBuffer()
{
    auto rsSurfaceRenderNode = CreateSurfaceNode();
    const auto& surfaceConsumer = rsSurfaceRenderNode->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    psurf = Surface::CreateSurfaceAsProducer(producer);
    psurf->SetQueueSize(1);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    [[maybe_unused]] GSError ret = psurf->RequestBuffer(buffer, requestFence, requestConfig);
    sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
    ret = psurf->FlushBuffer(buffer, flushFence, flushConfig);
    OHOS::sptr<SurfaceBuffer> cbuffer;
    Rect damage;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    ret = surfaceConsumer->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);
    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(*(rsSurfaceRenderNode.get()));
    surfaceHandler.SetBuffer(cbuffer, acquireFence, damage, timestamp);
    auto drGPUContext = std::make_shared<Drawing::GPUContext>();
    rsSurfaceRenderNode->SetDrawingGPUContext(drGPUContext.get());
    return rsSurfaceRenderNode;
}
}
