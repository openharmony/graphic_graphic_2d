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

#include "params/rs_render_params.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
#include "string_utils.h"
#endif
#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {

constexpr int64_t MIN_REUSECOUNT = 10;
constexpr int64_t MAX_REUSECOUNT = 60;
constexpr int32_t MAX_TRY_TIMES = 3;

// mark stable node
void RSRenderNode::OpincSetInAppStateStart(bool& unchangeMarkInApp)
{
    if (unchangeMarkInApp) {
        return;
    }
    isUnchangeMarkInApp_ = true;
    unchangeMarkInApp = true;
}

void RSRenderNode::OpincSetInAppStateEnd(bool& unchangeMarkInApp)
{
    if (isUnchangeMarkInApp_) {
        isUnchangeMarkInApp_ = false;
        unchangeMarkInApp = false;
    }
}

void RSRenderNode::OpincQuickMarkStableNode(bool& unchangeMarkInApp, bool& unchangeMarkEnable,
    bool isAccessibilityChanged)
{
    if (!unchangeMarkInApp) {
        return;
    }
    if (GetSuggestOpincNode() && !unchangeMarkEnable) {
        isUnchangeMarkEnable_ = true;
        unchangeMarkEnable = true;
    }
    if (!unchangeMarkEnable) {
        return;
    }
    auto isSelfDirty = IsSubTreeDirty() || IsContentDirty() ||
        nodeGroupType_ > RSRenderNode::NodeGroupType::NONE || (isOpincRootFlag_ && isAccessibilityChanged);
    if (isSelfDirty) {
        NodeCacheStateChange(NodeChangeType::SELF_DIRTY);
    } else if (nodeCacheState_ != NodeCacheState::STATE_UNCHANGE) {
        NodeCacheStateChange(NodeChangeType::KEEP_UNCHANGE);
    }
}

void RSRenderNode::OpincUpdateRootFlag(bool& unchangeMarkEnable)
{
    if (unchangeMarkEnable) {
        if (IsOpincUnchangeState()) {
            isOpincRootFlag_ = true;
            if (stagingRenderParams_) {
                stagingRenderParams_->OpincUpdateRootFlag(true);
            }
        }
    }
    if (isUnchangeMarkEnable_) {
        isUnchangeMarkEnable_ = false;
        unchangeMarkEnable = false;
    }
}

bool RSRenderNode::IsOpincUnchangeState()
{
    return (nodeCacheState_ == NodeCacheState::STATE_UNCHANGE) && OpincGetNodeSupportFlag() && GetSuggestOpincNode();
}

std::string RSRenderNode::QuickGetNodeDebugInfo()
{
    std::string ret("");
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
    AppendFormat(ret, "%llx, IsSTD:%d s:%d uc:%d suggest:%d support:%d rootF:%d",
        GetId(), IsSubTreeDirty(), nodeCacheState_, unchangeCount_, GetSuggestOpincNode(),
        OpincGetNodeSupportFlag(), isOpincRootFlag_);
#endif
    return ret;
}

// mark support node
void RSRenderNode::OpincUpdateNodeSupportFlag(bool supportFlag)
{
    isOpincNodeSupportFlag_ = isOpincNodeSupportFlag_ && supportFlag && (!IsMarkedRenderGroup());
}

bool RSRenderNode::IsMarkedRenderGroup()
{
    return (nodeGroupType_ > RSRenderNode::NodeGroupType::NONE) || isOpincRootFlag_;
}

bool RSRenderNode::OpincForcePrepareSubTree()
{
    bool flag = (!(IsSubTreeDirty() || IsContentDirty())) && GetSuggestOpincNode() &&
        OpincGetNodeSupportFlag() && !isOpincRootFlag_;
    if (flag) {
        SetParentSubTreeDirty();
    }
    return flag;
}

bool RSRenderNode::OpincGetRootFlag() const
{
    return isOpincRootFlag_;
}

// arkui mark
void RSRenderNode::MarkSuggestOpincNode(bool isOpincNode, bool isNeedCalculate)
{
    RS_TRACE_NAME_FMT("mark opinc %llx, isopinc:%d. isCal:%d", GetId(), isOpincNode, isNeedCalculate);
    isSuggestOpincNode_ = isOpincNode;
    isNeedCalculate_ = isNeedCalculate;
    ROSEN_LOGD("Node id %{public}" PRIu64 " set dirty, mark suggest opinc node", GetId());
    SetDirty();
}

bool RSRenderNode::GetSuggestOpincNode() const
{
    return isSuggestOpincNode_;
}

void RSRenderNode::NodeCacheStateChange(NodeChangeType type)
{
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
    RS_TRACE_NAME_FMT("NodeCacheStateChange %llx, type:%d r%d unc:%d uncU:%d",
        GetId(), type, nodeCacheState_, unchangeCount_, unchangeCountUpper_);
#endif
    switch (type) {
        case NodeChangeType::KEEP_UNCHANGE:
            unchangeCount_++;
            if (unchangeCount_ > unchangeCountUpper_) {
                nodeCacheState_ = NodeCacheState::STATE_UNCHANGE;
            }
            break;
        case NodeChangeType::SELF_DIRTY:
            NodeCacheStateReset(NodeCacheState::STATE_CHANGE);
            break;
    }
}

void RSRenderNode::SetCacheStateByRetrytime()
{
    do {
        tryCacheTimes_++;
        if (tryCacheTimes_ < MAX_TRY_TIMES) {
            unchangeCountUpper_ = unchangeCountUpper_ + MIN_REUSECOUNT;
            break;
        }
        unchangeCountUpper_ = unchangeCountUpper_ + MAX_REUSECOUNT;
    } while (false);
}

void RSRenderNode::NodeCacheStateReset(NodeCacheState nodeCacheState)
{
    nodeCacheState_ = nodeCacheState;
    unchangeCount_ = 0;
    isUnchangeMarkInApp_ = false;
    isUnchangeMarkEnable_ = false;
    if (OpincGetRootFlag()) {
        SetCacheStateByRetrytime();
    }
    if (stagingRenderParams_) {
        stagingRenderParams_->OpincSetCacheChangeFlag(true, lastFrameSynced_);
        stagingRenderParams_->OpincUpdateRootFlag(false);
    }
    isOpincRootFlag_ = false;
}
} // namespace Rosen
} // namespace OHOS
