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

#include <chrono>
#include <condition_variable>
#include <map>
#include <set>
#include <vector>

#include "drawable/rs_surface_render_node_drawable.h"
#include "pipeline/rs_processor.h"
#include "pipeline/main_thread/rs_main_thread.h"
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
    void AddProcessSkippedNode(NodeId id);
    void AddPendingPostNode(NodeId id, std::shared_ptr<RSSurfaceRenderNode>& node,
        MultiThreadCacheType cacheType);
    void AddPendingResetNode(NodeId id, std::shared_ptr<RSSurfaceRenderNode>& node);
    void AddPendingNodeBehindWindow(NodeId id, std::shared_ptr<RSSurfaceRenderNode>& node,
        MultiThreadCacheType currentFrameCacheType);

    bool NeedNextDrawForSkippedNode();

    CacheProcessStatus GetNodeStatus(NodeId id);
    // judge if surfacenode satisfies async subthread rendering condtions for Uifirst
    void UpdateUifirstNodes(RSSurfaceRenderNode& node, bool ancestorNodeHasAnimation);
    void UpdateUIFirstNodeUseDma(RSSurfaceRenderNode& node, const std::vector<RectI>& rects);
    void PostUifistSubTasks();
    void ProcessSubDoneNode();
    // check whether the node should skip onsync process,
    // if node is drawing in subthread, onsync may cause wrong result.
    bool CollectSkipSyncNode(const std::shared_ptr<RSRenderNode> &node);
    void ForceClearSubthreadRes();
    void ProcessForceUpdateNode();
    // check if uifirst is force-enabled or force-disabled
    bool ForceUpdateUifirstNodes(RSSurfaceRenderNode& node);

    // process animation events, such as LAUNCHER_SCROLL
    void OnProcessEventResponse(DataBaseRs& info);
    void OnProcessEventComplete(DataBaseRs& info);
    void PrepareCurrentFrameEvent();

    // animate procss
    void OnProcessAnimateScene(SystemAnimatedScenes systemAnimatedScene);

    // check if node is child of main screen or negative screen
    bool NodeIsInCardWhiteList(RSRenderNode& node);
    bool GetCurrentFrameSkipFirstWait() const
    {
        return currentFrameCanSkipFirstWait_.load();
    }
    bool CheckIfAppWindowHasAnimation(RSSurfaceRenderNode& node);
    void DisableUifirstNode(RSSurfaceRenderNode& node);
    static void ProcessTreeStateChange(RSSurfaceRenderNode& node);

    bool GetUiFirstSwitch() const
    {
        return isUiFirstOn_ && isUiFirstSupportFlag_;
    }

    void SetCardUiFirstSwitch(bool cardUiFirstSwitch)
    {
        isCardUiFirstOn_ = cardUiFirstSwitch;
    }

    UiFirstCcmType GetUiFirstType() const
    {
        return uifirstType_;
    }

    void SetUiFirstType(int type);

    void SetPurgeEnable(bool purgeEnable)
    {
        purgeEnable_ = purgeEnable;
    }

    void SetNodeNeedForceUpdateFlag(bool flag)
    {
        hasForceUpdateNode_ = flag;
    }

    bool HasForceUpdateNode()
    {
        return hasForceUpdateNode_;
    }

    void MergeOldDirty(NodeId id);
    void MergeOldDirtyToDirtyManager(std::shared_ptr<RSSurfaceRenderNode>& node);

    void PreStatusProcess(bool rotationChanged)
    {
        rotationChanged_ = rotationChanged;
        curUifirstWindowNums_ = 0;
    }

    bool IsRecentTaskScene() const
    {
        return isRecentTaskScene_.load();
    }

    bool IsMissionCenterScene() const
    {
        return isMissionCenterScene_.load();
    }

    bool IsSplitScreenScene() const
    {
        return isSplitScreenScene_.load();
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

    void SetFreeMultiWindowStatus(bool enable)
    {
        isFreeMultiWindowEnabled_ = enable;
    }
    UiFirstModeType GetUiFirstMode();
    void ReadUIFirstCcmParam();
    void RefreshUIFirstParam();
    // only use in mainThread & RT onsync
    inline void UifirstCurStateClear()
    {
        uifirstCacheState_.clear();
    }

    bool IsSubTreeNeedPrepareForSnapshot(RSSurfaceRenderNode& node);
    bool IsSubHighPriorityType(RSSurfaceRenderNode& node) const;
    void CheckHwcChildrenType(RSSurfaceRenderNode& node, SurfaceHwcNodeType& enabledType);
    void MarkSubHighPriorityType(RSSurfaceRenderNode& node);
    void MarkPostNodesPriority();
    bool SubThreadControlFrameRate(NodeId id,
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& drawable,
    std::shared_ptr<RSSurfaceRenderNode>& node);
    void RecordScreenRect(RSSurfaceRenderNode& node, RectI rect);
    void RecordDirtyRegionMatrix(RSSurfaceRenderNode& node, const Drawing::Matrix& matrix);
    // get cache state of uifirst root node
    CacheProcessStatus GetCacheSurfaceProcessedStatus(const RSSurfaceRenderParams& surfaceParams);
    void AddMarkedClearCacheNode(NodeId id);
    void ProcessMarkedNodeSubThreadCache();
private:
    struct NodeDataBehindWindow {
        uint64_t curTime = 0;
        bool isFirst = true;
    };
    RSUifirstManager() = default;
    ~RSUifirstManager() = default;
    RSUifirstManager(const RSUifirstManager&);
    RSUifirstManager(const RSUifirstManager&&);
    RSUifirstManager& operator=(const RSUifirstManager&);
    RSUifirstManager& operator=(const RSUifirstManager&&);

    void PostSubTask(NodeId id);
    void UpdateCompletedSurface(NodeId id);

    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> GetSurfaceDrawableByID(NodeId id);
    bool SetUifirstNodeEnableParam(RSSurfaceRenderNode& node, MultiThreadCacheType type);
    void RenderGroupUpdate(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> drawable);
    bool IsInLeashWindowTree(RSSurfaceRenderNode& node, NodeId instanceRootId);

    void ProcessResetNode();
    void ProcessSkippedNode();
    void ProcessDoneNode();
    void ProcessDoneNodeInner();
    void UpdateSkipSyncNode();
    void RestoreSkipSyncNode();
    void ClearSubthreadRes();
    // reset node status and clear cache when exit uifirst
    void ResetUifirstNode(std::shared_ptr<RSSurfaceRenderNode>& nodePtr);
    // clear main thread cache
    void ResetWindowCache(std::shared_ptr<RSSurfaceRenderNode>& nodePtr);
    bool CheckVisibleDirtyRegionIsEmpty(const std::shared_ptr<RSSurfaceRenderNode>& node);
    bool CurSurfaceHasVisibleDirtyRegion(const std::shared_ptr<RSSurfaceRenderNode>& node);
    bool IsBehindWindowOcclusion(const std::shared_ptr<RSSurfaceRenderNode>& node);
    bool NeedPurgeByBehindWindow(NodeId id, bool hasTexture,
    const std::shared_ptr<RSSurfaceRenderNode>& node);
    // filter out the nodes by behind window
    void HandlePurgeBehindWindow(std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>::iterator& it,
        std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>& pendingNode);
    // filter out the nodes which need to draw in subthread
    void DoPurgePendingPostNodes(std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>& pendingNode);
    // use in behindwindow condition to calculate the node purge time interval
    uint64_t GetTimeDiffBehindWindow(uint64_t currentTime, NodeId id);
    uint64_t GetMainThreadVsyncTime();
    void PurgePendingPostNodes();
    void SetNodePriorty(std::list<NodeId>& result,
        std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>& pendingNode);
    void SortSubThreadNodesPriority();
    // check if ArkTsCard enable uifirst
    static bool IsArkTsCardCache(RSSurfaceRenderNode& node, bool animation);
    // check if leash window enable uifirst
    static bool IsLeashWindowCache(RSSurfaceRenderNode& node, bool animation);
    void SyncHDRDisplayParam(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> drawable,
        const GraphicColorGamut& colorGamut);
    // check if non-focus window enable uifirst
    static bool IsNonFocusWindowCache(RSSurfaceRenderNode& node, bool animation);

    // update node's uifirst state
    void UifirstStateChange(RSSurfaceRenderNode& node, MultiThreadCacheType currentFrameCacheType);
    NodeId LeashWindowContainMainWindowAndStarting(RSSurfaceRenderNode& node);
    void NotifyUIStartingWindow(NodeId id, bool wait);
    bool HasStartingWindow(RSSurfaceRenderNode& node);

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
    bool IsVMSurfaceName(std::string surfaceName);

    bool IsToSubByAppAnimation() const;
    bool QuerySubAssignable(RSSurfaceRenderNode& node, bool isRotation);
    bool GetSubNodeIsTransparent(RSSurfaceRenderNode& node, std::string& dfxMsg);
    bool CheckHasTransAndFilter(RSSurfaceRenderNode& node);
    bool HasBgNodeBelowRootNode(RSSurfaceRenderNode& appNode) const;

    // starting
    void ProcessFirstFrameCache(RSSurfaceRenderNode& node, MultiThreadCacheType cacheType);

    bool IsFocusedNode(const RSSurfaceRenderNode& node) const;
    void IncreaseUifirstWindowCount(const RSSurfaceRenderNode& node);
    void DecreaseUifirstWindowCount(const RSSurfaceRenderNode& node);
    bool IsExceededWindowsThreshold(const RSSurfaceRenderNode& node) const;

    bool rotationChanged_ = false;
    bool isUiFirstOn_ = false;
    bool isUiFirstSupportFlag_ = false;
    bool purgeEnable_ = false;
    bool isCardUiFirstOn_ = false;
    UiFirstCcmType uifirstType_ = UiFirstCcmType::SINGLE;
    bool hasForceUpdateNode_ = false;
    bool isFreeMultiWindowEnabled_ = false;
    std::atomic<bool> currentFrameCanSkipFirstWait_ = false;
    // for recents scene
    std::atomic<bool> isRecentTaskScene_ = false;
    std::atomic<bool> isMissionCenterScene_ = false;
    std::atomic<bool> isSplitScreenScene_ = false;
    std::atomic<bool> isCurrentFrameHasCardNodeReCreate_ = false;
    static constexpr int CLEAR_RES_THRESHOLD = 3; // 3 frames  to clear resource
    static constexpr int BEHIND_WINDOW_TIME_THRESHOLD = 3;
    // Minimum frame drop time in behind window condition
    static constexpr int BEHIND_WINDOW_RELEASE_TIME = 33;
    // the max Delivery time in behind window condition
    static constexpr int PURGE_BEHIND_WINDOW_TIME = BEHIND_WINDOW_RELEASE_TIME - BEHIND_WINDOW_TIME_THRESHOLD;
    int32_t scbPid_ = 0;
    std::atomic<int> noUifirstNodeFrameCount_ = 0;
    NodeId entryViewNodeId_ = INVALID_NODEID; // desktop surfaceNode ID
    NodeId negativeScreenNodeId_ = INVALID_NODEID; // negativeScreen surfaceNode ID
    RSMainThread* mainThread_ = nullptr;
    // only use in mainThread & RT onsync
    std::vector<NodeId> pendingForceUpdateNode_;
    std::vector<std::shared_ptr<RSRenderNode>> markForceUpdateByUifirst_;

    std::map<NodeId, CacheProcessStatus> uifirstCacheState_;

    // priority
    bool isFocusNodeFound_ = false;
    pid_t focusNodeThreadIndex_ = UINT32_MAX;

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
    std::mutex skippedNodeMutex_;
    std::unordered_set<NodeId> subthreadProcessSkippedNode_;

    // pending post node: collect in main, use&clear in RT
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingPostNodes_;
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingPostCardNodes_;
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingResetNodes_;
    // record the release time of each pendingnode to control the release frequency
    std::unordered_map<NodeId, NodeDataBehindWindow> pendingNodeBehindWindow_;
    std::list<NodeId> sortedSubThreadNodeIds_;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> pendingResetWindowCachedNodes_;

    std::set<NodeId> collectedCardNodes_;
    // event list
    std::mutex globalFrameEventMutex_;
    std::vector<EventInfo> globalFrameEvent_; // <time, data>
    std::vector<EventInfo> currentFrameEvent_;
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
    const std::vector<std::string> toSubByAppAnimation_ = {
        { "WINDOW_TITLE_BAR_MINIMIZED" },
        { "LAUNCHER_APP_LAUNCH_FROM_DOCK" },
    };

    const std::vector<std::string> vmAppNameSet_ = {
        { "ohvm" },
        { "win_emulator" },
        { "hongyunvd" },
        { "ecoengine" },
    };
    std::vector<NodeId> capturedNodes_;
    std::vector<NodeId> currentFrameDeletedCardNodes_;

    // maximum uifirst window count
    int uifirstWindowsNumThreshold_ = 0;
    // current uifirst window count
    int curUifirstWindowNums_ = 0;
    // uifirst mem opt.
    std::set<NodeId> markedClearCacheNodes_;
};
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

    // If a subnode is delivered directly
    // record the firstLevelNodeId in the delivered subnode as the real one.
    RSUiFirstProcessStateCheckerHelper(NodeId curFirsLevelNodeId, NodeId curUifirstRootNodeId)
    {
        isCurUifirstRootNodeId_ = true;
        isCurFirsLevelNodeId_ = true;
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
    static inline void NotifyAll(std::function<void()> condChange)
    {
        if (LIKELY(condChange)) {
            std::unique_lock<std::mutex> lock(notifyMutex_);
            condChange();
        }
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
