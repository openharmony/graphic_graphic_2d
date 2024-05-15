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


#include "pipeline/rs_uifirst_manager.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_main_thread.h"

// use in mainthread, post subthread, not affect renderthread
namespace OHOS {
namespace Rosen {
namespace {
    static constexpr int EVENT_START_TIMEOUT = 500;
    static constexpr int EVENT_STOP_TIMEOUT = 150;
    constexpr std::string_view ARKTSCARDNODE_NAME = "ArkTSCardNode";
    static inline int64_t GetCurSysTime()
    {
        auto curTime = std::chrono::system_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(curTime).count();
    }
};

RSUifirstManager& RSUifirstManager::Instance()
{
    static RSUifirstManager instance; // store in mainthread instance ?
    return instance;
}

DrawableV2::RSSurfaceRenderNodeDrawable* RSUifirstManager::GetSurfaceDrawableByID(NodeId id)
{
    if (const auto cacheIt = subthreadProcessingNode_.find(id); cacheIt != subthreadProcessingNode_.end()) {
        if (const auto ptr = cacheIt->second) {
            return static_cast<DrawableV2::RSSurfaceRenderNodeDrawable*>(ptr.get());
        }
    }
    // unlikely
    auto ptr = DrawableV2::RSRenderNodeDrawableAdapter::GetDrawableById(id);
    if (ptr) {
        return static_cast<DrawableV2::RSSurfaceRenderNodeDrawable*>(ptr.get());
    }
    return nullptr;
}

void RSUifirstManager::SetUifirstNodeEnableParam(RSSurfaceRenderNode& node, MultiThreadCacheType type)
{
    node.SetUifirstNodeEnableParam(type); // update drawable param
    if (node.IsLeashWindow() && type != MultiThreadCacheType::ARKTS_CARD) {
        for (auto& child : *(node.GetChildren())) {
            if (!child) {
                continue;
            }
            auto surfaceChild = child->ReinterpretCastTo<RSSurfaceRenderNode>();
            if (!surfaceChild) {
                continue;
            }
            if (surfaceChild->IsMainWindowType()) {
                surfaceChild->SetIsParentUifirstNodeEnableParam(type == MultiThreadCacheType::LEASH_WINDOW);
                continue;
            }
        }
    }
}

// unref in sub when cache done
void RSUifirstManager::AddProcessDoneNode(NodeId id)
{
    // mutex
    if (id == INVALID_NODEID) {
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("sub done %lx", id);
    std::lock_guard<std::mutex> lock(childernDrawableMutex_);
    subthreadProcessDoneNode_.push_back(id);
}

void RSUifirstManager::MergeOldDirty(RSSurfaceRenderNode& node)
{
    auto params = static_cast<RSSurfaceRenderParams*>(node.GetStagingRenderParams().get());
    if (!params->GetPreSurfaceCacheContentStatic()) {
        return;
    }
    if (node.IsAppWindow() && !RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetParent().lock())) {
        node.GetDirtyManager()->MergeDirtyRect(node.GetOldDirty());
        return;
    }
    for (auto& child : *node.GetSortedChildren()) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
        if (surfaceNode && surfaceNode->IsAppWindow()) {
            surfaceNode->GetDirtyManager()->MergeDirtyRect(surfaceNode->GetOldDirty());
            break;
        }
    }
}

void RSUifirstManager::RenderGroupUpdate(DrawableV2::RSSurfaceRenderNodeDrawable* drawable)
{
    auto surfaceNode = static_cast<const RSSurfaceRenderNode*>(drawable->GetRenderNode().get());
    if (surfaceNode == nullptr) {
        return;
    }
    // mark all parent rendergroup need update; planning: mark autoCache need update
    auto node = surfaceNode->GetParent().lock();
    if (node == nullptr) {
        return;
    }

    do {
        if (node->GetType() == RSRenderNodeType::DISPLAY_NODE) {
            break;
        }
        if (node->IsSuggestedDrawInGroup()) {
            RS_OPTIONAL_TRACE_NAME_FMT("cache_changed by uifirst card %lx", node->GetId());
            node->SetDrawingCacheChanged(true);
            node->AddToPendingSyncList();
        }
        node = node->GetParent().lock();
    } while (node);
}

void RSUifirstManager::ProcessForceUpdateNode()
{
    if (!mainThread_) {
        return;
    }
    std::vector<std::shared_ptr<RSRenderNode>> toDirtyNodes;
    for (auto id : pendingForceUpdateNode_) {
        auto node = mainThread_->GetContext().GetNodeMap().GetRenderNode(id);
        if (!node) {
            continue;
        }
        toDirtyNodes.push_back(node);
        if (!node->IsDirty() && !node->IsSubTreeDirty()) {
            markForceUpdateByUifirst_.push_back(node);
            node->SetForceUpdateByUifirst(true);
        }
        if (node->GetLastFrameUifirstFlag() == MultiThreadCacheType::ARKTS_CARD) {
            continue;
        }
        for (auto& child : *node->GetChildren()) {
            if (!child) {
                continue;
            }
            auto surfaceNode = child->ReinterpretCastTo<RSSurfaceRenderNode>();
            if (!surfaceNode || !surfaceNode->IsMainWindowType()) {
                continue;
            }
            toDirtyNodes.push_back(child);
            if (!child->IsDirty() && !child->IsSubTreeDirty()) {
                markForceUpdateByUifirst_.push_back(child);
                child->SetForceUpdateByUifirst(true);
            }
        }
    }
    for (auto& node : toDirtyNodes) {
        node->SetDirty(true);
    }
    pendingForceUpdateNode_.clear();
}

void RSUifirstManager::ProcessDoneNode()
{
    SetHasDoneNodeFlag(false);
    std::vector<NodeId> tmp;
    {
        std::lock_guard<std::mutex> lock(childernDrawableMutex_);
        std::swap(tmp, subthreadProcessDoneNode_);
        subthreadProcessDoneNode_.clear();
    }
    RS_TRACE_NAME_FMT("ProcessDoneNode num%d", tmp.size());
    for (auto& id : tmp) {
        RS_OPTIONAL_TRACE_NAME_FMT("Done %lx", id);
        DrawableV2::RSSurfaceRenderNodeDrawable* drawable = GetSurfaceDrawableByID(id);
        if (drawable && drawable->GetCacheSurfaceNeedUpdated() &&
            drawable->GetCacheSurface(UNI_MAIN_THREAD_INDEX, false)) {
            drawable->UpdateCompletedCacheSurface();
            RenderGroupUpdate(drawable);
            SetHasDoneNodeFlag(true);
            pendingForceUpdateNode_.push_back(id);
        }
        subthreadProcessingNode_.erase(id);
    }

    // reset node when node is not doing
    for (auto it = pendingResetNodes_.begin(); it != pendingResetNodes_.end();) {
        if (subthreadProcessingNode_.find(it->first) == subthreadProcessingNode_.end()) {
            // reset uifirst
            SetUifirstNodeEnableParam(*it->second, MultiThreadCacheType::NONE);
            RSMainThread::Instance()->GetContext().AddPendingSyncNode(it->second);
            DrawableV2::RSSurfaceRenderNodeDrawable* drawable = GetSurfaceDrawableByID(it->first);
            if (drawable) {
                drawable->ResetUifirst();
            }
            it = pendingResetNodes_.erase(it);
        } else {
            it++;
        }
    }

    for (auto it = subthreadProcessingNode_.begin(); it != subthreadProcessingNode_.end(); it++) {
        pendingPostNodes_.erase(it->first); // dele doing node in pendingpostlist
        pendingPostCardNodes_.erase(it->first);
    }
}


void RSUifirstManager::DoPurgePendingPostNodes(std::unordered_map<NodeId,
    std::shared_ptr<RSSurfaceRenderNode>>& pendingNode)
{
    auto deviceType = RSMainThread::Instance()->GetDeviceType();
    for (auto it = pendingNode.begin(); it != pendingNode.end();) {
        auto id = it->first;
        DrawableV2::RSSurfaceRenderNodeDrawable* drawable = GetSurfaceDrawableByID(id);
        if (!drawable) {
            ++it;
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
        if (!surfaceParams) {
            ++it;
            continue;
        }
        auto node = it->second;
        bool staticContent = node->GetLastFrameUifirstFlag() == MultiThreadCacheType::ARKTS_CARD ?
            node->GetForceUpdateByUifirst() : drawable->IsCurFrameStatic(deviceType);
        if (drawable->HasCachedTexture() && staticContent &&
            (subthreadProcessingNode_.find(id) == subthreadProcessingNode_.end())) {
            RS_OPTIONAL_TRACE_NAME_FMT("Purge node name %s", surfaceParams->GetName().c_str());
            it = pendingNode.erase(it);
        } else {
            ++it;
        }
    }
}

void RSUifirstManager::PurgePendingPostNodes()
{
    RS_OPTIONAL_TRACE_NAME_FMT("PurgePendingPostNodes");
    DoPurgePendingPostNodes(pendingPostNodes_);
    DoPurgePendingPostNodes(pendingPostCardNodes_);
    for (auto& node : markForceUpdateByUifirst_) {
        node->SetForceUpdateByUifirst(false);
    }
    markForceUpdateByUifirst_.clear();
}

void RSUifirstManager::PostSubTask(NodeId id)
{
    RS_TRACE_NAME("post UpdateCacheSurface");

    if (subthreadProcessingNode_.find(id) != subthreadProcessingNode_.end()) { // drawable is doing, do not send
        RS_TRACE_NAME_FMT("node %lx is doning", id);
        RS_LOGE("RSUifirstManager ERROR: post task twice");
        return;
    }

    // 1.find in cache list(done to dele) 2.find in global list
    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::GetDrawableById(id);
    if (drawable) {
        // ref drawable
        subthreadProcessingNode_[id] = drawable;
        // post task
        RS_OPTIONAL_TRACE_NAME_FMT("Post_SubTask_s %lx", id);
        RSSubThreadManager::Instance()->ScheduleRenderNodeDrawable(
            static_cast<DrawableV2::RSSurfaceRenderNodeDrawable*>(drawable.get()));
    }
}

void RSUifirstManager::UpdateSkipSyncNode()
{
    processingNodeSkipSync_.clear();
    processingNodePartialSync_.clear();
    processingCardNodeSkipSync_.clear();
    RS_OPTIONAL_TRACE_NAME_FMT("UpdateSkipSyncNode doning%d", subthreadProcessingNode_.size());
    if (subthreadProcessingNode_.size() == 0) {
        return;
    }
    if (!mainThread_) {
        return;
    }
    for (auto it = subthreadProcessingNode_.begin(); it != subthreadProcessingNode_.end(); it++) {
        RS_TRACE_NAME_FMT("doning%lx", it->first);
        auto node = mainThread_->GetContext().GetNodeMap().GetRenderNode(it->first);
        if (!node) {
            continue;
        }
        auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (!surfaceNode) {
            continue;
        }
        // ArkTSCard
        if (NodeIsInCardWhiteList(*node)) {
            if (surfaceNode->GetLastFrameUifirstFlag() == MultiThreadCacheType::ARKTS_CARD) {
                processingCardNodeSkipSync_.insert(it->first);
                continue;
            }
        }

        // leash window
        processingNodePartialSync_.insert(it->first); // partial sync
        std::vector<std::pair<NodeId, std::weak_ptr<RSSurfaceRenderNode>>> allSubSurfaceNodes;
        surfaceNode->GetAllSubSurfaceNodes(allSubSurfaceNodes);
        for (auto& [id, node] : allSubSurfaceNodes) {
            processingNodeSkipSync_.insert(id); // skip sync
        }
    }
}

void RSUifirstManager::ProcessSubDoneNode()
{
    RS_OPTIONAL_TRACE_NAME_FMT("ProcessSubDoneNode");
    ProcessDoneNode(); // release finish drawable
    UpdateSkipSyncNode();
    RestoreSkipSyncNode();
}

bool RSUifirstManager::CollectSkipSyncNode(const std::shared_ptr<RSRenderNode> &node)
{
    if (!node) {
        return false;
    }
    if (pendingPostNodes_.find(node->GetId()) != pendingPostNodes_.end() ||
        pendingPostCardNodes_.find(node->GetId()) != pendingPostCardNodes_.end()) {
        node->SetUifirstSyncFlag(true);
    }

    if (NodeIsInCardWhiteList(*node) && processingCardNodeSkipSync_.count(node->GetUifirstRootNodeId())) {
        pendingSyncForSkipBefore_[node->GetUifirstRootNodeId()].push_back(node);
        if (node->GetUifirstRootNodeId() == node->GetId()) {
            RS_OPTIONAL_TRACE_NAME_FMT("set partial_sync card %lx root%lx",
                node->GetId(), node->GetUifirstRootNodeId());
            node->SetUifirstSkipPartialSync(true);
            return false;
        } else {
            RS_OPTIONAL_TRACE_NAME_FMT("CollectSkipSyncNode card root %lx, node %lx",
                node->GetUifirstRootNodeId(), node->GetId());
            return true;
        }
    }
    if (processingNodePartialSync_.count(node->GetInstanceRootNodeId()) > 0) {
        pendingSyncForSkipBefore_[node->GetInstanceRootNodeId()].push_back(node);
        if (node->GetInstanceRootNodeId() == node->GetId()) {
            RS_OPTIONAL_TRACE_NAME_FMT("set partial_sync %lx root%lx", node->GetId(), node->GetInstanceRootNodeId());
            node->SetUifirstSkipPartialSync(true);
            return false;
        } else {
            RS_OPTIONAL_TRACE_NAME_FMT("CollectSkipSyncNode root %lx, node %lx",
                node->GetInstanceRootNodeId(), node->GetId());
            return true;
        }
    } else if (processingNodeSkipSync_.count(node->GetInstanceRootNodeId()) > 0) {
        RS_OPTIONAL_TRACE_NAME_FMT("CollectSkipSyncNode root %lx, node %lx",
            node->GetInstanceRootNodeId(), node->GetId());
        pendingSyncForSkipBefore_[node->GetInstanceRootNodeId()].push_back(node);
        return true;
    } else {
        return false;
    }
}

void RSUifirstManager::RestoreSkipSyncNode()
{
    std::vector<NodeId> todele;
    for (auto& it : pendingSyncForSkipBefore_) {
        if (processingNodeSkipSync_.count(it.first) == 0 && processingNodePartialSync_.count(it.first) == 0 &&
            processingCardNodeSkipSync_.count(it.first) == 0) {
            todele.push_back(it.first);
            RS_OPTIONAL_TRACE_NAME_FMT("RestoreSkipSyncNode %lx num%d", it.first, it.second.size());
            for (auto& node : it.second) {
                node->SetUifirstSkipPartialSync(false);
                node->AddToPendingSyncList();
            }
        }
    }
    for (auto id : todele) {
        pendingSyncForSkipBefore_.erase(id);
    }
}

void RSUifirstManager::ClearSubthreadRes()
{
    RS_OPTIONAL_TRACE_NAME_FMT("ClearSubthreadRes");
    if (subthreadProcessingNode_.size() == 0 &&
        pendingSyncForSkipBefore_.size() == 0) {
        if (noUifirstNodeFrameCount_ < CLEAR_RES_THRESHOLD) {
            ++noUifirstNodeFrameCount_;
            if (noUifirstNodeFrameCount_ == CLEAR_RES_THRESHOLD) {
                RSSubThreadManager::Instance()->ResetSubThreadGrContext();
            }
        }
    } else {
        noUifirstNodeFrameCount_ = 0;
    }
    reuseNodes_.clear();
}

void RSUifirstManager::ForceClearSubthreadRes()
{
    noUifirstNodeFrameCount_ = 0;
    RSSubThreadManager::Instance()->ForceReleaseResource();
}

void RSUifirstManager::SetNodePriorty(std::list<NodeId>& result,
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>& pendingNode)
{
    bool isFocusNodeFound = false;
    for (auto& item : pendingNode) {
        auto const& [id, value] = item;
        DrawableV2::RSSurfaceRenderNodeDrawable* drawable = GetSurfaceDrawableByID(id);
        if (!drawable) {
            continue;
        }
        if (!isFocusNodeFound) {
            bool isFocus = ((id == RSMainThread::Instance()->GetFocusNodeId()) ||
                (id == RSMainThread::Instance()->GetFocusLeashWindowId()));
            if (isFocus) {
                // for resolving response latency
                drawable->SetRenderCachePriority(NodePriorityType::SUB_FOCUSNODE_PRIORITY);
                isFocusNodeFound = true;
            }
        }
        if (drawable->HasCachedTexture()) {
            drawable->SetRenderCachePriority(NodePriorityType::SUB_LOW_PRIORITY);
        } else {
            drawable->SetRenderCachePriority(NodePriorityType::SUB_HIGH_PRIORITY);
        }
        result.emplace_back(id);
    }
}

void RSUifirstManager::SortSubThreadNodesPriority()
{
    sortedSubThreadNodeIds_.clear();
    SetNodePriorty(sortedSubThreadNodeIds_, pendingPostNodes_);
    SetNodePriorty(sortedSubThreadNodeIds_, pendingPostCardNodes_);

    sortedSubThreadNodeIds_.sort([this](const auto& first, const auto& second) -> bool {
        auto drawable1 = GetSurfaceDrawableByID(first);
        auto drawable2 = GetSurfaceDrawableByID(second);
        if (drawable1 == nullptr || drawable2 == nullptr) {
            ROSEN_LOGE("RSUifirstManager::SortSubThreadNodesPriority sort nullptr found in pendingPostNodes_, "
                "this should not happen");
            return false;
        }
        auto surfaceParams1 = static_cast<RSSurfaceRenderParams*>(drawable1->GetRenderParams().get());
        if (!surfaceParams1) {
            RS_LOGE("RSSurfaceRenderNodeDrawable::sortsubthread params1 is nullptr");
            return false;
        }
        auto surfaceParams2 = static_cast<RSSurfaceRenderParams*>(drawable2->GetRenderParams().get());
        if (!surfaceParams2) {
            RS_LOGE("RSSurfaceRenderNodeDrawable::sortsubthread params2 is nullptr");
            return false;
        }
        if (drawable1->GetRenderCachePriority() == drawable2->GetRenderCachePriority()) {
            return surfaceParams2->GetPositionZ() < surfaceParams1->GetPositionZ();
        } else {
            return drawable1->GetRenderCachePriority() < drawable2->GetRenderCachePriority();
        }
    });
}

// post in drawframe sync time
void RSUifirstManager::PostUifistSubTasks()
{
    PurgePendingPostNodes();
    SortSubThreadNodesPriority();
    if (sortedSubThreadNodeIds_.size() > 0) {
        for (auto& id : sortedSubThreadNodeIds_) {
            PostSubTask(id);
        }
        pendingPostNodes_.clear();
        pendingPostCardNodes_.clear();
        sortedSubThreadNodeIds_.clear();
    } else {
        ClearSubthreadRes();
    }
}

bool RSUifirstManager::IsInLeashWindowTree(RSSurfaceRenderNode& node, NodeId instanceRootId)
{
    if (node.GetInstanceRootNodeId() == instanceRootId) {
        return true;
    }
    if (!node.IsLeashWindow()) {
        return false;
    }
    for (auto& child : *(node.GetChildren())) {
        if (!child) {
            continue;
        }
        auto surfaceChild = child->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (!surfaceChild) {
            continue;
        }
        if (surfaceChild->GetInstanceRootNodeId() == instanceRootId) {
            return true;
        }
    }
    return false;
}

void RSUifirstManager::AddPendingPostNode(NodeId id, std::shared_ptr<RSSurfaceRenderNode>& node,
    MultiThreadCacheType currentFrameCacheType)
{
    if (id == INVALID_NODEID) {
        return;
    }

    // process for uifirst node
    UpdateChildrenDirtyRect(*node);
    node->SetHwcChildrenDisabledStateByUifirst();
    node->AddToPendingSyncList();

    if (currentFrameCacheType == MultiThreadCacheType::LEASH_WINDOW) {
        // dele card node in leashwindow tree
        for (auto it = pendingPostCardNodes_.begin(); it != pendingPostCardNodes_.end();) {
            auto surfaceNode = it->second;
            if (surfaceNode && IsInLeashWindowTree(*node, surfaceNode->GetInstanceRootNodeId())) {
                DisableUifirstNode(*surfaceNode);
                it = pendingPostCardNodes_.erase(it);
            } else {
                it++;
            }
        }
        pendingPostNodes_[id] = node;
    } else if (currentFrameCacheType == MultiThreadCacheType::ARKTS_CARD) {
        pendingPostCardNodes_[id] = node;
    }

    if (pendingResetNodes_.count(id)) {
        pendingResetNodes_.erase(id); // enable uifirst when waiting for reset
    }
}

void RSUifirstManager::AddPendingResetNode(NodeId id, std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (id == INVALID_NODEID) {
        return;
    }
    pendingResetNodes_[id] = node;
}

CacheProcessStatus RSUifirstManager::GetNodeStatus(NodeId id)
{
    DrawableV2::RSSurfaceRenderNodeDrawable* drawable = GetSurfaceDrawableByID(id);
    if (drawable) {
        return drawable->GetCacheSurfaceProcessedStatus();
    }
    return CacheProcessStatus::UNKNOWN;
}

void RSUifirstManager::UpdateCompletedSurface(NodeId id)
{
    DrawableV2::RSSurfaceRenderNodeDrawable* drawable = GetSurfaceDrawableByID(id);
    if (drawable) {
        drawable->UpdateCompletedCacheSurface();
    }
}

// add&clear in render
void RSUifirstManager::AddReuseNode(NodeId id)
{
    if (id == INVALID_NODEID) {
        return;
    }
    reuseNodes_.insert(id);
}

void RSUifirstManager::OnProcessEventResponse(DataBaseRs& info)
{
    EventInfo eventInfo = {GetCurSysTime(), 0, info.uniqueId, info.appPid, info.sceneId};
    std::lock_guard<std::mutex> lock(globalFrameEventMutex_);
    globalFrameEvent_.push_back(std::move(eventInfo));
    currentFrameCanSkipFirstWait_ = EventsCanSkipFirstWait(globalFrameEvent_);
}

void RSUifirstManager::OnProcessEventComplete(DataBaseRs& info)
{
    int64_t curSysTime = GetCurSysTime();
    std::lock_guard<std::mutex> lock(globalFrameEventMutex_);
    for (auto it = globalFrameEvent_.begin(); it != globalFrameEvent_.end(); it++) {
        if (it->uniqueId == info.uniqueId && it->sceneId == info.sceneId) {
            // delay delete for animation continue
            it->stopTime = curSysTime;
            break;
        }
    }
}

void RSUifirstManager::PrepareCurrentFrameEvent()
{
    int64_t curSysTime = GetCurSysTime();
    currentFrameEvent_.clear();
    if (!mainThread_ || entryViewNodeId_ == INVALID_NODEID || negativeScreenNodeId_ == INVALID_NODEID) {
        mainThread_ = RSMainThread::Instance();
        if (mainThread_) {
            entryViewNodeId_ = mainThread_->GetContext().GetNodeMap().GetEntryViewNodeId();
            negativeScreenNodeId_ = mainThread_->GetContext().GetNodeMap().GetNegativeScreenNodeId();
            scbPid_ = ExtractPid(entryViewNodeId_);
        }
    }
    {
        std::lock_guard<std::mutex> lock(globalFrameEventMutex_);
        for (auto it = globalFrameEvent_.begin(); it != globalFrameEvent_.end();) {
            if (it->stopTime != 0 &&
                ((curSysTime > it->stopTime) && (curSysTime - it->stopTime) > EVENT_STOP_TIMEOUT)) {
                it = globalFrameEvent_.erase(it);
                continue;
            }

            if ((curSysTime > it->startTime) && (curSysTime - it->startTime) > EVENT_START_TIMEOUT) {
                it = globalFrameEvent_.erase(it);
                continue;
            }
            it++;
        }
        if (globalFrameEvent_.empty()) {
            currentFrameCanSkipFirstWait_ = false;
            return;
        }
        // copy global to current, judge leashwindow stop
        currentFrameEvent_.assign(globalFrameEvent_.begin(), globalFrameEvent_.end());
    }
    currentFrameCanSkipFirstWait_ = EventsCanSkipFirstWait(currentFrameEvent_);
}

bool RSUifirstManager::NodeIsInCardWhiteList(RSRenderNode& node)
{
    if ((entryViewNodeId_ != INVALID_NODEID) && (negativeScreenNodeId_ != INVALID_NODEID)) {
        auto instanceRootId = node.GetInstanceRootNodeId();
        if (instanceRootId == entryViewNodeId_ || instanceRootId == negativeScreenNodeId_) {
            return true;
        }
    }
    return false;
}

bool RSUifirstManager::IsCardSkipFirstWaitScene(std::string& scene, int32_t appPid)
{
    if (appPid != scbPid_) {
        return false;
    }
    for (auto& item : cardCanSkipFirstWaitScene_) {
        if ((scene.find(item) != std::string::npos)) {
            return true;
        }
    }
    return false;
}

bool RSUifirstManager::EventsCanSkipFirstWait(std::vector<EventInfo>& events)
{
    if (events.empty()) {
        return false;
    }
    for (auto& item : events) {
        if (IsCardSkipFirstWaitScene(item.sceneId, item.appPid)) {
            return true;
        }
    }
    return false;
}

bool RSUifirstManager::CheckIfAppWindowHasAnimation(RSSurfaceRenderNode& node)
{
    if (currentFrameEvent_.empty()) {
        return false;
    }

    std::set<int32_t> appPids;
    if (node.IsAppWindow()) {
        appPids.insert(ExtractPid(node.GetId()));
    } else if (node.IsLeashWindow()) {
        for (auto& child : *(node.GetChildren())) {
            if (!child) {
                continue;
            }
            auto surfaceChild = child->ReinterpretCastTo<RSSurfaceRenderNode>();
            if (!surfaceChild) {
                continue;
            }
            if (surfaceChild->IsAppWindow()) {
                appPids.insert(ExtractPid(surfaceChild->GetId()));
            }
        }
    }

    if (appPids.empty()) {
        return false;
    }
    for (auto& item : currentFrameEvent_) {
        if (appPids.count(item.appPid)) {
            return true; // app has animation, stop leashwindow uifirst
        }
    }
    return false;
}

bool RSUifirstManager::IsUifirstNode(RSSurfaceRenderNode& node, bool animation)
{
    bool isDisplayRotation = false; // planning: for pc
    bool isUIFirstEnable = RSMainThread::Instance()->GetDeviceType() != DeviceType::PC;
    bool isNeedAssignToSubThread = false;
    if (!isUIFirstEnable) { // only enable on phone and tablet, disable PC
        return false;
    }
    if (node.GetFirstLevelNodeId() != node.GetId() || RSUifirstManager::Instance().NodeIsInCardWhiteList(node)) {
        return false;
    }
    // 1: Planning: support multi appwindows
    if (isUIFirstEnable && node.IsLeashWindow()) {
        isNeedAssignToSubThread = (animation || ROSEN_EQ(node.GetGlobalAlpha(), 0.0f) ||
            node.GetForceUIFirst()) && !node.HasFilter() && !RSUifirstManager::Instance().rotationChanged_;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("Assign info: name[%s] id[%lu]"
        " filter:%d animation:%d forceUIFirst:%d isNeedAssign:%d",
        node.GetName().c_str(), node.GetId(),
        node.HasFilter(), animation, node.GetForceUIFirst(), isNeedAssignToSubThread);
    std::string surfaceName = node.GetName();
    bool needFilterSCB = surfaceName.substr(0, 3) == "SCB" ||
        surfaceName.substr(0, 13) == "BlurComponent"; // filter BlurComponent, 13 is string len
    if (needFilterSCB || node.IsSelfDrawingType()) {
        return false;
    }
    if (isUIFirstEnable) {
        return isNeedAssignToSubThread;
    } else { // PC
        if ((node.IsFocusedNode(RSMainThread::Instance()->GetFocusNodeId()) ||
            node.IsFocusedNode(RSMainThread::Instance()->GetFocusLeashWindowId())) &&
            node.GetHasSharedTransitionNode()) {
            return false;
        }
        return node.QuerySubAssignable(isDisplayRotation);
    }
}

bool RSUifirstManager::IsArkTsCardCache(RSSurfaceRenderNode& node, bool animation) // maybe canvas node ?
{
    bool flag = ((RSMainThread::Instance()->GetDeviceType() == DeviceType::PHONE) &&
        (node.GetSurfaceNodeType() == RSSurfaceNodeType::ABILITY_COMPONENT_NODE) &&
        RSUifirstManager::Instance().NodeIsInCardWhiteList(node) &&
        (node.ShouldPaint()) && (node.GetName().find(ARKTSCARDNODE_NAME) != std::string::npos));
    if (flag) { // Planning: mark by arkui or app
        return true;
    }
    return false;
}

// animation first, may reuse last image cache
bool RSUifirstManager::IsLeashWindowCache(RSSurfaceRenderNode& node, bool animation)
{
    return RSUifirstManager::IsUifirstNode(node, animation) &&
        !RSUifirstManager::Instance().CheckIfAppWindowHasAnimation(node);
}

void RSUifirstManager::UpdateUifirstNodes(RSSurfaceRenderNode& node, bool ancestorNodeHasAnimation)
{
    RS_OPTIONAL_TRACE_NAME_FMT("UpdateUifirstNodes: node[%llu] name[%s] FirstLevelNodeId[%llu] ",
        node.GetId(), node.GetName().c_str(), node.GetFirstLevelNodeId());

    auto currentFrameCacheType = MultiThreadCacheType::NONE;
    do {
        if (!isUiFirstOn_ || !node.GetUifirstSupportFlag()) {
            break;
        }
        if (RSUifirstManager::IsLeashWindowCache(node, ancestorNodeHasAnimation)) {
            currentFrameCacheType = MultiThreadCacheType::LEASH_WINDOW;
            break;
        }
        if (RSUifirstManager::IsArkTsCardCache(node, ancestorNodeHasAnimation)) {
            currentFrameCacheType = MultiThreadCacheType::ARKTS_CARD;
            break;
        }
    } while (false);
    UifirstStateChange(node, currentFrameCacheType);
}

void RSUifirstManager::UifirstStateChange(RSSurfaceRenderNode& node, MultiThreadCacheType currentFrameCacheType)
{
    auto lastFrameCacheType = node.GetLastFrameUifirstFlag();
    if ((lastFrameCacheType != MultiThreadCacheType::NONE) && (lastFrameCacheType != currentFrameCacheType)) {
        // not support cache type switch, just disable multithread cache
        currentFrameCacheType = MultiThreadCacheType::NONE;
    }
    auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.shared_from_this());
    if (lastFrameCacheType == MultiThreadCacheType::NONE) { // likely branch: last is disable
        if (currentFrameCacheType != MultiThreadCacheType::NONE) { // switch: disable -> enable
            RS_TRACE_NAME_FMT("UIFirst_switch disable -> enable %lx", node.GetId());
            SetUifirstNodeEnableParam(node, currentFrameCacheType);
            if (currentFrameCacheType == MultiThreadCacheType::ARKTS_CARD) { // now only update ArkTSCardNode
                node.UpdateTreeUifirstRootNodeId(node.GetId());
            }
            auto func = std::bind(&RSUifirstManager::ProcessTreeStateChange, std::placeholders::_1);
            node.RegisterTreeStateChangeCallback(func);
            AddPendingPostNode(node.GetId(), surfaceNode, currentFrameCacheType); // clear pending reset status
        } else { // keep disable
            RS_TRACE_NAME_FMT("UIFirst_keep disable  %lx", node.GetId());
        }
    } else { // last is enable
        if (currentFrameCacheType != MultiThreadCacheType::NONE) { // keep enable
            RS_TRACE_NAME_FMT("UIFirst_keep enable  %lx", node.GetId());
            MergeOldDirty(node);
            AddPendingPostNode(node.GetId(), surfaceNode, currentFrameCacheType);
        } else { // switch: enable -> disable
            RS_TRACE_NAME_FMT("UIFirst_switch enable -> disable %lx", node.GetId());
            AddPendingResetNode(node.GetId(), surfaceNode); // set false onsync when task done
        }
    }
    node.SetLastFrameUifirstFlag(currentFrameCacheType);
}

// appwindow will not be traversed in process when cache leashwindow
void RSUifirstManager::UpdateChildrenDirtyRect(RSSurfaceRenderNode& node)
{
    RectI rect(0, 0, 0, 0);
    if (node.IsLeashWindow()) {
        for (auto& child : *(node.GetChildren())) {
            if (!child) {
                continue;
            }
            auto surfaceChild = child->ReinterpretCastTo<RSSurfaceRenderNode>();
            if (!surfaceChild) {
                continue;
            }
            if (surfaceChild->IsMainWindowType()) {
                rect = rect.JoinRect(surfaceChild->GetOldDirtyInSurface());
                continue;
            }
        }
    }
    node.SetUifirstChildrenDirtyRectParam(rect);
}

void RSUifirstManager::ProcessTreeStateChange(RSSurfaceRenderNode& node)
{
    // Planning: do not clear complete image for card
    if (!node.IsOnTheTree()) {
        RSUifirstManager::Instance().DisableUifirstNode(node);
        RSUifirstManager::Instance().ForceClearSubthreadRes();
    }
}

void RSUifirstManager::DisableUifirstNode(RSSurfaceRenderNode& node)
{
    RS_TRACE_NAME_FMT("DisableUifirstNode");
    UifirstStateChange(node, MultiThreadCacheType::NONE);
}
} // namespace Rosen
} // namespace OHOS
