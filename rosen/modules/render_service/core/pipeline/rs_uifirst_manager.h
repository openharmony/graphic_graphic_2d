/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <condition_variable>
#include <map>
#include <set>
#include <vector>

#include "rs_processor.h"

#include "drawable/rs_surface_render_node_drawable.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS::Rosen {
class RSUifirstManager {
public:
    // planning: move to display node
    static RSUifirstManager& Instance();

    typedef enum {
        STATE_NEED_SKIP,
        STATE_NOT_SKIP,
        STATE_NEED_CHECK,
    } SkipSyncState;

    struct EventInfo {
        int64_t startTime = 0;
        int64_t stopTime = 0;
        int64_t uniqueId = 0;
        int32_t appPid = -1;
        std::string sceneId;
        std::set<NodeId> disableNodes;
    };

    void AddProcessDoneNode(NodeId id);
    void AddPendingPostNode(NodeId id, std::shared_ptr<RSSurfaceRenderNode>& node,
        MultiThreadCacheType cacheType);
    void AddPendingResetNode(NodeId id, std::shared_ptr<RSSurfaceRenderNode>& node);
    void AddReuseNode(NodeId id);

    CacheProcessStatus GetNodeStatus(NodeId id);
    // judge if surfacenode satisfies async subthread rendering condtions for Uifirst
    void UpdateUifirstNodes(RSSurfaceRenderNode& node, bool ancestorNodeHasAnimation);
    void PostUifistSubTasks();
    void ProcessSubDoneNode();
    bool CollectSkipSyncNode(const std::shared_ptr<RSRenderNode> &node);
    void ForceClearSubthreadRes();
    void ProcessForceUpdateNode();
    bool ForceUpdateUifirstNodes(RSSurfaceRenderNode& node);

    // event process
    void OnProcessEventResponse(DataBaseRs& info);
    void OnProcessEventComplete(DataBaseRs& info);
    void PrepareCurrentFrameEvent();

    // animate procss
    void OnProcessAnimateScene(SystemAnimatedScenes systemAnimatedScene);

    bool NodeIsInCardWhiteList(RSRenderNode& node);
    bool GetCurrentFrameSkipFirstWait() const
    {
        return currentFrameCanSkipFirstWait_.load();
    }
    bool CheckIfAppWindowHasAnimation(RSSurfaceRenderNode& node);
    void DisableUifirstNode(RSSurfaceRenderNode& node);
    static void ProcessTreeStateChange(RSSurfaceRenderNode& node);

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

    void MergeOldDirty(NodeId id);
    void MergeOldDirtyToDrawable(std::shared_ptr<RSSurfaceRenderNode> node);

    void SetRotationChanged(bool rotationChanged)
    {
        rotationChanged_ = rotationChanged;
    }

    bool IsRecentTaskScene() const
    {
        return isRecentTaskScene_.load();
    }

    void AddCapturedNodes(NodeId id);

    void AddCardNodes(NodeId id, MultiThreadCacheType currentFrameCacheType)
    {
        if (currentFrameCacheType != MultiThreadCacheType::ARKTS_CARD) {
            return;
        }
        collectedCardNodes_.insert(id);
    }

    void RemoveCardNodes(NodeId id)
    {
        if (!collectedCardNodes_.count(id)) {
            return;
        }
        collectedCardNodes_.erase(id);
    }

    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> GetPendingPostNodes()
    {
        return pendingPostNodes_;
    }

    void PostReleaseCacheSurfaceSubTasks();
    void PostReleaseCacheSurfaceSubTask(NodeId id);
    void TryReleaseTextureForIdleThread();

    // only use in mainThread & RT onsync
    inline void UifirstCurStateClear()
    {
        uifirstCacheState_.clear();
    }

    bool IsSubTreeNeedPrepareForSnapshot(RSSurfaceRenderNode& node);

private:
    RSUifirstManager() = default;
    ~RSUifirstManager() = default;
    RSUifirstManager(const RSUifirstManager&);
    RSUifirstManager(const RSUifirstManager&&);
    RSUifirstManager& operator=(const RSUifirstManager&);
    RSUifirstManager& operator=(const RSUifirstManager&&);

