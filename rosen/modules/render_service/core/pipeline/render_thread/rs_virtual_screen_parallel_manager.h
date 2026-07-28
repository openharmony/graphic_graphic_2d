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

#ifndef RENDER_SERVICE_PIPELINE_RENDER_THREAD_RS_VIRTUAL_SCREEN_PARALLEL_MANAGER_H
#define RENDER_SERVICE_PIPELINE_RENDER_THREAD_RS_VIRTUAL_SCREEN_PARALLEL_MANAGER_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include "common/rs_common_def.h"
#include "engine/rs_uni_render_engine.h"
#include "drawable/rs_render_node_drawable.h"
#include "ffrt.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_virtual_screen_thread_id_adapt.h"

namespace OHOS {
namespace Rosen {
class RSVirtualScreenParallelManager {
public:
    RSVirtualScreenParallelManager() = default;
    ~RSVirtualScreenParallelManager() = default;

    void CollectVirtualScreenNodeId(ScreenId screenId, NodeId nodeId, CompositeType compositeType);
    void GetStagingNodeIds(std::unordered_set<NodeId>& nodeIds);
    bool ShouldSkipRenderNodeOnDraw(NodeId nodeId);
    void ExecuteAllVirtualScreenRenderTasks(std::unique_ptr<RSRenderThreadParams> stagingRenderThreadParams);
    void WaitForAllVirtualScreenRenderTasksComplete();
    void CleanupThreadResources(ScreenId screenId);
    bool GetRenderEngineByTid(int32_t tid, std::shared_ptr<RSBaseRenderEngine>& renderEngine);

private:
    struct ScreenDrawableInfo {
        std::shared_ptr<DrawableV2::RSScreenRenderNodeDrawable> drawable;
        RSScreenRenderParams* params = nullptr;
        ScreenId screenId = INVALID_SCREEN_ID;
        bool IsValid() const { return drawable && params; }
    };

    ScreenDrawableInfo GetScreenDrawableInfo(NodeId nodeId);
    void InitializeThread(ScreenId screenId, std::shared_ptr<ffrt::queue>& ffrtThread);
    void IncrementPendingTaskCount(size_t nodeCount);
    void DecrementPendingTaskCount(ScreenId screenId);
    std::shared_ptr<ffrt::queue> AssignThreadIndex(ScreenId screenId);
    std::shared_ptr<RSUniRenderEngine> GetVirtualScreenRenderEngine(ScreenId screenId);
    void SyncCollectedInfo(const std::unordered_set<NodeId>& nodeIds);
    void FilterExcessScreenNodes();
    void VirtualScreenRenderTask(std::shared_ptr<RSRenderThreadParams> renderThreadParams,
        ScreenDrawableInfo info, int32_t tid);

    ffrt::mutex taskMutex_;
    std::unordered_set<NodeId> virtualScreenNodeIds_;
    std::unordered_set<NodeId> stagingNodeIds_;
    std::unordered_set<ScreenId> excludedScreenIds_;
    ffrt::condition_variable taskCondition_;
    ffrt::condition_variable screenTaskCondition_;
    size_t pendingTaskCount_ = 0;
    std::unordered_map<ScreenId, std::shared_ptr<ffrt::queue>> ffrtThreadIndexMap_;
    std::unordered_map<ScreenId, size_t> screenTaskCountMap_;
    std::unordered_map<ScreenId, std::shared_ptr<RSUniRenderEngine>> uniRenderEngineMap_;
    std::unordered_map<ScreenId, int32_t> screenIdToTidMap_;
    std::unordered_map<ScreenId, std::shared_ptr<RSUniRenderEngine>> tidToUniRenderEngineMap_;
    std::unordered_set<int32_t> usedTidSet_;
    int32_t virtualScreenCnt_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_PIPELINE_RENDER_THREAD_RS_VIRTUAL_SCREEN_PARALLEL_MANAGER_H