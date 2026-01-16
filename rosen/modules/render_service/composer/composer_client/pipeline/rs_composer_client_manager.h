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

#ifndef RENDER_SERVICE_COMPOSER_CLIENT_MANAGER_PIPELINE_RS_RENDER_COMPOSER_CLIENT_H
#define RENDER_SERVICE_COMPOSER_CLIENT_MANAGER_PIPELINE_RS_RENDER_COMPOSER_CLIENT_H

#include <unordered_map>
#include "rs_composer_client.h"

namespace OHOS {
namespace Rosen {
class RSComposerClientManager {
public:
    RSComposerClientManager() = default;
    virtual ~RSComposerClientManager() = default;
    void AddComposerClient(ScreenId screenId, const std::shared_ptr<RSComposerClient>& rsComposerClient);
    void DeleteComposerClient(ScreenId screenId);
    std::shared_ptr<RSComposerClient> GetComposerClient(ScreenId screenId);
    void ClearRedrawGPUCompositionCache(const std::unordered_set<uint64_t>& bufferIds);
    void RenderFrameStart(uint64_t timestamp);
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId, NodeId nodeId);
    void SetScreenBacklight(ScreenId screenId, uint32_t level);
    PipelineParam GetPipelineParam(ScreenId screenId);
    void UpdatePipelineParam(ScreenId screenId, const PipelineParam& pipelineParam);
    void PreAllocProtectedFrameBuffers(ScreenId screenId, const sptr<SurfaceBuffer>& buffer);
    void RegisterOnReleaseLayerBuffersCB(ScreenId screenId, OnReleaseLayerBuffersCB cb);
    void ReleaseLayerBuffers(uint64_t screenId,
        std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>>& timestampVec,
        std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>>& releaseBufferFenceVec);
    int32_t GetMinAccumulatedBufferCount();
    void DumpSurfaceInfo(std::string& dumpString);
    void DumpCurrentFrameLayers();
    void ClearFrameBuffers(const std::unordered_set<ScreenId>& screenHasProtectedLayerSet);

private:
    mutable std::mutex rsComposerMapMutex_;
    std::unordered_map<ScreenId, std::shared_ptr<RSComposerClient>> composerClientMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_CLIENT_MANAGER_PIPELINE_RS_RENDER_COMPOSER_CLIENT_H