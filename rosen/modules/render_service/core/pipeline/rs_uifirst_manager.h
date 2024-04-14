/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RS_UIFIRST_MANAGER_H
#define RS_UIFIRST_MANAGER_H

#include <map>
#include <vector>
#include <set>
#include "drawable/rs_surface_render_node_drawable.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS::Rosen {
#ifdef RS_PARALLEL
class RSUifirstManager {
public:
    // planning: move to display node
    static RSUifirstManager& Instance();

    void AddProcessDoneNode(NodeId id);
    void AddPendingPostNode(NodeId id, std::shared_ptr<RSSurfaceRenderNode>& node);
    void AddPendingResetNode(NodeId id, std::shared_ptr<RSSurfaceRenderNode>& node);
    void AddReuseNode(NodeId id);

    CacheProcessStatus GetNodeStatus(NodeId id);
    // judge if surfacenode satisfies async subthread rendering condtions for Uifirst
    void UpdateUifirstNodes(RSSurfaceRenderNode& node, bool ancestorNodeHasAnimation);
    void PostUifistSubTasks();
    void ProcessSubDoneNode();
    bool CollectSkipSyncNode(const std::shared_ptr<RSRenderNode> &node);
    void ForceClearSubthreadRes();

    void PrepareUifirstNode(RSSurfaceRenderNode& node, bool animation);
    void DisableUifirstNode(RSSurfaceRenderNode& node);
    static void ProcessTreeStateChange(RSSurfaceRenderNode& node);

    static bool IsUifirstNode(RSSurfaceRenderNode& node, bool animation);
    
    void SetUiFirstSwitch(bool uiFirstSwitch)
    {
        isUiFirstOn_ = uiFirstSwitch;
    }

    void SetHasDoneNodeFlag(bool flag)
    {
        hasDoneNode_ = flag;
    }

    bool HasDoneNode()
    {
        return hasDoneNode_;
    }

    void MergeOldDirty(RSSurfaceRenderNode& node);

private:
    RSUifirstManager() = default;
    ~RSUifirstManager() = default;
    RSUifirstManager(const RSUifirstManager&);
    RSUifirstManager(const RSUifirstManager&&);
    RSUifirstManager& operator=(const RSUifirstManager&);
    RSUifirstManager& operator=(const RSUifirstManager&&);

    void PostSubTask(NodeId id);
    void UpdateCompletedSurface(NodeId id);

    DrawableV2::RSSurfaceRenderNodeDrawable* GetSurfaceDrawableByID(NodeId id);

    void ProcessResetNode();
    void ProcessDoneNode();
    void UpdateSkipSyncNode();
    void RestoreSkipSyncNode();
    void ClearSubthreadRes();
    void PurgePendingPostNodes();
    void SortSubThreadNodesPriority();

    void UifirstStateChange(RSSurfaceRenderNode& node, bool currentFrameIsUifirstNode);
    void UpdateChildrenDirtyRect(RSSurfaceRenderNode& node);
    // only use in mainThread; keep ref by subthreadProcessingNode_

    // only use in RT
    std::unordered_map<NodeId, std::shared_ptr<DrawableV2::RSRenderNodeDrawableAdapter>> subthreadProcessingNode_;
    std::set<NodeId> processingNodeSkipSync_;
    std::set<NodeId> processingNodePartialSync_;
    // (instanceId, vector<needsync_node>)
    std::unordered_map<NodeId, std::vector<std::shared_ptr<RSRenderNode>>> pendingSyncForSkipBefore_;

    // use in RT & subThread
    std::mutex childernDrawableMutex_;
    std::vector<NodeId> subthreadProcessDoneNode_;

    // pending post node: collect in main, use&clear in RT
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingPostNodes_;
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingResetNodes_;
    bool isUiFirstOn_ = false;
    std::list<NodeId> sortedSubThreadNodeIds_;

    std::set<NodeId> reuseNodes_;
    static constexpr int CLEAR_RES_THRESHOLD = 3; // 3 frames  to clear resource
    int noUifirstNodeFrameCount_ = 0;
    bool hasDoneNode_ = false;
};
#endif
}
#endif // RS_UIFIRST_MANAGER_H
