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

#include "ui/rs_canvas_callback_router.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSCanvasCallbackRouter& RSCanvasCallbackRouter::GetInstance()
{
    static RSCanvasCallbackRouter instance;
    return instance;
}

void RSCanvasCallbackRouter::RegisterNode(NodeId nodeId, std::weak_ptr<RSCanvasDrawingNode> weakNode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    nodeRoutes_[nodeId] = weakNode;
}

void RSCanvasCallbackRouter::UnregisterNode(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    nodeRoutes_.erase(nodeId);
}

std::shared_ptr<RSCanvasDrawingNode> RSCanvasCallbackRouter::RouteToNode(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = nodeRoutes_.find(nodeId);
    if (it == nodeRoutes_.end()) {
        RS_LOGE("RSCanvasCallbackRouter::RouteToNode node not found, nodeId=%{public}" PRIu64, nodeId);
        return nullptr;
    }

    auto sharedNode = it->second.lock();
    if (sharedNode == nullptr) {
        RS_LOGE("RSCanvasCallbackRouter::RouteToNode node destroyed, nodeId=%{public}" PRIu64, nodeId);
        // Clean up the expired weak_ptr
        nodeRoutes_.erase(it);
    }
    return sharedNode;
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS && RS_ENABLE_VK