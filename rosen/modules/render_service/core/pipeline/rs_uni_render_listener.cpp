/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_uni_render_listener.h"

#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSUniRenderListener::~RSUniRenderListener() {}

RSUniRenderListener::RSUniRenderListener(std::weak_ptr<RSDisplayRenderNode> displayRenderNode)
    : displayRenderNode_(displayRenderNode) {}

void RSUniRenderListener::OnBufferAvailable()
{
    auto node = displayRenderNode_.lock();
    if (node == nullptr) {
        RS_LOGE("RSUniRenderListener::OnBufferAvailable node is nullptr");
        return;
    }
    RS_LOGI("RSUniRenderListener::OnBufferAvailable node id:%llu", node->GetId());

    if (!node->IsOnTheTree()) {
        RSMainThread::Instance()->PostTask([node]() {
            RS_LOGI("RSUniRenderListener::OnBufferAvailable node id:%llu: is not on the tree",
                node->GetId());
            auto& surfaceConsumer = node->GetConsumer();
            if (surfaceConsumer == nullptr) {
                RS_LOGE("RSUniRenderListener::OnBufferAvailable: consumer is null!");
                return;
            }
            sptr<SurfaceBuffer> buffer;
            int32_t fence = -1;
            int64_t timestamp = 0;
            Rect damage;
            auto ret = surfaceConsumer->AcquireBuffer(buffer, fence, timestamp, damage);
            if (buffer == nullptr || ret != SURFACE_ERROR_OK) {
                RS_LOGE("RSUniRenderListener::OnBufferAvailable: AcquireBuffer failed!");
                return;
            }

            if (node->GetBuffer() != nullptr && node->GetBuffer() != buffer) {
                (void)surfaceConsumer->ReleaseBuffer(node->GetBuffer(), -1);
            }
            node->SetBuffer(buffer);
            sptr<SyncFence> acquireFence = new SyncFence(fence);
            node->SetFence(acquireFence);
        });
    } else {
        node->IncreaseAvailableBuffer();
        std::shared_ptr<RSProcessor> processor;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            processor = processor_;
        }
        if (processor != nullptr) {
            processor_->ProcessSurface(*node);
            processor_->PostProcess();
        }
    }
}

void RSUniRenderListener::UpdateProcessor(std::shared_ptr<RSProcessor> processor)
{
    std::lock_guard<std::mutex> lock(mutex_);
    processor_ = processor;
}
}
}

