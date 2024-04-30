/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "command/rs_proxy_node_command.h"

#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
void ProxyNodeCommandHelper::Create(RSContext& context, NodeId id, NodeId targetId)
{
    // PLANNING: if we run in RS and target not found, we should display a warning
    auto targetNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(targetId);
    auto node = std::shared_ptr<RSProxyRenderNode>(new RSProxyRenderNode(id,
        targetNode, targetId, context.weak_from_this()), RSRenderNodeGC::NodeDestructor);
    context.GetMutableNodeMap().RegisterRenderNode(node);
}

void ProxyNodeCommandHelper::ResetContextVariableCache(RSContext& context, NodeId id)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSProxyRenderNode>(id)) {
        node->ResetContextVariableCache();
    }
}
} // namespace Rosen
} // namespace OHOS
