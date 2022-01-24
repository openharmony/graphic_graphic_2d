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

#include "pipeline/rs_render_node_map.h"

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_render_node.h"

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

template<>
std::shared_ptr<RSBaseRenderNode> RSRenderNodeMap::GetRenderNode(NodeId id)
{
    auto itr = renderNodeMap_.find(id);
    if (itr == renderNodeMap_.end()) {
        return nullptr;
    }
    return itr->second;
}

std::shared_ptr<RSRenderNode> RSRenderNodeMap::GetAnimationFallbackNode()
{
    return std::static_pointer_cast<RSRenderNode>(renderNodeMap_.at(0));
}

} // namespace Rosen
} // namespace OHOS
