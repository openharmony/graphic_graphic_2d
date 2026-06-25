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
#ifndef RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_LISTENER_H
#define RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_LISTENER_H

#include <ibuffer_consumer_listener.h>
#include <mutex>
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"

#include "render_server/rs_render_service.h"

namespace OHOS {
namespace Rosen {
class RSRenderServiceListener : public IBufferConsumerListener {
public:
    RSRenderServiceListener(std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode,
        std::weak_ptr<RSSurfaceHandler> surfaceHandler,
        std::shared_ptr<RSComposerClientManager> composerClientManager);
    ~RSRenderServiceListener() override;
    void OnBufferAvailable() override;
    void OnTunnelHandleChange() override;
    void OnCleanCache(uint32_t *bufSeqNum) override;
    void OnGoBackground() override;
    void OnTransformChange() override;
    void OnDropBuffer() override;
    void SetRSSurfaceBufferInterface(std::weak_ptr<RSSurfaceBufferInterface> surfaceBufferInterface);
    void OnCleanCacheForBufferInfoMap(std::vector<CleanCacheBufferInfo> &infos) override;
    bool IsNeedBufferInfo() override;

private:
    void SetBufferInfoAndRequest(const std::shared_ptr<RSSurfaceHandler> &surfaceHandler,
        const sptr<IConsumerSurface> &consumer, bool doFastCompose = false);
    void NotifyBufferAvailableOnce(const std::shared_ptr<RSSurfaceRenderNode>& node);
    bool CheckFastCompose(const sptr<IConsumerSurface>& consumer);
    std::weak_ptr<RSSurfaceBufferInterface> surfaceBufferInterface_;
    std::weak_ptr<RSSurfaceHandler> surfaceHandler_;
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode_;
    void CleanLayerBufferCache();
    bool ForceRefresh(std::shared_ptr<RSSurfaceRenderNode> &node);
    std::shared_ptr<RSComposerClientManager> composerClientManager_;
    uint64_t nodeId_ = 0;

    void ConsumeBufferToKeepQueueRunning(std::weak_ptr<RSSurfaceHandler> surfaceHandler);
    void ProcessPendingCallbacks();
    std::string name_;
    std::mutex pendingStateMutex_;
    uint8_t pendingCallbackBits_ = 0;
    uint32_t cleanCacheBufSeqNum_ = 0;
    bool isInterfaceDirty_ = false;
    enum PendingStateBit : uint8_t {
        PENDING_ON_BUFFER_AVAILABLE_BIT = 0x01,
        PENDING_ON_TUNNEL_HANDLE_CHANGE_BIT = 0x02,
        PENDING_ON_CLEAN_CACHE_BIT = 0x04,
        PENDING_ON_GO_BACKGROUND_BIT = 0x08,
        PENDING_ON_TRANSFORM_CHANGE_BIT = 0x10,
    };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_LISTENER_H
