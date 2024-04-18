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
#ifdef RS_PARALLEL

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

static inline void SetUifirstNodeEnableParam(RSSurfaceRenderNode& node, bool enable)
{
    node.SetUifirstNodeEnableParam(enable); // update drawable param
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
                surfaceChild->SetIsParentUifirstNodeEnableParam(enable);
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
            SetHasDoneNodeFlag(true);
        }
        subthreadProcessingNode_.erase(id);
    }

    // reset node when node is not doing
    for (auto it = pendingResetNodes_.begin(); it != pendingResetNodes_.end();) {
        if (subthreadProcessingNode_.find(it->first) == subthreadProcessingNode_.end()) {
            // reset uifirst
            SetUifirstNodeEnableParam(*it->second, false);
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
    }
}

void RSUifirstManager::PurgePendingPostNodes()
{
    RS_OPTIONAL_TRACE_NAME_FMT("PurgePendingPostNodes");
    auto deviceType = RSMainThread::Instance()->GetDeviceType();
    for (auto it = pendingPostNodes_.begin(); it != pendingPostNodes_.end();) {
        auto id = it->first;
        DrawableV2::RSSurfaceRenderNodeDrawable* drawable = GetSurfaceDrawableByID(id);
        if (drawable) {
            if (drawable->HasCachedTexture() && drawable->IsCurFrameStatic(deviceType) &&
                (subthreadProcessingNode_.find(id) == subthreadProcessingNode_.end())) {
                auto surfaceParams = static_cast<RSSurfaceRenderParams*>(
                    drawable->GetRenderNode()->GetRenderParams().get());
                if (!surfaceParams) {
                    RS_LOGE("PurgePendingPostNodes params is nullptr");
                    ++it;
                    continue;
                }
                RS_OPTIONAL_TRACE_NAME_FMT("Purge node name %s", surfaceParams->GetName().c_str());
                it = pendingPostNodes_.erase(it);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
}

void RSUifirstManager::UpdateUifirstNodes(RSSurfaceRenderNode& node, bool ancestorNodeHasAnimation)
{
    RS_OPTIONAL_TRACE_NAME_FMT("UpdateUifirstNodes: node[%llu] name[%s] FirstLevelNodeId[%llu] ",
        node.GetId(), node.GetName().c_str(), node.GetFirstLevelNodeId());
    // UIFirst Enable state is signed only when node's firstLevelNode is itself
    if (node.GetFirstLevelNodeId() == node.GetId() && node.GetUifirstSupportFlag()) {
        RSUifirstManager::Instance().PrepareUifirstNode(node, ancestorNodeHasAnimation);
    } else {
        RSUifirstManager::Instance().DisableUifirstNode(node);
    }
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
    RS_OPTIONAL_TRACE_NAME_FMT("UpdateSkipSyncNode doning%d", subthreadProcessingNode_.size());
    if (subthreadProcessingNode_.size() == 0) {
        return;
    }
    for (auto it = subthreadProcessingNode_.begin(); it != subthreadProcessingNode_.end(); it++) {
        RS_TRACE_NAME_FMT("doning%lx", it->first);
        
        DrawableV2::RSSurfaceRenderNodeDrawable* drawable = GetSurfaceDrawableByID(it->first);
        if (!drawable) {
            continue;
        }
        if (!drawable->GetRenderNode()) {
            continue;
        }

        processingNodePartialSync_.insert(it->first); // partial sync
        for (auto& child : *(drawable->GetRenderNode()->GetChildren())) {
            if (!child) {
                continue;
            }
            processingNodeSkipSync_.insert(child->GetInstanceRootNodeId()); // skip sync
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
    if (pendingPostNodes_.find(node->GetId()) != pendingPostNodes_.end()) { // only sync root node's uifirstDrawCmdList_
        node->SetUifirstSyncFlag(true);
    }
    if (processingNodePartialSync_.count(node->GetInstanceRootNodeId()) > 0) {
        pendingSyncForSkipBefore_[node->GetInstanceRootNodeId()].push_back(node);
        if (node->GetInstanceRootNodeId() == node->GetId()) {
            RS_TRACE_NAME_FMT("set partial_sync %lx root%lx", node->GetId(), node->GetInstanceRootNodeId());
            node->SetUifirstSkipPartialSync(true);
            return false;
        } else {
            RS_TRACE_NAME_FMT("CollectSkipSyncNode root %lx, node %lx", node->GetInstanceRootNodeId(), node->GetId());
            return true;
        }
    } else if (processingNodeSkipSync_.count(node->GetInstanceRootNodeId()) > 0) {
        RS_TRACE_NAME_FMT("CollectSkipSyncNode root %lx, node %lx", node->GetInstanceRootNodeId(), node->GetId());
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
        if (processingNodeSkipSync_.count(it.first) == 0 && processingNodePartialSync_.count(it.first) == 0) {
            todele.push_back(it.first);
            RS_OPTIONAL_TRACE_NAME_FMT("RestoreSkipSyncNode %lx num%d", it.first, it.second.size());
            for (auto& node : it.second) {
                node->SetUifirstSkipPartialSync(false);
                node->SetUifirstSyncFlag(true); // child sync but child will not use, planning: only sync root
                RSMainThread::Instance()->GetContext().AddPendingSyncNode(node);
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
        pendingSyncForSkipBefore_.size() == 0 &&
        reuseNodes_.size() == 0) {
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

void RSUifirstManager::SortSubThreadNodesPriority()
{
    bool isFocusNodeFound = false;
    sortedSubThreadNodeIds_.clear();
    for (auto& item : pendingPostNodes_) {
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
        sortedSubThreadNodeIds_.emplace_back(id);
    }
    sortedSubThreadNodeIds_.sort([this](const auto& first, const auto& second) -> bool {
        auto drawable1 = GetSurfaceDrawableByID(first);
        auto drawable2 = GetSurfaceDrawableByID(second);
        if (drawable1 == nullptr || drawable2 == nullptr) {
            ROSEN_LOGE("RSUifirstManager::SortSubThreadNodesPriority sort nullptr found in pendingPostNodes_, "
                "this should not happen");
            return false;
        }
        auto surfaceParams1 = static_cast<RSSurfaceRenderParams*>(drawable1->GetRenderNode()->GetRenderParams().get());
        if (!surfaceParams1) {
            RS_LOGE("RSSurfaceRenderNodeDrawable::sortsubthread params1 is nullptr");
            return false;
        }
        auto surfaceParams2 = static_cast<RSSurfaceRenderParams*>(drawable2->GetRenderNode()->GetRenderParams().get());
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
    } else {
        ClearSubthreadRes();
    }
}

void RSUifirstManager::AddPendingPostNode(NodeId id, std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (id == INVALID_NODEID) {
        return;
    }
    pendingPostNodes_[id] = node;
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

bool RSUifirstManager::IsUifirstNode(RSSurfaceRenderNode& node, bool animation)
{
    bool isDisplayRotation = false; // planning: for pc
    bool isUIFirstEnable = RSMainThread::Instance()->GetDeviceType() != DeviceType::PC;
    bool isNeedAssignToSubThread = false;
    if (!isUIFirstEnable) { // only enable on phone and tablet, disable PC
        return false;
    }
    if (isUIFirstEnable && node.IsLeashWindow()) {
        isNeedAssignToSubThread = (node.IsScale() || ROSEN_EQ(node.GetGlobalAlpha(), 0.0f) ||
            node.GetForceUIFirst()) && !node.HasFilter();
    }
    RS_TRACE_NAME_FMT("Assign info: name[%s] id[%lu]"
        " filter:%d isScale:%d forceUIFirst:%d isNeedAssign:%d",
        node.GetName().c_str(), node.GetId(),
        node.HasFilter(), node.IsScale(), node.GetForceUIFirst(), isNeedAssignToSubThread);
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

void RSUifirstManager::UifirstStateChange(RSSurfaceRenderNode& node, bool currentFrameIsUifirstNode)
{
    auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.shared_from_this());
    bool lastFrameIsUifirstNode = node.GetLastFrameUifirstFlag();
    if (!lastFrameIsUifirstNode) { // likely branch: last is disable
        if (currentFrameIsUifirstNode) { // switch: disable -> enable
            RS_TRACE_NAME_FMT("UIFirst_switch disable -> enable %lx", node.GetId());
            SetUifirstNodeEnableParam(node, true);
            UpdateChildrenDirtyRect(*surfaceNode);
            node.SetHwcChildrenDisabledStateByUifirst();
            auto func = std::bind(&RSUifirstManager::ProcessTreeStateChange, std::placeholders::_1);
            node.RegisterTreeStateChangeCallback(func);
            AddPendingPostNode(node.GetId(), surfaceNode); // clear pending reset status
            RSMainThread::Instance()->GetContext().AddPendingSyncNode(surfaceNode);
        } else { // keep disable
            RS_TRACE_NAME_FMT("UIFirst_keep disable  %lx", node.GetId());
        }
    } else { // last is enable
        if (currentFrameIsUifirstNode) { // keep enable
            RS_TRACE_NAME_FMT("UIFirst_keep enable  %lx", node.GetId());
            UpdateChildrenDirtyRect(*surfaceNode);
            node.SetHwcChildrenDisabledStateByUifirst();
            MergeOldDirty(node);
            AddPendingPostNode(node.GetId(), surfaceNode);
        } else { // switch: enable -> disable
            RS_TRACE_NAME_FMT("UIFirst_switch enable -> disable %lx", node.GetId());
            AddPendingResetNode(node.GetId(), surfaceNode); // set false onsync when task done
        }
    }
    node.SetLastFrameUifirstFlag(currentFrameIsUifirstNode);
}

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
    if (!node.IsOnTheTree()) {
        RSUifirstManager::Instance().UifirstStateChange(node, false);
        RSUifirstManager::Instance().ForceClearSubthreadRes();
    }
}

void RSUifirstManager::DisableUifirstNode(RSSurfaceRenderNode& node)
{
    RS_TRACE_NAME_FMT("DisableUifirstNode");
    UifirstStateChange(node, false);
}

void RSUifirstManager::PrepareUifirstNode(RSSurfaceRenderNode& node, bool animation)
{
    RS_TRACE_NAME_FMT("PrepareUifirstNode");
    if (isUiFirstOn_) {
        UifirstStateChange(node, IsUifirstNode(node, animation));
    } else {
        DisableUifirstNode(node);
    }
}
#endif
}
}
