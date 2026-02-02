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

#include "feature/render_group/rs_render_group_cache.h"

namespace OHOS {
namespace Rosen {

bool RSRenderGroupCache::ExcludedFromNodeGroup(bool isExcluded)
{
    if (excludedFromNodeGroup_ == isExcluded) {
        return false;
    }
    excludedFromNodeGroup_ = isExcluded;
    isExcludedStateChanged_ = true;
    return true;
}

bool RSRenderGroupCache::SetHasChildExcludedFromNodeGroup(bool isExcluded)
{
    if (hasChildExcludedFromNodeGroup_ == isExcluded) {
        return false;
    }
    hasChildExcludedFromNodeGroup_ = isExcluded;
    return true;
}

bool RSRenderGroupCache::SetRenderGroupExcludedStateChanged(bool isChanged)
{
    if (isExcludedStateChanged_ == isChanged) {
        return false;
    }
    isExcludedStateChanged_ = isChanged;
    return true;
}

bool RSRenderGroupCache::SetCachedSubTreeDirty(bool isDirty)
{
    if (isCachedSubTreeDirty_ == isDirty) {
        return false;
    }
    isCachedSubTreeDirty_ = isDirty;
    return true;
}

bool RSRenderGroupCache::SetChildHasTranslateOnSqueeze(bool val)
{
    if (childHasTranslateOnSqueeze_ == val) {
        return false;
    }
    childHasTranslateOnSqueeze_ = val;
    return true;
}

AutoRenderGroupExcludedSubTreeGuard::AutoRenderGroupExcludedSubTreeGuard(
    NodeId& curExcludedRootNodeId, bool isCurNodeExcluded, NodeId curNodeId)
    : curExcludedRootNodeId_(curExcludedRootNodeId)
{
    if (curExcludedRootNodeId_ == INVALID_NODEID && isCurNodeExcluded) {
        curExcludedRootNodeId_ = curNodeId;
        isExcluded_ = true;
    }
}

AutoRenderGroupExcludedSubTreeGuard::~AutoRenderGroupExcludedSubTreeGuard()
{
    if (isExcluded_) {
        curExcludedRootNodeId_ = INVALID_NODEID;
    }
}
} // namespace Rosen
} // namespace OHOS