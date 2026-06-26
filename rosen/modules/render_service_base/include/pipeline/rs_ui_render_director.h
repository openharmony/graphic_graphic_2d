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

#ifndef RENDER_SERVICE_CORE_UI_RS_UI_RENDER_DIRECTOR_H
#define RENDER_SERVICE_CORE_UI_RS_UI_RENDER_DIRECTOR_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "common/rs_common_def.h"
#include "pipeline/rs_base_render_node.h"

namespace OHOS {
namespace Rosen {

/**
 * @brief Service-side counterpart of RSUIDirector for lifecycle state tracking.
 *
 * RSUIRenderDirector mirrors the lifecycle state of a client-side RSUIDirector.
 * It is created on demand when the first lifecycle state sync command is received
 * and removed when DESTROYED state is received.
 */
class RSUIRenderDirector {
public:
    explicit RSUIRenderDirector(uint64_t token);
    ~RSUIRenderDirector() = default;

    RSUIRenderDirector(const RSUIRenderDirector&) = delete;
    RSUIRenderDirector& operator=(const RSUIRenderDirector&) = delete;
    RSUIRenderDirector(RSUIRenderDirector&&) = delete;
    RSUIRenderDirector& operator=(RSUIRenderDirector&&) = delete;

    /**
     * @brief Handles lifecycle state sync from client.
     * @param state The target lifecycle state.
     */
    void OnStateSync(RSUIDirectorLifecycleState state);

    /**
     * @brief Gets the current lifecycle state.
     * @return The current state.
     */
    RSUIDirectorLifecycleState GetCurrentState() const;

    /**
     * @brief Gets the token associated with this director.
     * @return The token.
     */
    uint64_t GetToken() const;

    /**
     * @brief Registers a render node to this director.
     * @param id The node id.
     * @param node The render node.
     */
    void RegisterRenderNode(NodeId id, const std::shared_ptr<RSBaseRenderNode>& node);

    /**
     * @brief Unregisters a render node from this director.
     * @param id The node id.
     */
    void UnregisterRenderNode(NodeId id);

    /**
     * @brief Gets a registered render node by id.
     * @param id The node id.
     * @return The render node, or nullptr if not found.
     */
    std::shared_ptr<RSBaseRenderNode> GetRenderNode(NodeId id) const;

    /**
     * @brief Clears all registered render nodes.
     */
    void ClearRenderNodes();

private:
    uint64_t token_;
    std::shared_ptr<RSSurfaceRenderNode> appWindowNode_;
    RSUIDirectorLifecycleState currentState_;
    mutable std::mutex stateMutex_;
    std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>> renderNodes_;
    mutable std::mutex nodeMutex_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_UI_RS_UI_RENDER_DIRECTOR_H