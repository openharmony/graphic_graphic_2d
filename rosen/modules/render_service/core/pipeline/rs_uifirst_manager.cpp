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

// unref in sub when cache done
void RSUifirstManager::AddProcessDoneNode(NodeId id)
{
    // mutex
    if (id == INVALID_NODEID) {
        return;
    }
    RS_TRACE_NAME_FMT("sub done %lx", id);
    std::lock_guard<std::mutex> lock(childernDrawableMutex_);
    subthreadProcessDoneNode_.push_back(id);
}

void RSUifirstManager::ProcessDoneNode()
{
    std::vector<NodeId> tmp;
    {
        std::lock_guard<std::mutex> lock(childernDrawableMutex_);
        if (subthreadProcessDoneNode_.size() == 0) {
            RS_TRACE_NAME_FMT("ProcessDoneNode no done");
            return;
        }
        std::swap(tmp, subthreadProcessDoneNode_);
        subthreadProcessDoneNode_.clear();
    }
    RS_TRACE_NAME_FMT("ProcessDoneNode num%d", tmp.size());
    for (auto& id : tmp) {
        RS_TRACE_NAME_FMT("Done %lx", id);
        DrawableV2::RSSurfaceRenderNodeDrawable* drawable = GetSurfaceDrawableByID(id);
        if (!drawable) {
            continue;
        }
        if (drawable->GetCacheSurfaceNeedUpdated() && drawable->GetCacheSurface(UNI_MAIN_THREAD_INDEX, false)) {
            drawable->UpdateCompletedCacheSurface();
        }

        if (pendingResetNodes_.count(id) > 0) { // reset node
            // reset uifirst
            drawable->ResetUifirst();
            pendingResetNodes_.erase(id);
            lastFrameDoingNum_ = -1;
            currentFrameDoingNum_ = -1;
        }
        subthreadProcessingNode_.erase(id);
    }

    for (auto it = subthreadProcessingNode_.begin(); it != subthreadProcessingNode_.end(); it++) {
        pendingPostNodes_.erase(it->first); // dele doing node in pendingpostlist
    }
}

void RSUifirstManager::PostSubTask(NodeId id)
{
    RS_TRACE_NAME("post UpdateCacheSurface");

    if (subthreadProcessingNode_.find(id) != subthreadProcessingNode_.end()) { // drawable is doing, do not send
        RS_TRACE_NAME_FMT("node %lx is doning", id);
        return;
    }
    
    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::GetDrawableById(id); // TODO: 1.find in cache list(done to dele) 2.find in global list
    if (drawable) {
        // ref drawable
        subthreadProcessingNode_[id] = drawable;

        // post task
        RS_TRACE_NAME_FMT("Post_SubTask_s %lx", id);
        RSSubThreadManager::Instance()->ScheduleRenderNodeDrawable(
            static_cast<DrawableV2::RSSurfaceRenderNodeDrawable*>(drawable.get()));
    }
}

