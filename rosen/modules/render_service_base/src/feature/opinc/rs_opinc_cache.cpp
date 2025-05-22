/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "feature/opinc/rs_opinc_cache.h"
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
#include "string_utils.h"
#endif
#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {

constexpr int64_t MIN_REUSECOUNT = 10;
constexpr int64_t MAX_REUSECOUNT = 20;
constexpr int32_t MAX_TRY_TIMES = 3;
constexpr int32_t MIN_UNCHANGE_COUNT = 3;
constexpr int32_t MAX_UNCHANGE_COUNT = 100;
constexpr int32_t INIT_WAIT_COUNT = 60;

// mark stable node
void RSOpincCache::OpincSetInAppStateStart(bool& unchangeMarkInApp)
{
    if (unchangeMarkInApp) {
        return;
    }
    isUnchangeMarkInApp_ = true;
    unchangeMarkInApp = true;
}

void RSOpincCache::OpincSetInAppStateEnd(bool& unchangeMarkInApp)
{
    if (isUnchangeMarkInApp_) {
        isUnchangeMarkInApp_ = false;
        unchangeMarkInApp = false;
    }
}

void RSOpincCache::OpincQuickMarkStableNode(bool& unchangeMarkInApp, bool& unchangeMarkEnable,
    bool isSelfDirty)
{
    if (!unchangeMarkInApp) {
        return;
    }
    if (IsSuggestOpincNode() && !unchangeMarkEnable) {
        isUnchangeMarkEnable_ = true;
        unchangeMarkEnable = true;
    }
    if (!unchangeMarkEnable) {
        return;
    }
    if (isSelfDirty) {
        NodeCacheStateChange(NodeChangeType::SELF_DIRTY);
        isReseted_ = true;
    } else if (nodeCacheState_ != NodeCacheState::STATE_UNCHANGE) {
        NodeCacheStateChange(NodeChangeType::KEEP_UNCHANGE);
        isReseted_ = false;
    } else {
        if (waitCount_ > 0) {
            waitCount_--;
        } else {
            unchangeCountUpper_ = MIN_UNCHANGE_COUNT;
        }
        isReseted_ = false;
    }
}

void RSOpincCache::OpincUpdateRootFlag(bool& unchangeMarkEnable, bool isOpincNodeSupportFlag)
{
    if (unchangeMarkEnable) {
        if (IsOpincUnchangeState() && isOpincNodeSupportFlag) {
            isOpincRootFlag_ = true;
        }
    }
    if (isUnchangeMarkEnable_) {
        isUnchangeMarkEnable_ = false;
        unchangeMarkEnable = false;
    }
}

bool RSOpincCache::IsOpincUnchangeState()
{
    return (nodeCacheState_ == NodeCacheState::STATE_UNCHANGE) && IsSuggestOpincNode();
}

bool RSOpincCache::IsMarkedRenderGroup(bool groupTypeNotNone)
{
    return groupTypeNotNone || isOpincRootFlag_;
}

bool RSOpincCache::OpincForcePrepareSubTree(bool autoCacheEnable, bool isDirty, bool supportFlag)
{
    if (!autoCacheEnable) {
        return false;
    }
    // Non-root nodes that support opinc and are marked by arkui need to forcibly prepare subtrees
    // until they are marked as root
    return (!isDirty) && IsSuggestOpincNode() && supportFlag && !isOpincRootFlag_;
}

bool RSOpincCache::OpincGetRootFlag() const
{
    return isOpincRootFlag_;
}

bool RSOpincCache::GetCacheChangeFlag() const
{
    return cacheChangeFlag_;
}

// arkui mark
void RSOpincCache::MarkSuggestOpincNode(bool isOpincNode, bool isNeedCalculate)
{
    isSuggestOpincNode_ = isOpincNode;
    isNeedCalculate_ = isNeedCalculate;
}

bool RSOpincCache::IsSuggestOpincNode() const
{
    return isSuggestOpincNode_;
}

void RSOpincCache::NodeCacheStateChange(NodeChangeType type)
{
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
    RS_TRACE_NAME_FMT("NodeCacheStateChange type:%d r%d unc:%d uncU:%d",
        type, nodeCacheState_, unchangeCount_, unchangeCountUpper_);
#endif
    switch (type) {
        case NodeChangeType::KEEP_UNCHANGE:
            unchangeCount_++;
            if (unchangeCount_ > unchangeCountUpper_) {
                nodeCacheState_ = NodeCacheState::STATE_UNCHANGE;
            }
            cacheChangeFlag_ = false;
            break;
        case NodeChangeType::SELF_DIRTY:
            NodeCacheStateReset(NodeCacheState::STATE_CHANGE);
            break;
    }
}

void RSOpincCache::SetCacheStateByRetrytime()
{
    tryCacheTimes_++;
    if (unchangeCountUpper_ > MAX_UNCHANGE_COUNT) {
        return;
    }
    if (tryCacheTimes_ < MAX_TRY_TIMES) {
        unchangeCountUpper_ = unchangeCountUpper_ + MIN_REUSECOUNT;
        return;
    }
    unchangeCountUpper_ = unchangeCountUpper_ + MAX_REUSECOUNT;
}

void RSOpincCache::NodeCacheStateReset(NodeCacheState nodeCacheState)
{
    if (nodeCacheState_ == NodeCacheState::STATE_UNCHANGE) {
        waitCount_ = INIT_WAIT_COUNT;
    }
    nodeCacheState_ = nodeCacheState;
    unchangeCount_ = 0;
    isUnchangeMarkInApp_ = false;
    isUnchangeMarkEnable_ = false;
    if (OpincGetRootFlag()) {
        SetCacheStateByRetrytime();
    }
    cacheChangeFlag_ = true;
    isOpincRootFlag_ = false;
}
} // namespace Rosen
} // namespace OHOS
