/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "command/rs_depth_node_command.h"

#include "pipeline/rs_depth_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
void RSDepthNodeCommandHelper::Create(RSContext& context, NodeId id, bool isTextureExportNode)
{
    auto node = std::shared_ptr<RSDepthRenderNode>(new RSDepthRenderNode(id, context.weak_from_this(),
        isTextureExportNode), RSRenderNodeGC::NodeDestructor);
    context.GetMutableNodeMap().RegisterRenderNode(node);
}

void RSDepthNodeCommandHelper::SetType(RSContext& context, NodeId id, DepthSpaceType depthSpaceType)
{
    if (depthSpaceType < DepthSpaceType::DEPTH_SPACE_TYPE_MIN ||
        depthSpaceType > DepthSpaceType::DEPTH_SPACE_TYPE_MAX) {
        RS_LOGE("RSDepthNodeCommandHelper::SetType invalid depthSpaceType=%{public}d",
            static_cast<int32_t>(depthSpaceType));
        return;
    }
    if (const auto& node = context.GetNodeMap().GetRenderNode<RSDepthRenderNode>(id)) {
        node->SetDepthSpaceType(depthSpaceType);
    }
}
} // namespace Rosen
} // namespace OHOS