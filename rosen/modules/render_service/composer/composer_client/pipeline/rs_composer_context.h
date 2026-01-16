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

#ifndef RENDER_SERVICE_COMPOSER_CLIENT_PIPELINE_RS_LAYER_CONTEXT_H
#define RENDER_SERVICE_COMPOSER_CLIENT_PIPELINE_RS_LAYER_CONTEXT_H

#include <functional>
#include <memory>
#include <mutex>
#include "irs_render_to_composer_connection.h"
#include "rs_layer.h"
#include "rs_layer_transaction_handler.h"

namespace OHOS {
namespace Rosen {

using OnReleaseLayerBuffersCB = std::function<void(std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>>& rsLayers,
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>>& releaseBufferFenceVec,
    uint64_t screenId)>;

class RSC_EXPORT RSComposerContext : public std::enable_shared_from_this<RSComposerContext> {
public:
    RSComposerContext(const sptr<IRSRenderToComposerConnection>& conn);
    RSComposerContext(const RSComposerContext&) = delete;
    RSComposerContext(const RSComposerContext&&) = delete;
    RSComposerContext& operator=(const RSComposerContext&) = delete;
    RSComposerContext& operator=(const RSComposerContext&&) = delete;
    RSComposerContext() = default;
    virtual ~RSComposerContext() = default;

protected:
    std::shared_ptr<RSLayerTransactionHandler> GetRSLayerTransaction() const;

    void AddRSLayer(const std::shared_ptr<RSLayer>& rsLayer);
    void RemoveRSLayer(RSLayerId layerId);
    std::shared_ptr<RSLayer> GetRSLayer(RSLayerId rsLayerId) const;

    bool CommitLayers(ComposerInfo& composerInfo);
    void ReleaseLayerBuffers(uint64_t screenId,
        std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>>& timestampVec,
        std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>>& releaseBufferFenceVec);
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId);
    void ClearFrameBuffers();
    void DumpLayersInfo(std::string &dumpString);
    void DumpCurrentFrameLayers();
    void RegisterOnReleaseLayerBuffersCB(OnReleaseLayerBuffersCB onReleaseLayerBuffersCB)
    {
        if (onReleaseLayerBuffersCB_ == nullptr) {
            onReleaseLayerBuffersCB_ = std::move(onReleaseLayerBuffersCB);
        }
    }
    void PreAllocProtectedFrameBuffers(const sptr<SurfaceBuffer>& buffer);

private:
    void ANCOTransactionOnComplete(const std::shared_ptr<RSLayer>& rsLayer, const sptr<SyncFence>& previousReleaseFence);
    mutable std::recursive_mutex rsLayerTransMutex_;
    std::shared_ptr<RSLayerTransactionHandler> rsLayerTransactionHandler_;
    mutable std::mutex rsLayerMutex_;
    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers_;
    sptr<IRSRenderToComposerConnection> rsComposerConnection_;
    std::vector<RSLayerId> lastCommitLayersId_;

    OnReleaseLayerBuffersCB onReleaseLayerBuffersCB_ = nullptr;
    friend class RSComposerClient;
    friend class RSSurfaceLayer;
    friend class RSSurfaceRCDLayer;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_CLIENT_PIPELINE_RS_LAYER_CONTEXT_H