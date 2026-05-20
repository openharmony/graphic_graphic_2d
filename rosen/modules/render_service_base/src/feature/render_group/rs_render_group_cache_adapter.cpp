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

#include "feature/render_group/rs_render_group_cache_adapter.h"

namespace OHOS {
namespace Rosen {

std::vector<FilterNodeInfo>& RSRenderGroupCacheAdapter::GetFilterInfoVec()
{
    return filterInfoVec_;
}

void RSRenderGroupCacheAdapter::ClearFilterInfoVec()
{
    filterInfoVec_.clear();
}

std::unordered_map<NodeId, Drawing::Matrix>& RSRenderGroupCacheAdapter::GetWithoutFilterMatrixMap()
{
    return withoutFilterMatrixMap_;
}

size_t RSRenderGroupCacheAdapter::GetFilterNodeSize() const
{
    return filterNodeSize_;
}

void RSRenderGroupCacheAdapter::SetFilterNodeSize(size_t size)
{
    filterNodeSize_ = size;
}

void RSRenderGroupCacheAdapter::ReduceFilterNodeSize()
{
    if (filterNodeSize_ > 0) {
        --filterNodeSize_;
    }
}

void RSRenderGroupCacheAdapter::SetLastDrawnFilterNodeId(NodeId nodeId)
{
    lastDrawnFilterNodeId_ = nodeId;
}

NodeId RSRenderGroupCacheAdapter::GetLastDrawnFilterNodeId() const
{
    return lastDrawnFilterNodeId_;
}
} // namespace Rosen
} // namespace OHOS