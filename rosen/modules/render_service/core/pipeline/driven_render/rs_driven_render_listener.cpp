/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_driven_render_listener.h"

#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSDrivenRenderListener::~RSDrivenRenderListener() {}

RSDrivenRenderListener::RSDrivenRenderListener(std::weak_ptr<RSDrivenSurfaceRenderNode> surfaceRenderNode)
    : surfaceRenderNode_(surfaceRenderNode) {}

void RSDrivenRenderListener::OnBufferAvailable()
{
    auto node = surfaceRenderNode_.lock();
    if (node == nullptr) {
        RS_LOGE("RSDrivenRenderListener::OnBufferAvailable node is nullptr");
        return;
    }
    node->IncreaseAvailableBuffer();

    RSMainThread::Instance()->NotifyDrivenRenderFinish();
}

void RSDrivenRenderListener::OnGoBackground()
{
    std::weak_ptr<RSDrivenSurfaceRenderNode> surfaceNode = surfaceRenderNode_;
    RSMainThread::Instance()->PostTask([surfaceNode]() {
        auto node = surfaceNode.lock();
        if (node == nullptr) {
            RS_LOGW("RSDrivenRenderListener::OnGoBackground node is nullptr");
            return;
        }
        node->ResetBufferAvailableCount();
        node->CleanCache();
    });
}
} // namespace Rosen
} // namespace OHOS
