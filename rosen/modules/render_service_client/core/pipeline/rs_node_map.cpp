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
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {

RSNodeMap::RSNodeMap()
{
    // add animation fallback node
    nodeMap_.emplace(0, new RSNode(0));
}

RSNodeMap& RSNodeMap::Instance()
{
    static RSNodeMap renderThread;
    return renderThread;
}

bool RSNodeMap::RegisterNode(const RSBaseNode::SharedPtr& nodePtr)
{
    NodeId id = nodePtr->GetId();
    auto itr = nodeMap_.find(id);
    if (itr == nodeMap_.end()) {
        RSBaseNode::WeakPtr ptr(nodePtr);
        nodeMap_.emplace(id, ptr);
        return true;
    }
    return false;
}

void RSNodeMap::UnregisterNode(NodeId id)
{
    auto itr = nodeMap_.find(id);
    if (itr != nodeMap_.end()) {
        nodeMap_.erase(itr);
    }
}

RSBaseNode::WeakPtr RSNodeMap::GetNode(NodeId id)
{
    auto itr = nodeMap_.find(id);
    if (itr != nodeMap_.end()) {
        return itr->second;
    }

    return RSBaseNode::WeakPtr();
}

std::shared_ptr<RSPropertyNode> RSNodeMap::GetAnimationFallbackNode()
{
    return std::static_pointer_cast<RSPropertyNode>(nodeMap_.at(0));
}

} // namespace Rosen
} // namespace OHOS