void RSUifirstManager::UpdateSkipSyncNode()
{
    processingNodeSkipSync_.clear();
    processingNodePartialSync_.clear();
    RS_TRACE_NAME_FMT("UpdateSkipSyncNode doning%d", subthreadProcessingNode_.size());
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
    RS_TRACE_NAME_FMT("ProcessSubDoneNode");
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
        RS_TRACE_NAME_FMT("set partial_sync %lx root%lx", node->GetId(), node->GetInstanceRootNodeId());
        node->SetUifirstSkipPartialSync(true);
        return false;
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
            RS_TRACE_NAME_FMT("RestoreSkipSyncNode %lx num%d", it.first, it.second.size());
            for (auto& node : it.second) {
                node->SetUifirstSkipPartialSync(false);
                node->SetUifirstSyncFlag(true); // child sync but child will not use, TODO, only sync root
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
    int lastFrameBackup = lastFrameDoingNum_;
    lastFrameDoingNum_ = currentFrameDoingNum_;
    currentFrameDoingNum_ = subthreadProcessingNode_.size();
    if (lastFrameBackup > 0 && lastFrameDoingNum_ == 0 && currentFrameDoingNum_ == 0 &&
        pendingSyncForSkipBefore_.size() == 0 && reuseNodes_.size() == 0) {
        // clear res
        RS_TRACE_NAME_FMT(" postclear last_last%d last%d current%d now%d pendsync %d reuse%d pendpost%d", lastFrameBackup, lastFrameDoingNum_,
            currentFrameDoingNum_, (int)subthreadProcessingNode_.size(), (int)pendingSyncForSkipBefore_.size(),
        (int)reuseNodes_.size(),
        (int)pendingPostNodes_.size());
        RSSubThreadManager::Instance()->ResetSubThreadGrContext();
    }
    reuseNodes_.clear();
}

// post in drawframe sync time
void RSUifirstManager::PostUifistSubTasks()
{
    RS_TRACE_NAME_FMT("PostUifistSubTasks num%d", pendingPostNodes_.size());
    if (pendingPostNodes_.size() > 0) {
        for (auto& item : pendingPostNodes_) {
            PostSubTask(item.first);
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
}

void RSUifirstManager::AddPendingResetNode(NodeId id)
{
    if (id == INVALID_NODEID) {
        return;
    }
    pendingResetNodes_.insert(id);
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
    bool isDisplayRotation = false; // TODO for pc
    bool isPhoneType = RSMainThread::Instance()->GetDeviceType() == DeviceType::PHONE;
    bool isNeedAssignToSubThread = false;
    if (!isPhoneType) { // only enable on phone, disable PC
        return false;
    }
    if (isPhoneType && node.IsLeashWindow()) {
        isNeedAssignToSubThread = (node.IsScale() || ROSEN_EQ(node.GetGlobalAlpha(), 0.0f) ||
            node.GetForceUIFirst()) && !node.HasFilter();
    }
    std::string surfaceName = node.GetName();
    bool needFilterSCB = surfaceName.substr(0, 3) == "SCB" ||
        surfaceName.substr(0, 13) == "BlurComponent"; // filter BlurComponent, 13 is string len

    if (needFilterSCB || node.IsSelfDrawingType()) {
        return false;
    }

    if (isPhoneType) {
        return isNeedAssignToSubThread;
    } else { // PC or TABLET
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
            node.SetUifirstNodeEnableParam(true); // update drawable param
            CheckIfParentUifirstNodeEnable(node, true);
            AddPendingPostNode(node.GetId(), surfaceNode);
            RSMainThread::Instance()->GetContext().AddPendingSyncNode(surfaceNode);
        } else { // keep disable
           RS_TRACE_NAME_FMT("UIFirst_keep disable  %lx", node.GetId());
        }
    } else { // last is enable
        if (currentFrameIsUifirstNode) { // keep enable
            RS_TRACE_NAME_FMT("UIFirst_keep enable  %lx", node.GetId());
            AddPendingPostNode(node.GetId(), surfaceNode);
        } else { // switch: enable -> disable
            RS_TRACE_NAME_FMT("UIFirst_switch enable -> disable %lx", node.GetId());
            node.SetUifirstNodeEnableParam(false); // update drawable param
            AddPendingResetNode(node.GetId());
            RSMainThread::Instance()->GetContext().AddPendingSyncNode(node.shared_from_this());
            CheckIfParentUifirstNodeEnable(node, false);
        }
    }
    node.SetLastFrameUifirstFlag(currentFrameIsUifirstNode);
}

void RSUifirstManager::CheckIfParentUifirstNodeEnable(RSSurfaceRenderNode& node, bool parentUifirstNodeEnable)
{
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
                surfaceChild->SetIsParentUifirstNodeEnableParam(parentUifirstNodeEnable);
                continue;
            }
        }
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
    if (isUiFirstOn_){
        UifirstStateChange(node, IsUifirstNode(node, animation));
    } else {
        DisableUifirstNode(node);
    }
}
#endif
}
}
