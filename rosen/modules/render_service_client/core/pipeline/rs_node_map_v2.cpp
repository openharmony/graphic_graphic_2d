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

#include "pipeline/rs_node_map_v2.h"

#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "ui/rs_base_node.h"

namespace OHOS {
namespace Rosen {

RSNodeMapV2::RSNodeMapV2()
{
}

RSNodeMapV2::~RSNodeMapV2() noexcept
{
    std::unique_lock<std::mutex> lock(mutex_);
    nodeMapNew_.clear();
}

bool RSNodeMapV2::RegisterNode(const RSBaseNode::SharedPtr& nodePtr)
{
    std::unique_lock<std::mutex> lock(mutex_);
    NodeId id = nodePtr->GetId();
    auto itr = nodeMapNew_.find(id);
    if (itr != nodeMapNew_.end()) {
        ROSEN_LOGW("RSNodeMapV2::RegisterNode: node id %{public}" PRIu64 " already exists", id);
        return false;
    }
    RSBaseNode::WeakPtr ptr(nodePtr);
    nodeMapNew_.emplace(id, ptr);
    return true;
}

void RSNodeMapV2::UnregisterNode(NodeId id)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto itr = nodeMapNew_.find(id);
    if (itr != nodeMapNew_.end()) {
        nodeMapNew_.erase(itr);
    } else {
        ROSEN_LOGW("RSNodeMapV2::UnregisterNode: node id %{public}" PRIu64 " not found", id);
    }
}

template<>
const std::shared_ptr<RSBaseNode> RSNodeMapV2::GetNode<RSBaseNode>(NodeId id) const
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto itr = nodeMapNew_.find(id);
    if (itr == nodeMapNew_.end()) {
        return nullptr;
    }
    return itr->second.lock();
}
} // namespace Rosen
} // namespace OHOS
