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

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_CALLBACK_ROUTER_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_CALLBACK_ROUTER_H

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include <mutex>
#include <unordered_map>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSCanvasDrawingNode;

// RSCanvasCallbackRouter manages the routing of Canvas SurfaceBuffer callbacks
// from the process-wide global callback to specific RSCanvasDrawingNode instances.
//
// This router solves the dual NodeMap problem: nodes may register in either
// RSUIContext->GetMutableNodeMap() or RSNodeMap::MutableInstance(), so the
// global callback cannot directly query NodeMap. Instead, nodes register
// themselves here with weak_ptr for lifecycle-safe routing.
class RSCanvasCallbackRouter {
public:
    static RSCanvasCallbackRouter& GetInstance();

    // Register a node for callback routing
    // @param nodeId: the node ID to register
    // @param weakNode: weak pointer to the node (avoids extending lifetime)
    void RegisterNode(NodeId nodeId, std::weak_ptr<RSCanvasDrawingNode> weakNode);

    // Unregister a node (called in node destructor)
    // @param nodeId: the node ID to unregister
    void UnregisterNode(NodeId nodeId);

    // Route callback to the specific node
    // @param nodeId: the target node ID
    // @return shared_ptr to the node if still alive, nullptr otherwise
    std::shared_ptr<RSCanvasDrawingNode> RouteToNode(NodeId nodeId);

private:
    RSCanvasCallbackRouter() = default;
    ~RSCanvasCallbackRouter() = default;
    RSCanvasCallbackRouter(const RSCanvasCallbackRouter&) = delete;
    RSCanvasCallbackRouter& operator=(const RSCanvasCallbackRouter&) = delete;

    std::unordered_map<NodeId, std::weak_ptr<RSCanvasDrawingNode>> nodeRoutes_;
    std::mutex mutex_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS && RS_ENABLE_VK
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_CALLBACK_ROUTER_H