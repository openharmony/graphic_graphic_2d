/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "ui/rs_base_node.h"
#include "ui/rs_canvas_node.h"

namespace OHOS {
namespace Rosen {
RSNodeMap::RSNodeMap()
{
    // add animation fallback node
    auto fallback_node = new RSCanvasNode(false);
    fallback_node->SetId(0);
    nodeMap_.emplace(0, fallback_node);
}

RSNodeMap::~RSNodeMap()  noexcept
{
    for (auto &[id, node] : nodeMap_) {
        node = nullptr;
    }
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
    NodeId id = nodePtr->GetId();
    auto itr = nodeMap_.find(id);
    if (itr != nodeMap_.end()) {
        return false;
    }
    RSBaseNode::WeakPtr ptr(nodePtr);
    nodeMap_.emplace(id, ptr);
    return true;
}

void RSNodeMap::UnregisterNode(NodeId id)
{
    auto itr = nodeMap_.find(id);
    if (itr != nodeMap_.end()) {
        nodeMap_.erase(itr);
    }
}

template<>
const std::shared_ptr<RSBaseNode> RSNodeMap::GetNode<RSBaseNode>(NodeId id) const
{
    auto itr = nodeMap_.find(id);
    if (itr == nodeMap_.end()) {
        return nullptr;
    }
    return itr->second;
}

const std::shared_ptr<RSNode> RSNodeMap::GetAnimationFallbackNode() const
{
    auto itr = nodeMap_.find(0);
    if (itr == nodeMap_.end()) {
        return nullptr;
    }
    return std::static_pointer_cast<RSNode>(nodeMap_.at(0));
}

} // namespace Rosen
} // namespace OHOS
