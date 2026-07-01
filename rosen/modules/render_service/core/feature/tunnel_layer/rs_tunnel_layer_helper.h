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

#ifndef RS_TUNNEL_LAYER_HELPER_H
#define RS_TUNNEL_LAYER_HELPER_H

#include <memory>

#include <iconsumer_surface.h>

#include "common/rs_common_def.h"
#include "surface_buffer.h"
#include "sync_fence.h"
#include "rs_surface_layer.h"

namespace OHOS {
namespace Rosen {
class RSComposerClientManager;
class RSSurfaceHandler;
class RSSurfaceRenderNode;

class RSTunnelLayerHelper {
public:
    struct ListenerHandleResult {
        bool committed = false;
        bool needRequestVsync = false;
    };

    static bool ResolveTunnelLayerInfo(
        const sptr<IConsumerSurface>& consumer, uint64_t& tunnelLayerId, uint32_t& property,
        NodeId nodeId = 0);
    static bool AcquirePendingBuffer(const std::shared_ptr<RSSurfaceRenderNode>& node);
    static bool TryCommitBufferDirect(const std::shared_ptr<RSSurfaceRenderNode>& node,
        const std::shared_ptr<RSComposerClientManager>& composerClientManager, bool consumePendingBuffer,
        bool previousFrameWasRs = false);
    static void BeginTunnelBuilding(NodeId nodeId, uint64_t tunnelLayerId, uint32_t property);
    static void ResetTunnelState(const std::shared_ptr<RSSurfaceRenderNode>& node);
    static ListenerHandleResult HandleListenerBuffer(
        const std::shared_ptr<RSSurfaceRenderNode>& node,
        const std::shared_ptr<RSSurfaceHandler>& surfaceHandler,
        const std::shared_ptr<RSComposerClientManager>& composerClientManager);

private:
    static bool TryCommitPendingBuffer(const std::shared_ptr<RSSurfaceRenderNode>& node,
        const std::shared_ptr<RSComposerClientManager>& composerClientManager, bool fallbackOnFailure,
        bool previousFrameWasRs = false);
    static RSLayerPtr CreateTunnelLayer(const std::shared_ptr<RSSurfaceRenderNode>& node,
        const std::shared_ptr<RSComposerClientManager>& composerClientManager,
        const RSSurfaceHandler::SurfaceBufferEntry& bufferEntry);
    static void ReleaseTunnelLayer(const RSLayerPtr& layer, uint64_t bufferId);
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_TUNNEL_LAYER_HELPER_H
