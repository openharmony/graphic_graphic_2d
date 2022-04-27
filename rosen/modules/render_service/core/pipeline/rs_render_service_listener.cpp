/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_service_listener.h"

#include "platform/common/rs_log.h"
#include "pipeline/rs_main_thread.h"
#include "sync_fence.h"

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
        RS_LOGE("RSRenderServiceListener::OnBufferAvailable node is nullptr");
        return;
    }
    RS_LOGI("RsDebug RSRenderServiceListener::OnBufferAvailable node id:%llu", node->GetId());

    if (!node->IsOnTheTree() || !node->GetRenderProperties().GetVisible()) {
        RSMainThread::Instance()->PostTask([node]() {
            RS_LOGI("RsDebug RSRenderServiceListener::OnBufferAvailable node id:%llu:"\
                "IsOnTheTree = %s, IsVisible = %s",
                node->GetId(), node->IsOnTheTree() ? "true" : "false",
                node->GetRenderProperties().GetVisible() ? "true" : "false");
            auto& surfaceConsumer = node->GetConsumer();
            if (surfaceConsumer == nullptr) {
                RS_LOGE("RsDebug RSRenderServiceListener::OnBufferAvailable: consumer is null!");
                return;
            }
            sptr<SurfaceBuffer> buffer;
            sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
            int64_t timestamp = 0;
            Rect damage;
            auto ret = surfaceConsumer->AcquireBuffer(buffer, acquireFence, timestamp, damage);
            if (buffer == nullptr || ret != SURFACE_ERROR_OK) {
                RS_LOGE("RsDebug RSRenderServiceListener::OnBufferAvailable: AcquireBuffer failed!");
                return;
            }
            if (node->GetBuffer() != nullptr && node->GetBuffer() != buffer) {
                (void)surfaceConsumer->ReleaseBuffer(node->GetBuffer(), SyncFence::INVALID_FENCE);
            }
            node->SetBuffer(buffer);
            node->SetFence(acquireFence);
        });
    } else {
        node->IncreaseAvailableBuffer();
    }

    if (!node->IsBufferAvailable()) {
        // Only ipc for one time.
        RS_LOGI("RsDebug RSRenderServiceListener::OnBufferAvailable id = %llu "\
                "Notify buffer available", node->GetId());
        node->NotifyBufferAvailable();
    }
    RSMainThread::Instance()->RequestNextVSync();
}
} // namespace Rosen
} // namespace OHOS
