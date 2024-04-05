/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_node_map.h"

#include <atomic>

#include "ui/rs_base_node.h"
#include "ui/rs_canvas_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
static std::atomic_bool g_instance_valid = false;
}

RSNodeMap::RSNodeMap()
{
    // create animation fallback node
    auto fallback_node = new RSCanvasNode(false);
    fallback_node->SetId(0);
    animationFallbackNode_.reset(fallback_node);
    nodeMap_.emplace(0, animationFallbackNode_);
    g_instance_valid.store(true);
}

RSNodeMap::~RSNodeMap()  noexcept
{
    animationFallbackNode_ = nullptr;
    std::unique_lock<std::mutex> lock(mutex_);
    nodeMap_.clear();
    g_instance_valid.store(false);
}

RSNodeMap& RSNodeMap::MutableInstance()
{
    static RSNodeMap nodeMap;
    return nodeMap;
}

const RSNodeMap& RSNodeMap::Instance()
{
    return MutableInstance();
}

bool RSNodeMap::RegisterNode(const RSBaseNode::SharedPtr& nodePtr)
{
    std::unique_lock<std::mutex> lock(mutex_);
    NodeId id = nodePtr->GetId();
    auto itr = nodeMap_.find(id);
    if (itr != nodeMap_.end()) {
        ROSEN_LOGW("RSNodeMap::RegisterNode: node id %{public}" PRIu64 " already exists", id);
        return false;
    }
    RSBaseNode::WeakPtr ptr(nodePtr);
    nodeMap_.emplace(id, ptr);
    return true;
}

bool RSNodeMap::RegisterNodeInstanceId(NodeId id, int32_t instanceId)
{
    std::unique_lock<std::mutex> lock(mutex_);
    nodeIdMap_.emplace(id, instanceId);
    return true;
}

void RSNodeMap::UnregisterNode(NodeId id)
{
    if (!g_instance_valid.load()) {
        return;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    auto itr = nodeMap_.find(id);
    if (itr != nodeMap_.end()) {
        nodeIdMap_.erase(id);
        nodeMap_.erase(itr);
    } else {
        ROSEN_LOGW("RSNodeMap::UnregisterNode: node id %{public}" PRIu64 " not found", id);
    }
}

template<>
const std::shared_ptr<RSBaseNode> RSNodeMap::GetNode<RSBaseNode>(NodeId id) const
{
    if (!g_instance_valid.load()) {
        return nullptr;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    auto itr = nodeMap_.find(id);
    if (itr == nodeMap_.end()) {
        return nullptr;
    }
    return itr->second.lock();
}

int32_t RSNodeMap::GetNodeInstanceId(NodeId id) const
{
    if (!g_instance_valid.load()) {
        return INSTANCE_ID_UNDEFINED;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    auto itr = nodeIdMap_.find(id);
    if (itr == nodeIdMap_.end()) {
        return INSTANCE_ID_UNDEFINED;
    }
    return itr->second;
}

const std::shared_ptr<RSNode> RSNodeMap::GetAnimationFallbackNode() const
{
    return animationFallbackNode_;
}

} // namespace Rosen
} // namespace OHOS
