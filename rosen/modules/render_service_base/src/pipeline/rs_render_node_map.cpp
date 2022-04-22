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

#include "pipeline/rs_render_node_map.h"

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSRenderNodeMap::RSRenderNodeMap()
{
    // add animation fallback node
    renderNodeMap_.emplace(0, new RSCanvasRenderNode(0));
}

bool RSRenderNodeMap::RegisterRenderNode(const std::shared_ptr<RSBaseRenderNode>& nodePtr)
{
    NodeId id = nodePtr->GetId();
    if (renderNodeMap_.find(id) != renderNodeMap_.end()) {
        return false;
    }
    renderNodeMap_.emplace(id, nodePtr);
    return true;
}

void RSRenderNodeMap::UnregisterRenderNode(NodeId id)
{
    renderNodeMap_.erase(id);
}

void RSRenderNodeMap::FilterNodeByPid(pid_t pid)
{
    ROSEN_LOGI("RSRenderNodeMap::FilterNodeByPid removing all nodes belong to pid %d", pid);
    // remove all nodes belong to given pid (by matching higher 32 bits of node id)
    std::__libcpp_erase_if_container(renderNodeMap_, [pid](const auto& pair) -> bool {
        if (static_cast<pid_t>(pair.first >> 32) != pid) {
            return false;
        }
        pair.second->RemoveFromTree();
        return true;
    });
}

void RSRenderNodeMap::DumpNodeNotOnTree(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- Node Not On Tree\n");
    for (auto it = renderNodeMap_.begin(); it != renderNodeMap_.end(); it++) {
        if ((*it).second->GetType() == RSRenderNodeType::SURFACE_NODE && !(*it).second->IsOnTheTree()) {
            dumpString += "\n node Id[" + std::to_string((*it).first) + "]:\n";
            auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>((*it).second);
            auto& surfaceConsumer = node->GetConsumer();
            if (surfaceConsumer == nullptr) {
                continue;
            }
            surfaceConsumer->Dump(dumpString);
        }
    }
}

template<>
const std::shared_ptr<RSBaseRenderNode> RSRenderNodeMap::GetRenderNode(NodeId id) const
{
    auto itr = renderNodeMap_.find(id);
    if (itr == renderNodeMap_.end()) {
        return nullptr;
    }
    return itr->second;
}

const std::shared_ptr<RSRenderNode> RSRenderNodeMap::GetAnimationFallbackNode() const
{
    auto itr = renderNodeMap_.find(0);
    if (itr == renderNodeMap_.end()) {
        return nullptr;
    }
    return std::static_pointer_cast<RSRenderNode>(renderNodeMap_.at(0));
}

} // namespace Rosen
} // namespace OHOS