    void PostSubTask(NodeId id);
    void UpdateCompletedSurface(NodeId id);

    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> GetSurfaceDrawableByID(NodeId id);
    void SetUifirstNodeEnableParam(RSSurfaceRenderNode& node, MultiThreadCacheType type);
    void RenderGroupUpdate(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> drawable);
    bool IsInLeashWindowTree(RSSurfaceRenderNode& node, NodeId instanceRootId);

    void ProcessResetNode();
    void ProcessDoneNode();
    void ProcessDoneNodeInner();
    void UpdateSkipSyncNode();
    void RestoreSkipSyncNode();
    void ClearSubthreadRes();
    void ResetUifirstNode(std::shared_ptr<RSSurfaceRenderNode>& nodePtr);
    bool CheckVisibleDirtyRegionIsEmpty(const std::shared_ptr<RSSurfaceRenderNode>& node);
    bool CurSurfaceHasVisibleDirtyRegion(const std::shared_ptr<RSSurfaceRenderNode>& node);
    void DoPurgePendingPostNodes(std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>& pendingNode);
    void PurgePendingPostNodes();
    void SetNodePriorty(std::list<NodeId>& result,
        std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>& pendingNode);
    void SortSubThreadNodesPriority();
    static bool IsArkTsCardCache(RSSurfaceRenderNode& node, bool animation);
    static bool IsLeashWindowCache(RSSurfaceRenderNode& node, bool animation);
    static bool IsNonFocusWindowCache(RSSurfaceRenderNode& node, bool animation);
    void SyncHDRDisplayParam(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> drawable);

    void UifirstStateChange(RSSurfaceRenderNode& node, MultiThreadCacheType currentFrameCacheType);
    NodeId LeashWindowContainMainWindowAndStarting(RSSurfaceRenderNode& node);
    void NotifyUIStartingWindow(NodeId id, bool wait);
    void UpdateChildrenDirtyRect(RSSurfaceRenderNode& node);
    bool EventsCanSkipFirstWait(std::vector<EventInfo>& events);
    bool IsCardSkipFirstWaitScene(std::string& scene, int32_t appPid);
    void EventDisableLeashWindowCache(NodeId id, EventInfo& info);
    void ConvertPendingNodeToDrawable();
    void CheckCurrentFrameHasCardNodeReCreate(const RSSurfaceRenderNode& node);
    void ResetCurrentFrameDeletedCardNodes();
    bool IsPreFirstLevelNodeDoingAndTryClear(std::shared_ptr<RSRenderNode> node);
    SkipSyncState CollectSkipSyncNodeWithDrawableState(const std::shared_ptr<RSRenderNode> &node);
    CacheProcessStatus& GetUifirstCachedState(NodeId id);

    // only use in mainThread & RT onsync
    std::vector<NodeId> pendingForceUpdateNode_;
    std::vector<std::shared_ptr<RSRenderNode>> markForceUpdateByUifirst_;
    bool rotationChanged_ = false;

    std::map<NodeId, CacheProcessStatus> uifirstCacheState_;

    // only use in RT
    std::unordered_map<NodeId, std::shared_ptr<DrawableV2::RSRenderNodeDrawableAdapter>> subthreadProcessingNode_;
    std::set<NodeId> processingNodeSkipSync_;
    std::set<NodeId> processingNodePartialSync_;
    std::set<NodeId> processingCardNodeSkipSync_;
    // (instanceId, vector<needsync_node>)
    std::unordered_map<NodeId, std::vector<std::shared_ptr<RSRenderNode>>> pendingSyncForSkipBefore_;

    // use in RT & subThread
    std::mutex childernDrawableMutex_;
    std::vector<NodeId> subthreadProcessDoneNode_;

    // pending post node: collect in main, use&clear in RT
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingPostNodes_;
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingPostCardNodes_;
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingResetNodes_;
    bool isUiFirstOn_ = false;
    std::list<NodeId> sortedSubThreadNodeIds_;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>  pindingResetWindowCachedNodes_;

