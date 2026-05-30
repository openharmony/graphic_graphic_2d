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

#include "rsbasenodecommand_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <securec.h>

#include "command/rs_base_node_command.h"
#include "pipeline/rs_base_render_node.h"

namespace OHOS {
namespace Rosen {
bool DoBaseNodeCommand(FuzzedDataProvider& fdp)
{
    RSContext context;
    NodeId nodeId = fdp.ConsumeIntegral<NodeId>();
    NodeId childNodeId = fdp.ConsumeIntegral<NodeId>();
    int32_t index = fdp.ConsumeIntegral<int32_t>();
    NodeId newParentId = fdp.ConsumeIntegral<NodeId>();
    bool isCrossNode = fdp.ConsumeBool();
    NodeId cloneNodeId = fdp.ConsumeIntegral<NodeId>();
    bool autoClearCloneNode = fdp.ConsumeBool();

    auto node = std::make_shared<RSBaseRenderNode>(nodeId);
    context.GetMutableNodeMap().RegisterRenderNode(node);
    auto childNode = std::make_shared<RSBaseRenderNode>(childNodeId);
    context.GetMutableNodeMap().RegisterRenderNode(childNode);

    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    BaseNodeCommandHelper::MoveChild(context, nodeId, childNodeId, index);
    BaseNodeCommandHelper::RemoveChild(context, nodeId, childNodeId);
    BaseNodeCommandHelper::AddCrossParentChild(context, nodeId, childNodeId, index);
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    BaseNodeCommandHelper::SetIsCrossNode(context, nodeId, isCrossNode);
    BaseNodeCommandHelper::AddCrossScreenChild(context, nodeId, childNodeId, cloneNodeId,
        index, autoClearCloneNode);
    BaseNodeCommandHelper::RemoveCrossScreenChild(context, nodeId, childNodeId);
    BaseNodeCommandHelper::RemoveFromTree(context, nodeId);
    BaseNodeCommandHelper::ClearChildren(context, nodeId);
    BaseNodeCommandHelper::Destroy(context, nodeId);
    return true;
}
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OHOS::Rosen::DoBaseNodeCommand(fdp);
    return 0;
}
