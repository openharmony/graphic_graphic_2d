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

#ifndef RS_TUNNEL_LAYER_MANAGER_H
#define RS_TUNNEL_LAYER_MANAGER_H

#include <memory>
#include <sys/types.h>
#include <unordered_map>

#include <iconsumer_surface.h>
#include <surface_buffer.h>

#include "common/rs_common_def.h"
#include "pipeline/rs_surface_handler.h"
#include "feature/tunnel_layer/rs_tunnel_layer_state_handler.h"
#include "feature/tunnel_layer/rs_tunnel_runtime_state.h"
#include "rs_composer_client_manager.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {
class RSContext;
class RSSurfaceRenderNode;

class RSTunnelLayerManager : public RSTunnelLayerStateHandler {
public:
    explicit RSTunnelLayerManager(std::shared_ptr<RSContext> context);
    ~RSTunnelLayerManager() override = default;

    void TransferTunnelPendingBufferToNormalConsume(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const;
    void MarkTunnelBufferConsumedForNormal(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        const std::shared_ptr<RSComposerClientManager>& clientManager) const;
    void ClearRuntimeStateByPid(pid_t remotePid);
    void UpdateTunnelLayerState(NodeId nodeId, const std::shared_ptr<RSSurfaceHandler>& surfaceHandler);
    void HandleLayerStateChanged(NodeId nodeId, LayerStateChange state, uint64_t tunnelLayerGeneration) override;

private:
    static const char* ToLayerStateChangeName(LayerStateChange state);

    std::shared_ptr<RSContext> GetContext() const;
    std::shared_ptr<RSSurfaceRenderNode> GetSurfaceNode(NodeId nodeId) const;
    bool HandleLppTunnelLayerId(const std::shared_ptr<RSSurfaceHandler>& surfaceHandler, NodeId nodeId) const;
    void ResetTunnelLayerState(const std::shared_ptr<RSSurfaceHandler>& surfaceHandler,
        const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode);
    void HandleNewTunnelLayerId(const std::shared_ptr<RSSurfaceHandler>& surfaceHandler, NodeId nodeId);
    void ProcessLayerStateChanged(const std::shared_ptr<RSSurfaceHandler>& surfaceHandler, NodeId nodeId,
        LayerStateChange state, uint64_t tunnelLayerGeneration, const sptr<IConsumerSurface>& consumer);
    void UpdateLayerStateLog(NodeId nodeId, LayerStateChange callbackState, LayerStateChange inputState,
        bool isTunnelStateTracked);
    void DispatchLayerStateChanged(
        NodeId nodeId, LayerStateChange state, const sptr<IConsumerSurface>& callbackConsumer) const;

    std::weak_ptr<RSContext> context_;
    std::unordered_map<NodeId, LayerStateChange> lastNotifiedLayerStates_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_TUNNEL_LAYER_MANAGER_H