    std::set<NodeId> reuseNodes_;
    std::set<NodeId> collectedCardNodes_;
    static constexpr int CLEAR_RES_THRESHOLD = 3; // 3 frames  to clear resource
    std::atomic<int> noUifirstNodeFrameCount_ = 0;
    bool hasDoneNode_ = false;
    // event list
    std::mutex globalFrameEventMutex_;
    std::vector<EventInfo> globalFrameEvent_; // <time, data>
    std::vector<EventInfo> currentFrameEvent_;
    std::atomic<bool> currentFrameCanSkipFirstWait_ = false;
    NodeId entryViewNodeId_ = INVALID_NODEID; // desktop surfaceNode ID
    NodeId negativeScreenNodeId_ = INVALID_NODEID; // negativeScreen surfaceNode ID
    int32_t scbPid_ = 0;
    RSMainThread* mainThread_ = nullptr;
    // scene in scb
    const std::vector<std::string> cardCanSkipFirstWaitScene_ = {
        { "INTO_HOME_ANI" }, // unlock to desktop
        { "FINGERPRINT_UNLOCK_ANI" }, // finger unlock to desktop
        { "SCREEN_OFF_FINGERPRINT_UNLOCK_ANI" }, // aod finger unlock
        { "PASSWORD_UNLOCK_ANI" }, // password unlock to desktop
        { "FACIAL_FLING_UNLOCK_ANI" }, // facial unlock to desktop
        { "FACIAL_UNLOCK_ANI" }, // facial unlock to desktop
        { "APP_SWIPER_SCROLL" }, // desktop swipe
        { "APP_SWIPER_FLING" }, // desktop swipe
        { "LAUNCHER_SCROLL" }, // desktop swipe
        { "SCROLL_2_AA" }, // desktop to negativeScreen
    };

    // for recents scene
    std::atomic<bool> isRecentTaskScene_ = false;
    std::vector<NodeId> capturedNodes_;
    std::vector<NodeId> currentFrameDeletedCardNodes_;
    std::atomic<bool> isCurrentFrameHasCardNodeReCreate_ = false;
};

// If a subnode is delivered directly
// record the firstLevelNodeId in the delivered subnode as the real one.
class RSB_EXPORT RSUiFirstProcessStateCheckerHelper {
public:
    RSUiFirstProcessStateCheckerHelper(NodeId curFirsLevelNodeId, NodeId curUifirstRootNodeId, NodeId curNodeId)
    {
        isCurUifirstRootNodeId_ = curNodeId == curUifirstRootNodeId;
        isCurFirsLevelNodeId_ = curNodeId == curFirsLevelNodeId;
        if (isCurUifirstRootNodeId_) {
            curUifirstRootNodeId_ = curUifirstRootNodeId;
        }
        if (isCurFirsLevelNodeId_) {
            curFirstLevelNodeId_ = curFirsLevelNodeId;
        }
    }

    RSUiFirstProcessStateCheckerHelper(NodeId curFirsLevelNodeId, NodeId curUifirstRootNodeId)
    {
        isCurUifirstRootNodeId_ = true;
        isCurFirsLevelNodeId_  = true;
        curUifirstRootNodeId_ = curUifirstRootNodeId;
        curFirstLevelNodeId_ = curFirsLevelNodeId;
    }

    ~RSUiFirstProcessStateCheckerHelper()
    {
        if (isCurUifirstRootNodeId_) {
            curUifirstRootNodeId_ = INVALID_NODEID;
        }
        if (isCurFirsLevelNodeId_) {
            curFirstLevelNodeId_ = INVALID_NODEID;
        }
    }
    // return false when timeout
    static void NotifyAll()
    {
        notifyCv_.notify_all();
    }
    static bool CheckMatchAndWaitNotify(const RSRenderParams& params, bool checkMatch = true);
private:
    static bool IsCurFirstLevelMatch(const RSRenderParams& params);
    static bool CheckAndWaitPreFirstLevelDrawableNotify(const RSRenderParams& params);

    static inline std::mutex notifyMutex_;
    static inline std::condition_variable notifyCv_;

    static inline thread_local NodeId curUifirstRootNodeId_ = INVALID_NODEID;
    static inline thread_local NodeId curFirstLevelNodeId_ = INVALID_NODEID;

    bool isCurUifirstRootNodeId_ = false;
    bool isCurFirsLevelNodeId_ = false;
};
}
#endif // RS_UIFIRST_MANAGER_H
